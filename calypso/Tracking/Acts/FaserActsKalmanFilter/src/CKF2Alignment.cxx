#include "CKF2Alignment.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointCollection.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/Surface.h"
#include "Identifier/Identifier.h"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "FaserActsKalmanFilter/IndexSourceLink.h"
#include "FaserActsKalmanFilter/Measurement.h"
#include "FaserActsRecMultiTrajectory.h"
#include "TrackSelection.h"
#include <algorithm>

#include "FaserActsGeometry/FASERMagneticFieldWrapper.h"

#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/RectangleBounds.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/TrackFitting/GainMatrixSmoother.hpp"
#include "Acts/TrackFitting/GainMatrixUpdater.hpp"


#include "Acts/EventData/Measurement.hpp"
#include "Acts/Definitions/Units.hpp"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include <fstream>
#include <iostream>

size_t CKF2Alignment::TrajectoryInfo::nClusters {0};

using TrajectoriesContainer = std::vector<FaserActsRecMultiTrajectory>;

using namespace Acts::UnitLiterals;

CKF2Alignment::CKF2Alignment(
    const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator), m_thistSvc("THistSvc", name) {}


  StatusCode CKF2Alignment::initialize() {
    ATH_CHECK(m_fieldCondObjInputKey.initialize());
    ATH_CHECK(m_trackingGeometryTool.retrieve());
    ATH_CHECK(m_extrapolationTool.retrieve());
    ATH_CHECK(m_trackSeedTool.retrieve());
    ATH_CHECK(m_kalmanFitterTool1.retrieve());
    ATH_CHECK(m_createTrkTrackTool.retrieve());
    ATH_CHECK(detStore()->retrieve(m_idHelper,"FaserSCT_ID"));
    m_fit = makeTrackFinderFunction(m_trackingGeometryTool->trackingGeometry(),
	m_resolvePassive, m_resolveMaterial, m_resolveSensitive);
    m_kf = makeTrackFitterFunction(m_trackingGeometryTool->trackingGeometry());
    // FIXME fix Acts logging level
    if (m_actsLogging == "VERBOSE") {
      m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::VERBOSE);
    } else if (m_actsLogging == "DEBUG") {
      m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::DEBUG);
    } else {
      m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::INFO);
    }
    CHECK(m_thistSvc.retrieve());
    m_tree= new TTree("trackParam","tree");
    initializeTree();
    CHECK(m_thistSvc->regTree("/CKF2Alignment/trackParam",m_tree));
    m_tree->AutoSave();
    return StatusCode::SUCCESS;
  }


StatusCode CKF2Alignment::execute() {
  clearVariables();
  const EventContext& ctx = Gaudi::Hive::currentContext();
  m_numberOfEvents++;

  ATH_MSG_DEBUG("get the tracking geometry");
  bool save=false;
  std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry
    = m_trackingGeometryTool->trackingGeometry();

  const FaserActsGeometryContext& faserActsGeometryContext = m_trackingGeometryTool->getGeometryContext();
  auto gctx = faserActsGeometryContext.context();
  Acts::MagneticFieldContext magFieldContext = getMagneticFieldContext(ctx);
  Acts::CalibrationContext calibContext;
  ATH_MSG_DEBUG("get the seed");

  CHECK(m_trackSeedTool->run());
  std::shared_ptr<const Acts::Surface> initialSurface =
    m_trackSeedTool->initialSurface();
  std::shared_ptr<std::vector<Acts::CurvilinearTrackParameters>> initialParameters =
    m_trackSeedTool->initialTrackParameters();
  std::shared_ptr<std::vector<IndexSourceLink>> sourceLinks =
    m_trackSeedTool->sourceLinks();
  double origin = m_trackSeedTool->targetZPosition();

  ATH_MSG_DEBUG("get the output of seeding");
  std::shared_ptr<std::vector<Measurement>> measurements = m_trackSeedTool->measurements();
  std::shared_ptr<std::vector<const Tracker::FaserSCT_Cluster*>> clusters = m_trackSeedTool->clusters();
  std::vector<const Tracker::FaserSCT_Cluster*> clusters_sta0;
  clusters_sta0.clear();
  std::shared_ptr<std::vector<const Tracker::FaserSCT_SpacePoint*>> spacePoints = m_trackSeedTool->spacePoints();
  std::shared_ptr<std::vector<std::array<std::vector<const Tracker::FaserSCT_Cluster*>, 3>>> seedClusters = m_trackSeedTool->seedClusters();

  TrajectoryInfo::nClusters = sourceLinks->size();
  TrajectoriesContainer trajectories;
  trajectories.reserve(initialParameters->size());
  if(sourceLinks->size()<15||sourceLinks->size()>40)return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("size of clusters/sourcelinks "<<clusters->size()<<"/"<<sourceLinks->size());

  Acts::PropagatorPlainOptions pOptions;
  pOptions.maxSteps = m_maxSteps;

  Acts::MeasurementSelector::Config measurementSelectorCfg = {
    {Acts::GeometryIdentifier(), {m_chi2Max, m_nMax}},
  };


  // Set the CombinatorialKalmanFilter options
  CKF2Alignment::TrackFinderOptions options(
      gctx, magFieldContext, calibContext,
      IndexSourceLinkAccessor(), MeasurementCalibrator(*measurements),
      Acts::MeasurementSelector(measurementSelectorCfg),
      Acts::LoggerWrapper{*m_logger}, pOptions, &(*initialSurface));

  // Perform the track finding for all initial parameters
  m_numberOfTrackSeeds += initialParameters->size();
  ATH_MSG_DEBUG("Invoke track finding with " << initialParameters->size() << " seeds.");
  IndexSourceLinkContainer tmp;
  for (const auto& sl : *sourceLinks) {
    tmp.emplace_hint(tmp.end(), sl);
  }

  for (const auto& init : *initialParameters) {
    ATH_MSG_DEBUG("  position: " << init.position(gctx).transpose());
    ATH_MSG_DEBUG("  momentum: " << init.momentum().transpose());
    ATH_MSG_DEBUG("  charge:   " << init.charge());
  }

  ATH_MSG_DEBUG("sourcelink size "<<tmp.size());
  auto results = (*m_fit)(tmp, *initialParameters, options);

  // results contain a MultiTrajectory for each track seed with a trajectory of each branch of the CKF.
  // To simplify the ambiguity solving a list of MultiTrajectories is created, each containing only a single track.
  std::list<TrajectoryInfo> allTrajectories;
  for (auto &result : results) {
    if (not result.ok()) {
      continue;
    }
    CKFResult ckfResult = result.value();
    for (size_t trackTip : ckfResult.lastMeasurementIndices) {
      allTrajectories.emplace_back(TrajectoryInfo(FaserActsRecMultiTrajectory(
	      ckfResult.fittedStates, {trackTip}, {{trackTip, ckfResult.fittedParameters.at(trackTip)}})));
    }
  }
  m_numberOfFittedTracks += allTrajectories.size();

  // the list of MultiTrajectories is sorted by the number of measurements using the chi2 value as a tie-breaker
  allTrajectories.sort([](const TrajectoryInfo &left, const TrajectoryInfo &right) {
      if (left.nMeasurements > right.nMeasurements) return true;
      if (left.nMeasurements < right.nMeasurements) return false;
      if (left.chi2 < right.chi2) return true;
      else return false;
      });

  // select all tracks with at least 13 heats and with 6 or less shared hits, starting from the best track
  // TODO use Gaudi parameters for the number of hits and shared hits
  // TODO allow shared hits only in the first station?
  std::vector<FaserActsRecMultiTrajectory> selectedTrajectories {};
  while (not allTrajectories.empty()) {
    TrajectoryInfo selected = allTrajectories.front();
    selectedTrajectories.push_back(selected.trajectory);
    allTrajectories.remove_if([&](const TrajectoryInfo &p) {
	return (p.nMeasurements <= 12) || ((p.clusterSet & selected.clusterSet).count() > 6);
	});
  }


  //only use the first track
  int trackid=0;
  for (const FaserActsRecMultiTrajectory &traj : selectedTrajectories) {
    if(trackid>0)continue;
    trackid++;
    const auto& mj = traj.multiTrajectory();
    const auto& trackTips = traj.tips();
    if(trackTips.empty())continue;
    auto& trackTip = trackTips.front();
    auto trajState = Acts::MultiTrajectoryHelpers::trajectoryState(mj, trackTip);
    if(trajState.nMeasurements<15)continue;

    const auto params = traj.trackParameters(traj.tips().front());
    ATH_MSG_DEBUG("Fitted parameters");
    ATH_MSG_DEBUG("  params:   " << params.parameters().transpose());
    ATH_MSG_DEBUG("  position: " << params.position(gctx).transpose());
    ATH_MSG_DEBUG("  momentum: " << params.momentum().transpose());
    ATH_MSG_DEBUG("  charge:   " << params.charge());
    ATH_MSG_VERBOSE("checkte track");
    std::vector<double> t_align_stationId_sp;
    std::vector<double> t_align_centery_sp;
    std::vector<double> t_align_layerId_sp;
    std::vector<double> t_align_moduleId_sp;
    std::vector<double> t_align_clustersize_sp;
    std::vector<double> t_align_stereoId;
    std::vector<double> t_align_global_residual_y_sp;
    std::vector<double> t_align_global_measured_y_sp;
    std::vector<double> t_align_global_measured_x_sp;
    std::vector<double> t_align_global_measured_z_sp;
    std::vector<double> t_align_global_measured_ye_sp;
    std::vector<double> t_align_global_fitted_ye_sp;
    std::vector<double> t_align_global_fitted_y_sp;
    std::vector<double> t_align_global_fitted_x_sp;
    std::vector<double> t_align_local_residual_x_sp;
    std::vector<double> t_align_unbiased;
    std::vector<double> t_align_local_pull_x_sp;
    std::vector<double> t_align_local_measured_x_sp;
    std::vector<double> t_align_local_measured_xe_sp;
    std::vector<double> t_align_local_fitted_xe_sp;
    std::vector<double> t_align_local_fitted_x_sp;
    std::vector<double> t_align_derivation_x_x;
    std::vector<double> t_align_derivation_x_y;
    std::vector<double> t_align_derivation_x_z;
    std::vector<double> t_align_derivation_x_rx;
    std::vector<double> t_align_derivation_x_ry;
    std::vector<double> t_align_derivation_x_rz;
    std::vector<double> t_align_derivation_global_y_x;
    std::vector<double> t_align_derivation_global_y_y;
    std::vector<double> t_align_derivation_global_y_z;
    std::vector<double> t_align_derivation_global_y_rx;
    std::vector<double> t_align_derivation_global_y_ry; 
    std::vector<double> t_align_derivation_global_y_rz;
    bool foundift=false;
    std::unique_ptr<const Acts::BoundTrackParameters> ini_Param=nullptr;
    //get residual from ACTS
    if(m_biased){
      mj.visitBackwards(trackTip, [&](const auto &state) {
	  /// Only fill the track states with non-outlier measurement
	  auto typeFlags = state.typeFlags();
	  if (not typeFlags.test(Acts::TrackStateFlag::MeasurementFlag) and not typeFlags.test(Acts::TrackStateFlag::OutlierFlag))
	  {
	  return true;
	  }

	  const auto& surface = state.referenceSurface();
	  Acts::BoundVector meas = state.projector().transpose() * state.calibrated();
	  Acts::Vector2 local(meas[Acts::eBoundLoc0], meas[Acts::eBoundLoc1]);
	  const Acts::Vector3 dir = Acts::makeDirectionUnitFromPhiTheta(meas[Acts::eBoundPhi], meas[Acts::eBoundTheta]);
	  Acts::Vector3 mom(1, 1, 1);
	  Acts::Vector3 global = surface.localToGlobal(gctx, local, dir);
	  if (state.hasSmoothed())
	  {
	  Acts::BoundTrackParameters parameter(
	      state.referenceSurface().getSharedPtr(),
	      state.smoothed(),
	      state.smoothedCovariance());
	  auto covariance = state.smoothedCovariance();

	  /// Local hit residual info
	  auto H = state.effectiveProjector();
	  auto resCov = state.effectiveCalibratedCovariance() +
	    H * covariance * H.transpose();
	  auto residual = state.effectiveCalibrated() - H * state.smoothed();

	  if (state.hasUncalibrated()) {
	    const Tracker::FaserSCT_Cluster* cluster = state.uncalibrated().hit();
	    auto trans2 = cluster->detectorElement()->surface().transform();
	    auto trans1 = trans2;
	    Identifier id = cluster->identify();
	    int istation = m_idHelper->station(id);
	    int ilayer = m_idHelper->layer(id);
	    int stereoid = m_idHelper->side(id);
	    if(stereoid==1){
	      trans1=cluster->detectorElement()->otherSide()->surface().transform();
	    }
	    ATH_MSG_DEBUG("  save the residual "<<residual(Acts::eBoundLoc0));
	    const auto iModuleEta = m_idHelper->eta_module(id);
	    const auto iModulePhi = m_idHelper->phi_module(id);
	    int iModule = iModulePhi;
	    if (iModuleEta < 0) iModule +=4;
	    ATH_MSG_DEBUG("ID "<<istation<<"/"<<ilayer<<"/"<<iModule<<"/"<<stereoid<<"/"<<ilayer);
	    t_align_stationId_sp.push_back(istation);
	    t_align_centery_sp.push_back(cluster->detectorElement()->center().y());
	    t_align_layerId_sp.push_back(ilayer);
	    t_align_moduleId_sp.push_back(iModule);
	    t_align_clustersize_sp.push_back(cluster->rdoList().size());
	    t_align_stereoId.push_back(stereoid);
	    t_align_global_measured_x_sp.push_back(cluster->globalPosition().x());
	    t_align_global_measured_y_sp.push_back(cluster->globalPosition().y());
	    t_align_global_measured_z_sp.push_back(cluster->globalPosition().z());
	    t_align_global_fitted_x_sp.push_back(global.x());
	    t_align_global_fitted_y_sp.push_back(global.y());
	    t_align_local_residual_x_sp.push_back(residual(Acts::eBoundLoc0));
	    t_align_unbiased.push_back(0);
	    t_align_local_pull_x_sp.push_back(residual(Acts::eBoundLoc0)/sqrt(resCov(Acts::eBoundLoc0, Acts::eBoundLoc0)));
	    t_align_local_measured_x_sp.push_back(cluster->localPosition().x());
	    t_align_local_measured_xe_sp.push_back(sqrt(cluster->localCovariance()(0,0)));
	    t_align_local_fitted_xe_sp.push_back(sqrt(covariance(Acts::eBoundLoc0, Acts::eBoundLoc0)));
	    t_align_local_fitted_x_sp.push_back(meas[Acts::eBoundLoc0]);
	    save=true;
	    Amg::Vector2D local_mea=cluster->localPosition() ; 
	    t_align_derivation_x_x.push_back(getDerivation(ctx, parameter, trans1,local_mea, 0, stereoid, trans2, 0));
	    t_align_derivation_x_y.push_back(getDerivation(ctx, parameter, trans1,local_mea, 1, stereoid, trans2, 0));
	    t_align_derivation_x_z.push_back(getDerivation(ctx, parameter, trans1,local_mea, 2, stereoid, trans2, 0));
	    t_align_derivation_x_rx.push_back(getDerivation(ctx, parameter, trans1,local_mea, 3, stereoid, trans2, 0));
	    t_align_derivation_x_ry.push_back(getDerivation(ctx, parameter, trans1,local_mea, 4, stereoid, trans2, 0));
	    t_align_derivation_x_rz.push_back(getDerivation(ctx, parameter, trans1,local_mea, 5, stereoid, trans2, 0));
	    t_align_derivation_global_y_x.push_back(getDerivation(ctx, parameter, trans1,local_mea, 0, stereoid, trans2, 1));
	    t_align_derivation_global_y_y.push_back(getDerivation(ctx, parameter, trans1,local_mea, 1, stereoid, trans2, 1));
	    t_align_derivation_global_y_z.push_back(getDerivation(ctx, parameter, trans1,local_mea, 2, stereoid, trans2, 1));
	    t_align_derivation_global_y_rx.push_back(getDerivation(ctx, parameter, trans1,local_mea, 3, stereoid, trans2, 1));
	    t_align_derivation_global_y_ry.push_back(getDerivation(ctx, parameter, trans1,local_mea, 4, stereoid, trans2, 1));
	    t_align_derivation_global_y_rz.push_back(getDerivation(ctx, parameter, trans1,local_mea, 5, stereoid, trans2, 1));
	    ATH_MSG_VERBOSE("local derivation "<<t_align_derivation_x_x.back()<<" "<<t_align_derivation_x_y.back()<<" "<<t_align_derivation_x_z.back()<<" "<<t_align_derivation_x_rx.back()<<" "<<t_align_derivation_x_ry.back()<<" "<<t_align_derivation_x_rz.back());
	    ATH_MSG_VERBOSE("global derivation "<<t_align_derivation_global_y_x.back()<<" "<<t_align_derivation_global_y_y.back()<<" "<<t_align_derivation_global_y_z.back()<<""<<t_align_derivation_global_y_rx.back()<<" "<<t_align_derivation_global_y_ry.back()<<" "<<t_align_derivation_global_y_rz.back());
	    //get the residual for IFT
	    if(istation==1&&ilayer==0&&!foundift){
	      Amg::Transform3D ini_trans = Amg::Translation3D(0,0, -1910.) * Amg::RotationMatrix3D::Identity();
	      auto ini_surface = Acts::Surface::makeShared<Acts::PlaneSurface>(ini_trans, std::make_shared<const Acts::RectangleBounds>(1000.,1000.));
	      ini_Param = m_extrapolationTool->propagate( ctx, parameter, *ini_surface, Acts::backward);
	      //std::unique_ptr<const Acts::BoundTrackParameters> ini_Param = m_extrapolationTool->propagate( ctx, parameter, *ini_surface, Acts::backward);
	      //find the closet cluster
	      if(ini_Param!=nullptr){
		for(int i=0;i<3;i++){
		  for(int st=0;st<2;st++){
		    double chisq=999.;
		    size_t iclus=999;
		    double fitx=-999.;
		    double fity=-999.;
		    double gfitx=-999.;
		    double gfity=-999.;
		    double resi=-999.;
		    for(size_t ic =0 ;ic <clusters->size();ic++){
		      auto cluster = clusters->at(ic);
		      Identifier id = cluster->identify();
		      int jstation = m_idHelper->station(id);
		      int jlayer = m_idHelper->layer(id);
		      if(jstation!=0||jlayer!=i)continue;
		      int jstereoid = m_idHelper->side(id);
		      if(st==0){if(jstereoid==1)continue;}
		      if(st==1){if(jstereoid!=1)continue;}
		      auto trans2 = cluster->detectorElement()->surface().transform();
		      Amg::Transform3D shift_trans = Amg::Translation3D(0,0, cluster->globalPosition().z()) * Amg::RotationMatrix3D::Identity();
		      auto shift_surface = Acts::Surface::makeShared<Acts::PlaneSurface>(shift_trans, std::make_shared<const Acts::RectangleBounds>(1000.,1000.));
		      auto shift_Param = m_extrapolationTool->propagate( ctx, *ini_Param, *shift_surface);
		      if(shift_Param!=nullptr){
			auto shift_parameter = shift_Param->parameters();
			Amg::Vector2D local_m(shift_parameter[Acts::eBoundLoc0],shift_parameter[Acts::eBoundLoc1]);
			Amg::Vector3D glopos(local_m.x(),local_m.y(),cluster->globalPosition().z());
			auto localpos=trans2.inverse()*glopos;
			ATH_MSG_DEBUG("looping cluster "<<local_m.x()<<" "<<local_m.y()<<" : "<<cluster->globalPosition());
			ATH_MSG_DEBUG("local   cluster "<<localpos.x()<<" "<<localpos.y()<<" : "<<cluster->localPosition());
			double dist2 =(glopos.x()-cluster->globalPosition().x())*(glopos.x()-cluster->globalPosition().x())/40./40.+(glopos.y()-cluster->globalPosition().y())*(glopos.y()-cluster->globalPosition().y());
			if(dist2<chisq){
			  chisq=dist2;
			  iclus=ic;
			  fitx=localpos.x();
			  fity=localpos.y();
			  gfitx=glopos.x();
			  gfity=glopos.y();
			  resi=cluster->localPosition().x() - localpos.x();
			}
		      }
		    }
		    if(iclus>200)continue;
		    clusters_sta0.push_back(clusters->at(iclus));
		    foundift=true;
		    ATH_MSG_DEBUG(" local position "<<fitx<<" "<<fity<<" : "<<clusters->at(iclus)->localPosition());
		    ATH_MSG_DEBUG(" global position "<<gfitx<<" "<<gfity<<" : "<<clusters->at(iclus)->globalPosition());
		    Amg::Vector3D localresi(clusters->at(iclus)->localPosition().x()-fitx,clusters->at(iclus)->localPosition().y()-fity,0.);
		    auto gloresi=clusters->at(iclus)->detectorElement()->surface().transform()*localresi;
		    ATH_MSG_DEBUG(" local/global residual "<<localresi.x()<<" "<<localresi.y()<<" : "<<gloresi.x()<<" "<<gloresi.y()<<" "<<gloresi.z());

		    Identifier id = clusters->at(iclus)->identify();
		    int istation = m_idHelper->station(id);
		    int ilayer = m_idHelper->layer(id);
		    if(istation!=0||ilayer!=i)continue;
		    int stereoid = m_idHelper->side(id);
		    auto trans2 = clusters->at(iclus)->detectorElement()->surface().transform();
		    auto trans1 = trans2;
		    if(stereoid==1){
		      trans1=clusters->at(iclus)->detectorElement()->otherSide()->surface().transform();
		    }
		    ATH_MSG_DEBUG("  save the residual "<<resi);
		    const auto iModuleEta = m_idHelper->eta_module(id);
		    const auto iModulePhi = m_idHelper->phi_module(id);
		    int iModule = iModulePhi;
		    if (iModuleEta < 0) iModule +=4;
		    ATH_MSG_DEBUG("ID "<<istation<<"/"<<ilayer<<"/"<<iModule<<"/"<<stereoid<<"/"<<ilayer);
		    t_align_stationId_sp.push_back(istation);
		    t_align_centery_sp.push_back(clusters->at(iclus)->detectorElement()->center().y());
		    t_align_layerId_sp.push_back(ilayer);
		    t_align_moduleId_sp.push_back(iModule);
		    t_align_clustersize_sp.push_back(clusters->at(iclus)->rdoList().size());
		    t_align_stereoId.push_back(stereoid);
		    t_align_global_measured_x_sp.push_back(clusters->at(iclus)->globalPosition().x());
		    t_align_global_measured_y_sp.push_back(clusters->at(iclus)->globalPosition().y());
		    t_align_global_measured_z_sp.push_back(clusters->at(iclus)->globalPosition().z());
		    t_align_global_fitted_x_sp.push_back(gfitx);
		    t_align_global_fitted_y_sp.push_back(gfity);
		    t_align_local_residual_x_sp.push_back(resi);
		    t_align_unbiased.push_back(2);
		    t_align_local_pull_x_sp.push_back(resi/sqrt(clusters->at(iclus)->localCovariance()(0,0)));
		    t_align_local_measured_x_sp.push_back(clusters->at(iclus)->localPosition().x());
		    t_align_local_measured_xe_sp.push_back(sqrt(cluster->localCovariance()(0,0)));
		    t_align_local_fitted_xe_sp.push_back(sqrt(cluster->localCovariance()(0,0)));
		    t_align_local_fitted_x_sp.push_back(fitx);
		    Amg::Vector2D local_mea=clusters->at(iclus)->localPosition() ; 
		    t_align_derivation_x_x.push_back(getDerivation(ctx, parameter, trans1,local_mea, 0, stereoid, trans2, 0));
		    t_align_derivation_x_y.push_back(getDerivation(ctx, parameter, trans1,local_mea, 1, stereoid, trans2, 0));
		    t_align_derivation_x_z.push_back(getDerivation(ctx, parameter, trans1,local_mea, 2, stereoid, trans2, 0));
		    t_align_derivation_x_rx.push_back(getDerivation(ctx, parameter, trans1,local_mea, 3, stereoid, trans2, 0));
		    t_align_derivation_x_ry.push_back(getDerivation(ctx, parameter, trans1,local_mea, 4, stereoid, trans2, 0));
		    t_align_derivation_x_rz.push_back(getDerivation(ctx, parameter, trans1,local_mea, 5, stereoid, trans2, 0));
		    t_align_derivation_global_y_x.push_back(getDerivation(ctx, parameter, trans1,local_mea, 0, stereoid, trans2, 1));
		    t_align_derivation_global_y_y.push_back(getDerivation(ctx, parameter, trans1,local_mea, 1, stereoid, trans2, 1));
		    t_align_derivation_global_y_z.push_back(getDerivation(ctx, parameter, trans1,local_mea, 2, stereoid, trans2, 1));
		    t_align_derivation_global_y_rx.push_back(getDerivation(ctx, parameter, trans1,local_mea, 3, stereoid, trans2, 1));
		    t_align_derivation_global_y_ry.push_back(getDerivation(ctx, parameter, trans1,local_mea, 4, stereoid, trans2, 1));
		    t_align_derivation_global_y_rz.push_back(getDerivation(ctx, parameter, trans1,local_mea, 5, stereoid, trans2, 1));
		    ATH_MSG_VERBOSE("local derivation "<<t_align_derivation_x_x.back()<<" "<<t_align_derivation_x_y.back()<<" "<<t_align_derivation_x_z.back()<<" "<<t_align_derivation_x_rx.back()<<" "<<t_align_derivation_x_ry.back()<<" "<<t_align_derivation_x_rz.back());
		    ATH_MSG_VERBOSE("global derivation "<<t_align_derivation_global_y_x.back()<<" "<<t_align_derivation_global_y_y.back()<<" "<<t_align_derivation_global_y_z.back()<<""<<t_align_derivation_global_y_rx.back()<<" "<<t_align_derivation_global_y_ry.back()<<" "<<t_align_derivation_global_y_rz.back());
		  }
		}
	      }
	    }
	  }
	  }
	  return true;
      });
      ATH_MSG_VERBOSE("check track");

      std::unique_ptr<Trk::Track> track = m_createTrkTrackTool->createTrack(gctx, traj);
      if (track==nullptr||ini_Param==nullptr) continue;
      origin = -1910.;
      std::vector<TSOS4Residual> unbiase_resi= m_kalmanFitterTool1->getUnbiasedResidual(ctx, gctx, track.get(), Acts::BoundVector::Zero(), m_isMC, origin,clusters_sta0, *ini_Param);
      ATH_MSG_DEBUG(" found tsos size "<<unbiase_resi.size());
      if(unbiase_resi.size()>0){

	ATH_MSG_DEBUG(" check the tsos");
	for (auto& tsos : unbiase_resi){
	  ATH_MSG_DEBUG(" check the param");
	  auto cluster = tsos.cluster;
	  ATH_MSG_DEBUG("Positions "<<tsos.fit_global_x<<" "<<tsos.fit_global_y<<" "<<tsos.fit_global_z<<" , "<<cluster->globalPosition().x()<<" "<<cluster->globalPosition().y()<<" "<<cluster->globalPosition().z());
	  ATH_MSG_DEBUG("Found the ctsos ");
	  double localx =  tsos.fit_local_x;
	  double localy =  tsos.fit_local_y;
	  double globalx= tsos.fit_global_x;
	  double globaly= tsos.fit_global_y;
	  double globalz= tsos.fit_global_z;
	  //transfer to cluster surface
	  Amg::Vector3D glo_pos(globalx,globaly,globalz);
	  std::optional<Amg::Vector2D> loc_pos  = cluster->detectorElement()->surface().globalToLocal(glo_pos);

	  ATH_MSG_DEBUG("local pos "<<localx<<" "<<localy<<" ,  "<<loc_pos->x()<<" "<<loc_pos->y()<<" :vs "<<cluster->localPosition().x()<<" "<<cluster->localPosition().y());
	  localx = loc_pos->x();
	  localy = loc_pos->y();
	  ATH_MSG_DEBUG("global pos "<<globalx<<" "<<globaly<<" vs "<<cluster->globalPosition().x()<<" "<<cluster->globalPosition().y());
	  //	  outputTracks->push_back(std::move(newtrack));
	  ATH_MSG_DEBUG("Global pos "<<globalx<<" , "<<globaly);
	  auto trans2 = cluster->detectorElement()->surface().transform();
	  auto trans1 = trans2;
	  Identifier id = cluster->identify();
	  int istation = m_idHelper->station(id);
	  int ilayer = m_idHelper->layer(id);
	  if(istation!=0)continue;
	  //int layerid = istation*3 + ilayer;
	  //	  if(layerid == 0 || layerid == 11)continue;
	  int stereoid = m_idHelper->side(id);
	  if(stereoid==1){
	    trans1=cluster->detectorElement()->otherSide()->surface().transform();
	  }
	  Acts::BoundTrackParameters fitParameter=tsos.parameter;
	  const auto iModuleEta = m_idHelper->eta_module(id);
	  const auto iModulePhi = m_idHelper->phi_module(id);
	  int iModule = iModulePhi;
	  if (iModuleEta < 0) iModule +=4;
	  ATH_MSG_DEBUG("ID "<<istation<<"/"<<ilayer<<"/"<<iModule<<"/"<<stereoid<<"/"<<ilayer);
	  t_align_stationId_sp.push_back(istation);
	  t_align_centery_sp.push_back(cluster->detectorElement()->center().y());
	  t_align_layerId_sp.push_back(ilayer);
	  t_align_moduleId_sp.push_back(iModule);
	  t_align_clustersize_sp.push_back(cluster->rdoList().size());
	  t_align_stereoId.push_back(stereoid);
	  Amg::Vector2D local_mea=cluster->localPosition() ; 
	  Amg::Vector2D local_meae=cluster->localPosition() + Amg::Vector2D(sqrt(cluster->localCovariance()(0,0)),0);
	  Amg::Vector2D local_fit(localx, localy);
	  Amg::Vector2D local_fite(localx+0.0001, localy+0.0001);
	  ATH_MSG_DEBUG("  save the residual "<<local_mea.x()-local_fit.x()<<" vs "<<tsos.residual);
	  t_align_local_residual_x_sp.push_back(tsos.residual);
	  t_align_unbiased.push_back(1);
	  t_align_local_pull_x_sp.push_back((local_mea.x()-local_fit.x())/(sqrt(((cluster->localCovariance()(0,0)+(local_fite.x()-local_fit.x())*(local_fite.x()-local_fit.x()))))));
	  t_align_local_measured_x_sp.push_back(local_mea.x());
	  t_align_local_measured_xe_sp.push_back(fabs(local_meae.x()-local_mea.x()));
	  t_align_local_fitted_xe_sp.push_back(fabs(local_fit.x()-local_fite.x()));
	  t_align_local_fitted_x_sp.push_back(local_fit.x());

	  Amg::Vector3D global_fit= cluster->detectorElement()->surface().localToGlobal(local_fit);
	  Amg::Vector3D global_fite= cluster->detectorElement()->surface().localToGlobal( local_fite);
	  Amg::Vector3D global_mea= cluster->detectorElement()->surface().localToGlobal( local_mea);
	  Amg::Vector3D global_meae= cluster->detectorElement()->surface().localToGlobal( local_meae);
	  t_align_global_residual_y_sp.push_back(global_mea.y()-global_fit.y());
	  t_align_global_measured_x_sp.push_back(global_mea.x());
	  t_align_global_measured_y_sp.push_back(global_mea.y());
	  t_align_global_measured_z_sp.push_back(global_mea.z());
	  t_align_global_measured_ye_sp.push_back(fabs(global_mea.y()-global_meae.y()));
	  t_align_global_fitted_ye_sp.push_back(fabs(global_fite.y()-global_fite.y()));
	  t_align_global_fitted_x_sp.push_back(global_fit.x());
	  t_align_global_fitted_y_sp.push_back(global_fit.y());
	  t_align_derivation_x_x.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 0, stereoid, trans2, 0));
	  t_align_derivation_x_y.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 1, stereoid, trans2, 0));
	  t_align_derivation_x_z.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 2, stereoid, trans2, 0));
	  t_align_derivation_x_rx.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 3, stereoid, trans2, 0));
	  t_align_derivation_x_ry.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 4, stereoid, trans2, 0));
	  t_align_derivation_x_rz.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 5, stereoid, trans2, 0));
	  t_align_derivation_global_y_x.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 0, stereoid, trans2, 1));
	  t_align_derivation_global_y_y.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 1, stereoid, trans2, 1));
	  t_align_derivation_global_y_z.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 2, stereoid, trans2, 1));
	  t_align_derivation_global_y_rx.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 3, stereoid, trans2, 1));
	  t_align_derivation_global_y_ry.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 4, stereoid, trans2, 1));
	  t_align_derivation_global_y_rz.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 5, stereoid, trans2, 1));
	  ATH_MSG_VERBOSE("local derivation "<<t_align_derivation_x_x.back()<<" "<<t_align_derivation_x_y.back()<<" "<<t_align_derivation_x_z.back()<<" "<<t_align_derivation_x_rx.back()<<" "<<t_align_derivation_x_ry.back()<<" "<<t_align_derivation_x_rz.back());
	  ATH_MSG_VERBOSE("global derivation "<<t_align_derivation_global_y_x.back()<<" "<<t_align_derivation_global_y_y.back()<<" "<<t_align_derivation_global_y_z.back()<<""<<t_align_derivation_global_y_rx.back()<<" "<<t_align_derivation_global_y_ry.back()<<" "<<t_align_derivation_global_y_rz.back());
	}
      }
    }
    else{
      std::unique_ptr<Trk::Track> track = m_createTrkTrackTool->createTrack(gctx, traj);
      if (track!=nullptr) {
	for (const Trk::MeasurementBase *meas : *track->measurementsOnTrack()) {
	  const auto* clusterOnTrack = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*>(meas);
	  if (clusterOnTrack) {
	    const Tracker::FaserSCT_Cluster* cluster = clusterOnTrack->prepRawData();
	    ATH_MSG_DEBUG("  Found the cluster to be remove "<<cluster->globalPosition());
	    //remove the cluster and re-fit to get unbiased residual
	    Identifier id = clusterOnTrack->identify();
	    Identifier waferId = m_idHelper->wafer_id(id);
	    ATH_MSG_DEBUG("like cluster ID "<<m_idHelper->layer(id)<<" "<<m_idHelper->station(id));
	    double localx(-999);
	    double localxe(999);
	    double localy(-999);
	    double localye(999);
	    double globalx(-999);
	    double globaly(999);
	    double globalz(999);
//	    int charge =0;
//	    Acts::BoundSymMatrix cov = Acts::BoundSymMatrix::Identity();
	    ATH_MSG_DEBUG("  make unbiased residual");
	    m_numberOfSelectedTracks++;
	    if((*track->measurementsOnTrack()).size()<15)continue;
	    //remove the cluster and re-fit to get unbiased residual
	    std::vector<TSOS4Residual> unbiase_resi= m_kalmanFitterTool1->getUnbiasedResidual(ctx, gctx, track.get(), cluster->globalPosition().z(), Acts::BoundVector::Zero(), m_isMC, origin);
	    if(unbiase_resi.size()>0){

	      ATH_MSG_DEBUG(" check the tsos");
	      for (auto& tsos : unbiase_resi){
		ATH_MSG_DEBUG(" check the param");
		if(tsos.cluster->identify()!=cluster->identify())continue;
		ATH_MSG_DEBUG("Positions "<<tsos.fit_global_x<<" "<<tsos.fit_global_y<<" "<<tsos.fit_global_z<<" , "<<cluster->globalPosition().x()<<" "<<cluster->globalPosition().y()<<" "<<cluster->globalPosition().z());
		ATH_MSG_DEBUG("Found the ctsos ");
		localx =  tsos.fit_local_x;
		localy =  tsos.fit_local_y;
		globalx= tsos.fit_global_x;
		globaly= tsos.fit_global_y;
		globalz= tsos.fit_global_z;
		//transfer to cluster surface
		Amg::Vector3D glo_pos(globalx,globaly,globalz);
		std::optional<Amg::Vector2D> loc_pos  = cluster->detectorElement()->surface().globalToLocal(glo_pos);

		ATH_MSG_DEBUG("local pos "<<localx<<" "<<localy<<" ,  "<<loc_pos->x()<<" "<<loc_pos->y()<<" :vs "<<cluster->localPosition().x()<<" "<<cluster->localPosition().y());
		localx = loc_pos->x();
		localy = loc_pos->y();
		ATH_MSG_DEBUG("global pos "<<globalx<<" "<<globaly<<" vs "<<cluster->globalPosition().x()<<" "<<cluster->globalPosition().y());
		save=true;
		ATH_MSG_DEBUG("Global pos "<<globalx<<" , "<<globaly);
		auto trans2 = cluster->detectorElement()->surface().transform();
		auto trans1 = trans2;
		Identifier id = cluster->identify();
		int istation = m_idHelper->station(id);
		int ilayer = m_idHelper->layer(id);
		int stereoid = m_idHelper->side(id);
		if(stereoid==1){
		  trans1=cluster->detectorElement()->otherSide()->surface().transform();
		}
		Acts::BoundTrackParameters fitParameter=tsos.parameter;
		const auto iModuleEta = m_idHelper->eta_module(id);
		const auto iModulePhi = m_idHelper->phi_module(id);
		int iModule = iModulePhi;
		if (iModuleEta < 0) iModule +=4;
		ATH_MSG_DEBUG("ID "<<istation<<"/"<<ilayer<<"/"<<iModule<<"/"<<stereoid<<"/"<<ilayer);
		t_align_centery_sp.push_back(cluster->detectorElement()->center().y());
		t_align_stationId_sp.push_back(istation);
		t_align_layerId_sp.push_back(ilayer);
		t_align_moduleId_sp.push_back(iModule);
		t_align_clustersize_sp.push_back(cluster->rdoList().size());
		t_align_stereoId.push_back(stereoid);
		Amg::Vector2D local_mea=cluster->localPosition() ; 
		Amg::Vector2D local_meae=cluster->localPosition() + Amg::Vector2D(sqrt(cluster->localCovariance()(0,0)),0);
		Amg::Vector2D local_fit(localx, localy);
		Amg::Vector2D local_fite(localx+localxe, localy+localye);
		ATH_MSG_DEBUG("  save the residual "<<local_mea.x()-local_fit.x()<<" vs "<<tsos.residual);
		t_align_local_residual_x_sp.push_back(tsos.residual);
		t_align_unbiased.push_back(3);
		t_align_local_pull_x_sp.push_back((local_mea.x()-local_fit.x())/(sqrt(((cluster->localCovariance()(0,0)+(local_fite.x()-local_fit.x())*(local_fite.x()-local_fit.x()))))));
		t_align_local_measured_x_sp.push_back(local_mea.x());
		t_align_local_measured_xe_sp.push_back(fabs(local_meae.x()-local_mea.x()));
		t_align_local_fitted_xe_sp.push_back(fabs(local_fit.x()-local_fite.x()));
		t_align_local_fitted_x_sp.push_back(local_fit.x());

		Amg::Vector3D global_fit= cluster->detectorElement()->surface().localToGlobal(local_fit);
		Amg::Vector3D global_fite= cluster->detectorElement()->surface().localToGlobal( local_fite);
		Amg::Vector3D global_mea= cluster->detectorElement()->surface().localToGlobal( local_mea);
		Amg::Vector3D global_meae= cluster->detectorElement()->surface().localToGlobal( local_meae);
		t_align_global_residual_y_sp.push_back(global_mea.y()-global_fit.y());
		t_align_global_measured_x_sp.push_back(global_mea.x());
		t_align_global_measured_y_sp.push_back(global_mea.y());
		t_align_global_measured_z_sp.push_back(global_mea.z());
		t_align_global_measured_ye_sp.push_back(fabs(global_mea.y()-global_meae.y()));
		t_align_global_fitted_ye_sp.push_back(fabs(global_fite.y()-global_fite.y()));
		t_align_global_fitted_x_sp.push_back(global_fit.x());
		t_align_global_fitted_y_sp.push_back(global_fit.y());
		t_align_derivation_x_x.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 0, stereoid, trans2, 0));
		t_align_derivation_x_y.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 1, stereoid, trans2, 0));
		t_align_derivation_x_z.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 2, stereoid, trans2, 0));
		t_align_derivation_x_rx.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 3, stereoid, trans2, 0));
		t_align_derivation_x_ry.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 4, stereoid, trans2, 0));
		t_align_derivation_x_rz.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 5, stereoid, trans2, 0));
		t_align_derivation_global_y_x.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 0, stereoid, trans2, 1));
		t_align_derivation_global_y_y.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 1, stereoid, trans2, 1));
		t_align_derivation_global_y_z.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 2, stereoid, trans2, 1));
		t_align_derivation_global_y_rx.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 3, stereoid, trans2, 1));
		t_align_derivation_global_y_ry.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 4, stereoid, trans2, 1));
		t_align_derivation_global_y_rz.push_back(getDerivation(ctx, fitParameter, trans1,local_mea, 5, stereoid, trans2, 1));
		ATH_MSG_VERBOSE("local derivation "<<t_align_derivation_x_x.back()<<" "<<t_align_derivation_x_y.back()<<" "<<t_align_derivation_x_z.back()<<" "<<t_align_derivation_x_rx.back()<<" "<<t_align_derivation_x_ry.back()<<" "<<t_align_derivation_x_rz.back());
		ATH_MSG_VERBOSE("global derivation "<<t_align_derivation_global_y_x.back()<<" "<<t_align_derivation_global_y_y.back()<<" "<<t_align_derivation_global_y_z.back()<<""<<t_align_derivation_global_y_rx.back()<<" "<<t_align_derivation_global_y_ry.back()<<" "<<t_align_derivation_global_y_rz.back());
	      }
	    }
	  }
	}
      }
    }
    ATH_MSG_VERBOSE("finish getting residuals");
    if(save){
      ATH_MSG_VERBOSE("save the results");
      m_fitParam_nMeasurements.push_back(trajState.nMeasurements);
      m_fitParam_nStates.push_back(trajState.nStates);
      m_fitParam_nOutliers.push_back(trajState.nOutliers);
      m_fitParam_nHoles.push_back(trajState.nHoles);
      m_fitParam_chi2.push_back(trajState.chi2Sum);
      m_fitParam_ndf.push_back(trajState.NDF);
      m_fitParam_x.push_back(params.position(gctx).transpose().x());
      m_fitParam_y.push_back(params.position(gctx).transpose().y());
      m_fitParam_z.push_back(params.position(gctx).transpose().z());
      m_fitParam_charge.push_back(params.charge());
      m_fitParam_px.push_back(params.momentum().transpose().x());
      m_fitParam_py.push_back(params.momentum().transpose().y());
      m_fitParam_pz.push_back(params.momentum().transpose().z());
      m_align_stationId_sp.push_back(t_align_stationId_sp);
      m_align_centery_sp.push_back(t_align_centery_sp);
      m_align_layerId_sp.push_back(t_align_layerId_sp);
      m_align_moduleId_sp.push_back(t_align_moduleId_sp); 
      m_align_clustersize_sp.push_back(t_align_clustersize_sp); 
      m_align_stereoId.push_back(t_align_stereoId);
      m_align_global_residual_y_sp.push_back(t_align_global_residual_y_sp);
      m_align_global_measured_x_sp.push_back(t_align_global_measured_x_sp);
      m_align_global_measured_y_sp.push_back(t_align_global_measured_y_sp);
      m_align_global_measured_z_sp.push_back(t_align_global_measured_z_sp);
      m_align_global_measured_ye_sp.push_back(t_align_global_measured_ye_sp);
      m_align_global_fitted_ye_sp.push_back(t_align_global_fitted_ye_sp);
      m_align_global_fitted_x_sp.push_back(t_align_global_fitted_x_sp);
      m_align_global_fitted_y_sp.push_back(t_align_global_fitted_y_sp);
      m_align_local_residual_x_sp.push_back(t_align_local_residual_x_sp);
      m_align_unbiased_sp.push_back(t_align_unbiased);
      m_align_local_pull_x_sp.push_back(t_align_local_pull_x_sp);
      m_align_local_measured_x_sp.push_back(t_align_local_measured_x_sp);
      m_align_local_measured_xe_sp.push_back(t_align_local_measured_xe_sp);
      m_align_local_fitted_xe_sp.push_back(t_align_local_fitted_xe_sp);
      m_align_local_fitted_x_sp.push_back(t_align_local_fitted_x_sp);
      m_align_derivation_x_x.push_back(t_align_derivation_x_x);
      m_align_derivation_x_y.push_back(t_align_derivation_x_y);
      m_align_derivation_x_z.push_back(t_align_derivation_x_z);
      m_align_derivation_x_rx.push_back(t_align_derivation_x_rx);
      m_align_derivation_x_ry.push_back(t_align_derivation_x_ry);
      m_align_derivation_x_rz.push_back(t_align_derivation_x_rz);
      m_align_derivation_global_y_x.push_back(t_align_derivation_global_y_x);
      m_align_derivation_global_y_y.push_back(t_align_derivation_global_y_y);
      m_align_derivation_global_y_z.push_back(t_align_derivation_global_y_z);
      m_align_derivation_global_y_rx.push_back(t_align_derivation_global_y_rx);
      m_align_derivation_global_y_ry.push_back(t_align_derivation_global_y_ry);
      m_align_derivation_global_y_rz.push_back(t_align_derivation_global_y_rz);
    }
  }

  if(save)
    m_tree->Fill();

  return StatusCode::SUCCESS;
}


StatusCode CKF2Alignment::finalize() {
  ATH_MSG_INFO("CombinatorialKalmanFilterAlg::finalize()");
  ATH_MSG_INFO(m_numberOfEvents << " events processed.");
  ATH_MSG_INFO(m_numberOfTrackSeeds << " seeds.");
  ATH_MSG_INFO(m_numberOfFittedTracks << " fitted tracks.");
  ATH_MSG_INFO(m_numberOfSelectedTracks << " selected and re-fitted tracks.");
  //nominal geometry
  double nominaly[4][3][8]={0.};
  double sigma=0.064;
  nominaly[1][0][4]= -95.61;
  nominaly[1][0][0]=-95.61;
  nominaly[1][0][5]= -31.87;
  nominaly[1][0][1]=-31.87;
  nominaly[1][0][6]= 31.873;
  nominaly[1][0][2]=31.877;
  nominaly[1][0][7]= 95.61;
  nominaly[1][0][3]=95.61;
  nominaly[1][1][4]= -100.61;
  nominaly[1][1][0]=-100.61;
  nominaly[1][1][5]= -36.87;
  nominaly[1][1][1]=-36.87;
  nominaly[1][1][6]= 26.87;
  nominaly[1][1][2]=26.87;
  nominaly[1][1][7]= 90.61;
  nominaly[1][1][3]=90.61;
  nominaly[1][2][4]= -90.61;
  nominaly[1][2][0]=-90.61;
  nominaly[1][2][5]= -26.87;
  nominaly[1][2][1]=-26.87;
  nominaly[1][2][6]= 36.87;
  nominaly[1][2][2]=36.87;
  nominaly[1][2][7]= 100.61;
  nominaly[1][2][3]=100.61;
  nominaly[2][0][4]= -95.61;
  nominaly[2][0][0]=-95.61;
  nominaly[2][0][5]= -31.87;
  nominaly[2][0][1]=-31.87;
  nominaly[2][0][6]= 31.87;
  nominaly[2][0][2]=31.87;
  nominaly[2][0][7]= 95.61;
  nominaly[2][0][3]=95.61;
  nominaly[2][1][4]= -100.61;
  nominaly[2][1][0]=-100.61;
  nominaly[2][1][5]= -36.87;
  nominaly[2][1][1]=-36.87;
  nominaly[2][1][6]= 26.87;
  nominaly[2][1][2]=26.87;
  nominaly[2][1][7]= 90.61;
  nominaly[2][1][3]=90.61;
  nominaly[2][2][4]= -90.61;
  nominaly[2][2][0]=-90.61;
  nominaly[2][2][5]= -26.87;
  nominaly[2][2][1]=-26.87;
  nominaly[2][2][6]= 36.87;
  nominaly[2][2][2]=36.87;
  nominaly[2][2][7]= 100.61;
  nominaly[2][2][3]=100.61;
  nominaly[3][0][4]= -95.61;
  nominaly[3][0][0]=-95.61;
  nominaly[3][0][5]= -31.87;
  nominaly[3][0][1]=-31.87;
  nominaly[3][0][6]= 31.87;
  nominaly[3][0][2]=31.87;
  nominaly[3][0][7]= 95.61;
  nominaly[3][0][3]=95.61;
  nominaly[3][1][4]= -100.61;
  nominaly[3][1][0]=-100.61;
  nominaly[3][1][5]= -36.87;
  nominaly[3][1][1]=-36.87;
  nominaly[3][1][6]= 26.87;
  nominaly[3][1][2]=26.87;
  nominaly[3][1][7]= 90.61;
  nominaly[3][1][3]=90.61;
  nominaly[3][2][4]= -90.61;
  nominaly[3][2][0]=-90.61;
  nominaly[3][2][5]= -26.87;
  nominaly[3][2][1]=-26.87;
  nominaly[3][2][6]= 36.87;
  nominaly[3][2][2]=36.87;
  nominaly[3][2][7]=100.61;
  nominaly[3][2][3]=100.61;
  //dump the matrix to a txt file for alignment
  std::vector<double>* t_fitParam_chi2=0;
  std::vector<double>* t_fitParam_pz=0;
  std::vector<double>* t_fitParam_x=0;
  std::vector<double>* t_fitParam_y=0;
  std::vector<std::vector<double>>* t_fitParam_align_local_residual_x_sp=0;
  std::vector<std::vector<double>>* t_fitParam_align_local_measured_x_sp=0;
  std::vector<std::vector<double>>* t_fitParam_align_local_measured_xe_sp=0;
  std::vector<std::vector<double>>* t_fitParam_align_stationId_sp=0;
  std::vector<std::vector<double>>* t_fitParam_align_centery_sp=0;
  std::vector<std::vector<double>>* t_fitParam_align_layerId_sp=0;
  std::vector<std::vector<double>>* t_fitParam_align_moduleId_sp=0;
  std::vector<std::vector<double>>* t_fitParam_align_unbiased_sp=0;
  std::vector<std::vector<double>>* t_fitParam_align_local_derivation_x_x=0;
  std::vector<std::vector<double>>* t_fitParam_align_local_derivation_x_y=0;
  std::vector<std::vector<double>>* t_fitParam_align_local_derivation_x_z=0;
  std::vector<std::vector<double>>* t_fitParam_align_local_derivation_x_rx=0;
  std::vector<std::vector<double>>* t_fitParam_align_local_derivation_x_ry=0;
  std::vector<std::vector<double>>* t_fitParam_align_local_derivation_x_rz=0;
  std::vector<std::vector<double>>* t_fitParam_align_global_derivation_y_x=0;
  std::vector<std::vector<double>>* t_fitParam_align_global_derivation_y_y=0;
  std::vector<std::vector<double>>* t_fitParam_align_global_derivation_y_z=0;
  std::vector<std::vector<double>>* t_fitParam_align_global_derivation_y_rx=0;
  std::vector<std::vector<double>>* t_fitParam_align_global_derivation_y_ry=0;
  std::vector<std::vector<double>>* t_fitParam_align_global_derivation_y_rz=0;
  std::vector<std::vector<double>>* t_fitParam_align_stereoId=0;
  std::vector<int>* t_fitParam_nMeasurements=0;
  m_tree->SetBranchAddress("fitParam_nMeasurements", &t_fitParam_nMeasurements);
  m_tree->SetBranchAddress("fitParam_align_stereoId",&t_fitParam_align_stereoId);
  m_tree->SetBranchAddress("fitParam_align_local_derivation_x_x",&t_fitParam_align_local_derivation_x_x);
  m_tree->SetBranchAddress("fitParam_align_local_derivation_x_y",&t_fitParam_align_local_derivation_x_y);
  m_tree->SetBranchAddress("fitParam_align_local_derivation_x_z",&t_fitParam_align_local_derivation_x_z);
  m_tree->SetBranchAddress("fitParam_align_local_derivation_x_rx",&t_fitParam_align_local_derivation_x_rx);
  m_tree->SetBranchAddress("fitParam_align_local_derivation_x_ry",&t_fitParam_align_local_derivation_x_ry);
  m_tree->SetBranchAddress("fitParam_align_local_derivation_x_rz",&t_fitParam_align_local_derivation_x_rz);
  m_tree->SetBranchAddress("fitParam_align_global_derivation_y_x",&t_fitParam_align_global_derivation_y_x);
  m_tree->SetBranchAddress("fitParam_align_global_derivation_y_y",&t_fitParam_align_global_derivation_y_y);
  m_tree->SetBranchAddress("fitParam_align_global_derivation_y_z",&t_fitParam_align_global_derivation_y_z);
  m_tree->SetBranchAddress("fitParam_align_global_derivation_y_rx",&t_fitParam_align_global_derivation_y_rx);
  m_tree->SetBranchAddress("fitParam_align_global_derivation_y_ry",&t_fitParam_align_global_derivation_y_ry);
  m_tree->SetBranchAddress("fitParam_align_global_derivation_y_rz",&t_fitParam_align_global_derivation_y_rz);
  m_tree->SetBranchAddress("fitParam_align_local_residual_x_sp",&t_fitParam_align_local_residual_x_sp);
  m_tree->SetBranchAddress("fitParam_chi2",&t_fitParam_chi2);
  m_tree->SetBranchAddress("fitParam_pz",&t_fitParam_pz);
  m_tree->SetBranchAddress("fitParam_x",&t_fitParam_x);
  m_tree->SetBranchAddress("fitParam_y",&t_fitParam_y);
  m_tree->SetBranchAddress("fitParam_align_local_measured_x_sp",&t_fitParam_align_local_measured_x_sp);
  m_tree->SetBranchAddress("fitParam_align_local_measured_xe_sp",&t_fitParam_align_local_measured_xe_sp);
  m_tree->SetBranchAddress("fitParam_align_layerId_sp",&t_fitParam_align_layerId_sp);
  m_tree->SetBranchAddress("fitParam_align_moduleId_sp",&t_fitParam_align_moduleId_sp);
  m_tree->SetBranchAddress("fitParam_align_unbiased_sp",&t_fitParam_align_unbiased_sp);
  m_tree->SetBranchAddress("fitParam_align_stationId_sp",&t_fitParam_align_stationId_sp);
  m_tree->SetBranchAddress("fitParam_align_centery_sp",&t_fitParam_align_centery_sp);
  int ntrk_mod[12][8]={0};
  int ntrk_sta[4]={0};
  int ntrk_lay[12]={0};
  //define matrix
  int Ndim=6;
  std::vector<TMatrixD> denom_lay;
  std::vector<TMatrixD> num_lay;
  std::vector<std::vector<TMatrixD>> denom_mod;
  std::vector<std::vector<TMatrixD>> num_mod;
  std::vector<std::vector<TMatrixD>> denom_mod1;
  std::vector<std::vector<TMatrixD>> num_mod1;
  //initialize to 0
  auto num_matrix = [](int n) {
    TMatrixD num_t(n,1);
    for(int i=0;i<n;i++){
      num_t[i][0]=0.;
    }
    return num_t;
  };
  auto denom_matrix = [](int n) {
    TMatrixD denom_t(n,n);
    for(int i=0;i<n;i++){
      for(int j=0;j<n;j++){
	denom_t[i][j]=0.;
      }
    }
    return denom_t;
  };
  std::vector<TMatrixD> denom_sta;
  std::vector<TMatrixD> num_sta;
  for(int i=0;i<4;i++){
    denom_sta.push_back(denom_matrix(Ndim));
    num_sta.push_back(num_matrix(Ndim));
  }
  for(int i=0;i<12;i++){
    denom_lay.push_back(denom_matrix(Ndim));
    num_lay.push_back(num_matrix(Ndim));
    std::vector<TMatrixD> denom_l;
    std::vector<TMatrixD> num_l;
    std::vector<TMatrixD> denom_l1;
    std::vector<TMatrixD> num_l1;
    for(int j=0;j<8;j++){
      denom_l.push_back(denom_matrix(Ndim));
      num_l.push_back(num_matrix(Ndim));
      denom_l1.push_back(denom_matrix(Ndim));
      num_l1.push_back(num_matrix(Ndim));
    }
    denom_mod.push_back(denom_l1);
    denom_mod1.push_back(denom_l1);
    num_mod.push_back(num_l);
    num_mod1.push_back(num_l1);
  }
  double chi_mod_y[12][8]={0};
  std::cout<<"found "<<m_tree->GetEntries()<<" events "<<std::endl;
  //loop over all the entries
  for(int ievt=0;ievt<m_tree->GetEntries();ievt++){
    m_tree->GetEntry(ievt);
    if(ievt%10000==0)std::cout<<"processing "<<ievt<<" event"<<std::endl;
    if(t_fitParam_chi2->size()<1)continue;
    for(int i=0;i<1;i+=1){
      if(t_fitParam_chi2->at(i)>100||t_fitParam_pz->at(i)<300||sqrt(t_fitParam_x->at(i)*t_fitParam_x->at(i)+t_fitParam_y->at(i)*t_fitParam_y->at(i))>95)continue;
      if(t_fitParam_align_local_residual_x_sp->at(i).size()<15)continue;//only     use good track
      for(size_t j=0;j<t_fitParam_align_local_residual_x_sp->at(i).size();j++){
	double resx1=999.;
	double x1e=999.;
	if(m_biased&&t_fitParam_align_stationId_sp->at(i).at(j)==0){
	  if(t_fitParam_align_unbiased_sp->at(i).at(j)==1){
	    resx1=(t_fitParam_align_local_residual_x_sp->at(i).at(j));
	    x1e=sqrt(t_fitParam_align_local_measured_xe_sp->at(i).at(j));
	    if(fabs(resx1)>1.0)continue;
	  }
	  else continue;
	}
	else{
	  resx1=t_fitParam_align_local_residual_x_sp->at(i).at(j);
	  x1e=sqrt(t_fitParam_align_local_measured_xe_sp->at(i).at(j));
	  if(fabs(resx1)>0.2)continue;
	}
	TMatrixD m1(Ndim,1);
	m1[0][0]=t_fitParam_align_local_derivation_x_x->at(i).at(j)/x1e;
	m1[1][0]=t_fitParam_align_local_derivation_x_y->at(i).at(j)/x1e;
	m1[2][0]=t_fitParam_align_local_derivation_x_z->at(i).at(j)/x1e;
	m1[3][0]=t_fitParam_align_local_derivation_x_rx->at(i).at(j)/x1e;
	m1[4][0]=t_fitParam_align_local_derivation_x_ry->at(i).at(j)/x1e;
	m1[5][0]=t_fitParam_align_local_derivation_x_rz->at(i).at(j)/x1e;
	TMatrixD m2(1,Ndim);
	TMatrixD m3(Ndim,Ndim);
	m2.Transpose(m1);
	m3=m1*m2;
	TMatrixD m4(Ndim,1);
	m4[0][0]=t_fitParam_align_local_derivation_x_x->at(i).at(j)/x1e*resx1/x1e;
	m4[1][0]=t_fitParam_align_local_derivation_x_y->at(i).at(j)/x1e*resx1/x1e;
	m4[2][0]=t_fitParam_align_local_derivation_x_z->at(i).at(j)/x1e*resx1/x1e;
	m4[3][0]=t_fitParam_align_local_derivation_x_rx->at(i).at(j)/x1e*resx1/x1e;
	m4[4][0]=t_fitParam_align_local_derivation_x_ry->at(i).at(j)/x1e*resx1/x1e;
	m4[5][0]=t_fitParam_align_local_derivation_x_rz->at(i).at(j)/x1e*resx1/x1e;
	TMatrixD m6(Ndim,1);
	m6=m4;
	TMatrixD mg1(Ndim,1);
	mg1[0][0]=t_fitParam_align_global_derivation_y_x->at(i).at(j)/x1e;
	mg1[1][0]=t_fitParam_align_global_derivation_y_y->at(i).at(j)/x1e;
	mg1[2][0]=t_fitParam_align_global_derivation_y_z->at(i).at(j)/x1e;
	mg1[3][0]=t_fitParam_align_global_derivation_y_rx->at(i).at(j)/x1e;
	mg1[4][0]=t_fitParam_align_global_derivation_y_ry->at(i).at(j)/x1e;
	mg1[5][0]=t_fitParam_align_global_derivation_y_rz->at(i).at(j)/x1e;
	TMatrixD mg2(1,Ndim);
	TMatrixD mg3(Ndim,Ndim);
	mg2.Transpose(mg1);
	mg3=mg1*mg2;
	TMatrixD mg4(Ndim,1);
	mg4[0][0]=t_fitParam_align_global_derivation_y_x->at(i).at(j)/x1e*resx1/x1e;
	mg4[1][0]=t_fitParam_align_global_derivation_y_y->at(i).at(j)/x1e*resx1/x1e;
	mg4[2][0]=t_fitParam_align_global_derivation_y_z->at(i).at(j)/x1e*resx1/x1e;
	mg4[3][0]=t_fitParam_align_global_derivation_y_rx->at(i).at(j)/x1e*resx1/x1e;
	mg4[4][0]=t_fitParam_align_global_derivation_y_ry->at(i).at(j)/x1e*resx1/x1e;
	mg4[5][0]=t_fitParam_align_global_derivation_y_rz->at(i).at(j)/x1e*resx1/x1e;
	TMatrixD mg6(Ndim,1);
	mg6=mg4;
	int istation=t_fitParam_align_stationId_sp->at(i).at(j);
	int ilayer=t_fitParam_align_layerId_sp->at(i).at(j);
	int imodule=t_fitParam_align_moduleId_sp->at(i).at(j);
	//station: 1,2,3
	if(istation>=0&&istation<4){
	  denom_sta[istation]+=mg3;
	  num_sta[istation]+=mg6;
	  ntrk_sta[istation]+=1;
	}
	int ilayer_tot = ilayer + (istation )*3;
	double deltay=t_fitParam_align_centery_sp->at(i).at(j)-nominaly[istation][ilayer][imodule];
	ATH_MSG_DEBUG("nominal/new geometry Y position = "<<nominaly[istation][ilayer][imodule]<<"/"<<t_fitParam_align_centery_sp->at(i).at(j));
	chi_mod_y[ilayer_tot][imodule]+=deltay/sigma/sigma;
	if(ilayer_tot>=0&&ilayer_tot<12){
	  denom_lay[ilayer_tot]+=mg3;
	  num_lay[ilayer_tot]+=mg6;
	  ntrk_lay[ilayer_tot]+=1;
	  denom_mod[ilayer_tot][imodule]+=m3;
	  num_mod[ilayer_tot][imodule]+=m6;
	  ntrk_mod[ilayer_tot][imodule]+=1;
	}
      }
    }
  }

  std::cout<<"Get the nominal transform"<<std::endl;
  //get the alignment constants
  std::ofstream align_output("alignment_matrix.txt",std::ios::out);
  for(int i=0;i<4;i++){
    for(int ii=0;ii<Ndim;ii++){
      align_output<<i<<" "<<ii<<" "<<denom_sta[i][ii][0]<<" "<<denom_sta[i][ii][1]<<" "<<denom_sta[i][ii][2]<<" "<<denom_sta[i][ii][3]<<" "<<denom_sta[i][ii][4]<<" "<<denom_sta[i][ii][5]<<" "<<0<<std::endl;
      if(ii==0)
	std::cout<<i<<" "<<ii<<" "<<denom_sta[i][ii][0]<<" "<<denom_sta[i][ii][1]<<" "<<denom_sta[i][ii][2]<<" "<<denom_sta[i][ii][3]<<" "<<denom_sta[i][ii][4]<<" "<<denom_sta[i][ii][5]<<" "<<0<<std::endl;
    }
    align_output<<i<<" "<<6<<" "<<num_sta[i][0][0]<<" "<<num_sta[i][1][0]<<" "<<num_sta[i][2][0]<<" "<<num_sta[i][3][0]<<" "<<num_sta[i][4][0]<<" "<<num_sta[i][5][0]<<" "<<ntrk_sta[i]<<std::endl;
  }
  std::cout<<"Get the deltas for stations"<<std::endl;
  //layers
  for(size_t i=0;i<denom_lay.size();i++){
    for(int ii=0;ii<Ndim;ii++){
      align_output<<i/3<<i%3<<" "<<ii<<" "<<denom_lay[i][ii][0]<<" "<<denom_lay[i][ii][1]<<" "<<denom_lay[i][ii][2]<<" "<<denom_lay[i][ii][3]<<" "<<denom_lay[i][ii][4]<<" "<<denom_lay[i][ii][5]<<" "<<0<<std::endl;
    }
    align_output<<i/3<<i%3<<" "<<6<<" "<<num_lay[i][0][0]<<" "<<num_lay[i][1][0]<<" "<<num_lay[i][2][0]<<" "<<num_lay[i][3][0]<<" "<<num_lay[i][4][0]<<" "<<num_lay[i][5][0]<<" "<<ntrk_lay[i]<<std::endl;
  }

  std::cout<<"Get the deltas for layers"<<std::endl;
  for(size_t i=0;i<denom_lay.size();i++){
    for(  size_t j=0;j<num_mod[i].size();j++){
      for(int ii=0;ii<Ndim;ii++){
	align_output<<i/3<<i%3<<j<<" "<<ii<<" "<<denom_mod[i][j][ii][0]<<" "<<denom_mod[i][j][ii][1]<<" "<<denom_mod[i][j][ii][2]<<" "<<denom_mod[i][j][ii][3]<<" "<<denom_mod[i][j][ii][4]<<" "<<denom_mod[i][j][ii][5]<<" "<<chi_mod_y[i][j]<<std::endl;
      }
      align_output<<i/3<<i%3<<j<<" "<<6<<" "<<num_mod[i][j][0][0]<<" "<<num_mod[i][j][1][0]<<" "<<num_mod[i][j][2][0]<<" "<<num_mod[i][j][3][0]<<" "<<num_mod[i][j][4][0]<<" "<<num_mod[i][j][5][0]<<" "<<ntrk_mod[i][j]<<std::endl;
    }
  }
  std::cout<<"closing the output"<<std::endl;
  align_output.close();
  return StatusCode::SUCCESS;
}


Acts::MagneticFieldContext CKF2Alignment::getMagneticFieldContext(const EventContext& ctx) const {
  SG::ReadCondHandle<FaserFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  if (!readHandle.isValid()) {
    std::stringstream msg;
    msg << "Failed to retrieve magnetic field condition data " << m_fieldCondObjInputKey.key() << ".";
    throw std::runtime_error(msg.str());
  }
  const FaserFieldCacheCondObj* fieldCondObj{*readHandle};
  return Acts::MagneticFieldContext(fieldCondObj);
}




void CKF2Alignment::computeSharedHits(std::vector<IndexSourceLink>* sourceLinks, TrackFinderResult& results) const {
  // Compute shared hits from all the reconstructed tracks
  // Compute nSharedhits and Update ckf results
  // hit index -> list of multi traj indexes [traj, meas]
  static_assert(Acts::SourceLinkConcept<IndexSourceLink>,
      "Source link does not fulfill SourceLinkConcept");

  std::vector<std::size_t> firstTrackOnTheHit(
      sourceLinks->size(), std::numeric_limits<std::size_t>::max());
  std::vector<std::size_t> firstStateOnTheHit(
      sourceLinks->size(), std::numeric_limits<std::size_t>::max());

  for (unsigned int iresult = 0; iresult < results.size(); iresult++) {
    if (not results.at(iresult).ok()) {
      continue;
    }

    auto& ckfResult = results.at(iresult).value();
    auto& measIndexes = ckfResult.lastMeasurementIndices;

    for (auto measIndex : measIndexes) {
      ckfResult.fittedStates.visitBackwards(measIndex, [&](const auto& state) {
	  if (not state.typeFlags().test(Acts::TrackStateFlag::MeasurementFlag))
	  return;

	  std::size_t hitIndex = state.uncalibrated().index();

	  // Check if hit not already used
	  if (firstTrackOnTheHit.at(hitIndex) ==
	      std::numeric_limits<std::size_t>::max()) {
	  firstTrackOnTheHit.at(hitIndex) = iresult;
	  firstStateOnTheHit.at(hitIndex) = state.index();
	  return;
	  }

	  // if already used, control if first track state has been marked
	  // as shared
	  int indexFirstTrack = firstTrackOnTheHit.at(hitIndex);
	  int indexFirstState = firstStateOnTheHit.at(hitIndex);
	  if (not results.at(indexFirstTrack).value().fittedStates.getTrackState(indexFirstState).typeFlags().test(Acts::TrackStateFlag::SharedHitFlag))
	  results.at(indexFirstTrack).value().fittedStates.getTrackState(indexFirstState).typeFlags().set(Acts::TrackStateFlag::SharedHitFlag);

	  // Decorate this track
	  results.at(iresult).value().fittedStates.getTrackState(state.index()).typeFlags().set(Acts::TrackStateFlag::SharedHitFlag);
      });
    }
  }
}


namespace {

  using Updater = Acts::GainMatrixUpdater;
  using Smoother = Acts::GainMatrixSmoother;

  using Stepper = Acts::EigenStepper<>;
  using Navigator = Acts::Navigator;
  using Propagator = Acts::Propagator<Stepper, Navigator>;
  using CKF = Acts::CombinatorialKalmanFilter<Propagator, Updater, Smoother>;

  using TrackParametersContainer = std::vector<CKF2Alignment::TrackParameters>;

  struct TrackFinderFunctionImpl
    : public CKF2Alignment::TrackFinderFunction {
      CKF trackFinder;

      TrackFinderFunctionImpl(CKF&& f) : trackFinder(std::move(f)) {}

      CKF2Alignment::TrackFinderResult operator()(
	  const IndexSourceLinkContainer& sourcelinks,
	  const TrackParametersContainer& initialParameters,
	  const CKF2Alignment::TrackFinderOptions& options)
	const override {
	  return trackFinder.findTracks(sourcelinks, initialParameters, options);
	};
    };

}  // namespace

std::shared_ptr<CKF2Alignment::TrackFinderFunction>
CKF2Alignment::makeTrackFinderFunction(
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
    bool resolvePassive, bool resolveMaterial, bool resolveSensitive) {
  auto magneticField = std::make_shared<FASERMagneticFieldWrapper>();
  Stepper stepper(std::move(magneticField));
  Navigator::Config cfg{trackingGeometry};
  cfg.resolvePassive = resolvePassive;
  cfg.resolveMaterial = resolveMaterial;
  cfg.resolveSensitive = resolveSensitive;
  Navigator navigator(cfg);
  Propagator propagator(std::move(stepper), std::move(navigator));
  CKF trackFinder(std::move(propagator));

  // build the track finder functions. owns the track finder object.
  return std::make_shared<TrackFinderFunctionImpl>(std::move(trackFinder));
}


namespace {

  using Updater = Acts::GainMatrixUpdater;
  using Smoother = Acts::GainMatrixSmoother;
  using Stepper = Acts::EigenStepper<>;
  using Propagator = Acts::Propagator<Stepper, Acts::Navigator>;
  using Fitter = Acts::KalmanFitter<Propagator, Updater, Smoother>;

  struct TrackFitterFunctionImpl
    : public CKF2Alignment::TrackFitterFunction {
      Fitter trackFitter;

      TrackFitterFunctionImpl(Fitter &&f) : trackFitter(std::move(f)) {}

      CKF2Alignment::KFResult operator()(
	  const std::vector<IndexSourceLink> &sourceLinks,
	  const Acts::BoundTrackParameters &initialParameters,
	  const CKF2Alignment::TrackFitterOptions &options)
	const override {
	  return trackFitter.fit(sourceLinks, initialParameters, options);
	};
    };

}  // namespace


std::shared_ptr<CKF2Alignment::TrackFitterFunction>
CKF2Alignment::makeTrackFitterFunction(
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry) {
  auto magneticField = std::make_shared<FASERMagneticFieldWrapper>();
  auto stepper = Stepper(std::move(magneticField));
  Acts::Navigator::Config cfg{trackingGeometry};
  cfg.resolvePassive = false;
  cfg.resolveMaterial = true;
  cfg.resolveSensitive = true;
  Acts::Navigator navigator(cfg);
  Propagator propagator(std::move(stepper), std::move(navigator));
  Fitter trackFitter(std::move(propagator));
  return std::make_shared<TrackFitterFunctionImpl>(std::move(trackFitter));
}


void CKF2Alignment::initializeTree(){
  m_tree->Branch("evtId",&m_numberOfEvents);
  m_tree->Branch("fitParam_x", &m_fitParam_x);
  m_tree->Branch("fitParam_charge", &m_fitParam_charge);
  m_tree->Branch("fitParam_y", &m_fitParam_y);
  m_tree->Branch("fitParam_z", &m_fitParam_z);
  m_tree->Branch("fitParam_px", &m_fitParam_px);
  m_tree->Branch("fitParam_py", &m_fitParam_py);
  m_tree->Branch("fitParam_pz", &m_fitParam_pz);
  m_tree->Branch("fitParam_chi2", &m_fitParam_chi2);
  m_tree->Branch("fitParam_ndf", &m_fitParam_ndf);
  m_tree->Branch("fitParam_nHoles", &m_fitParam_nHoles);
  m_tree->Branch("fitParam_nOutliers", &m_fitParam_nOutliers);
  m_tree->Branch("fitParam_nStates", &m_fitParam_nStates);
  m_tree->Branch("fitParam_nMeasurements", &m_fitParam_nMeasurements);
  m_tree->Branch("fitParam_align_stereoId", &m_align_stereoId);
  m_tree->Branch("fitParam_align_stationId_sp", &m_align_stationId_sp);
  m_tree->Branch("fitParam_align_centery_sp", &m_align_centery_sp);
  m_tree->Branch("fitParam_align_layerId_sp", &m_align_layerId_sp);
  m_tree->Branch("fitParam_align_moduleId_sp", &m_align_moduleId_sp);
  m_tree->Branch("fitParam_align_clustersize_sp", &m_align_clustersize_sp);
  m_tree->Branch("fitParam_align_local_derivation_x_x", &m_align_derivation_x_x);
  m_tree->Branch("fitParam_align_local_derivation_x_y", &m_align_derivation_x_y);
  m_tree->Branch("fitParam_align_local_derivation_x_z", &m_align_derivation_x_z);
  m_tree->Branch("fitParam_align_local_derivation_x_rx", &m_align_derivation_x_rx);
  m_tree->Branch("fitParam_align_local_derivation_x_ry", &m_align_derivation_x_ry);
  m_tree->Branch("fitParam_align_local_derivation_x_rz", &m_align_derivation_x_rz);
  m_tree->Branch("fitParam_align_global_derivation_y_x", &m_align_derivation_global_y_x);
  m_tree->Branch("fitParam_align_global_derivation_y_y", &m_align_derivation_global_y_y);
  m_tree->Branch("fitParam_align_global_derivation_y_z", &m_align_derivation_global_y_z);
  m_tree->Branch("fitParam_align_global_derivation_y_rx", &m_align_derivation_global_y_rx);
  m_tree->Branch("fitParam_align_global_derivation_y_ry", &m_align_derivation_global_y_ry);
  m_tree->Branch("fitParam_align_global_derivation_y_rz", &m_align_derivation_global_y_rz);
  m_tree->Branch("fitParam_align_local_residual_x_sp", &m_align_local_residual_x_sp);
  m_tree->Branch("fitParam_align_unbiased_sp", &m_align_unbiased_sp);
  m_tree->Branch("fitParam_align_local_pull_x_sp", &m_align_local_pull_x_sp);
  m_tree->Branch("fitParam_align_local_measured_x_sp", &m_align_local_measured_x_sp);
  m_tree->Branch("fitParam_align_local_measured_xe_sp", &m_align_local_measured_xe_sp);
  m_tree->Branch("fitParam_align_local_fitted_xe_sp", &m_align_local_fitted_xe_sp);
  m_tree->Branch("fitParam_align_local_fitted_x_sp", &m_align_local_fitted_x_sp);
  m_tree->Branch("fitParam_align_global_residual_y_sp", &m_align_global_residual_y_sp);
  m_tree->Branch("fitParam_align_global_measured_x_sp", &m_align_global_measured_x_sp);
  m_tree->Branch("fitParam_align_global_measured_y_sp", &m_align_global_measured_y_sp);
  m_tree->Branch("fitParam_align_global_measured_ye_sp", &m_align_global_measured_ye_sp);
  m_tree->Branch("fitParam_align_global_measured_z_sp", &m_align_global_measured_z_sp);
  m_tree->Branch("fitParam_align_global_fitted_ye_sp", &m_align_global_fitted_ye_sp);
  m_tree->Branch("fitParam_align_global_fitted_x_sp", &m_align_global_fitted_x_sp);
  m_tree->Branch("fitParam_align_global_fitted_y_sp", &m_align_global_fitted_y_sp);
}

void CKF2Alignment::clearVariables(){
  m_fitParam_x.clear();
  m_fitParam_charge.clear();
  m_fitParam_y.clear();
  m_fitParam_z.clear();
  m_fitParam_px.clear();
  m_fitParam_py.clear();
  m_fitParam_pz.clear();
  m_fitParam_chi2.clear();
  m_fitParam_ndf.clear();
  m_fitParam_nHoles.clear();
  m_fitParam_nOutliers.clear();
  m_fitParam_nStates.clear();
  m_fitParam_nMeasurements.clear();
  m_align_derivation_x_x.clear();
  m_align_derivation_x_y.clear();
  m_align_derivation_x_z.clear();
  m_align_derivation_x_rx.clear();
  m_align_derivation_x_ry.clear();
  m_align_derivation_x_rz.clear();
  m_align_derivation_global_y_x.clear();
  m_align_derivation_global_y_y.clear();
  m_align_derivation_global_y_z.clear();
  m_align_derivation_global_y_rx.clear();
  m_align_derivation_global_y_ry.clear();
  m_align_derivation_global_y_rz.clear();
  m_align_local_residual_x_sp.clear();
  m_align_unbiased_sp.clear();
  m_align_local_pull_x_sp.clear();
  m_align_local_measured_x_sp.clear();
  m_align_local_measured_xe_sp.clear();
  m_align_local_fitted_xe_sp.clear();
  m_align_local_fitted_x_sp.clear();
  m_align_global_residual_y_sp.clear();
  m_align_global_measured_x_sp.clear();
  m_align_global_measured_y_sp.clear();
  m_align_global_measured_z_sp.clear();
  m_align_global_measured_ye_sp.clear();
  m_align_global_fitted_ye_sp.clear();
  m_align_global_fitted_x_sp.clear();
  m_align_global_fitted_y_sp.clear();
  m_align_stationId_sp.clear();
  m_align_centery_sp.clear();
  m_align_layerId_sp.clear();
  m_align_moduleId_sp.clear();
  m_align_clustersize_sp.clear();
  m_align_stereoId.clear();
}


double CKF2Alignment::getLocalDerivation(Amg::Transform3D trans1, Amg::Vector2D loc_pos, int ia, int side , Amg::Transform3D trans2)const {
  double delta[6]={0.001,0.001,0.001,0.0001,0.0001,0.0001};
  auto translation_m = Amg::Translation3D(0,0,0);
  Amg::Transform3D transform_m= translation_m* Amg::RotationMatrix3D::Identity();
  auto translation_p = Amg::Translation3D(0,0,0);
  Amg::Transform3D transform_p= translation_p* Amg::RotationMatrix3D::Identity();
  switch (ia)
  {
    case 0:
      transform_m *= Amg::Translation3D(0-delta[ia],0,0);
      transform_p *= Amg::Translation3D(0+delta[ia],0,0);
      break;
    case 1:
      transform_m *= Amg::Translation3D(0,0-delta[ia],0);
      transform_p *= Amg::Translation3D(0,0+delta[ia],0);
      break;
    case 2:
      transform_m *= Amg::Translation3D(0,0,0-delta[ia]);
      transform_p *= Amg::Translation3D(0,0,0+delta[ia]);
      break;
    case 3:
      transform_m *= Amg::AngleAxis3D(0-delta[ia], Amg::Vector3D(1,0,0));
      transform_p *= Amg::AngleAxis3D(0+delta[ia], Amg::Vector3D(1,0,0));
      break;
    case 4:
      transform_m *= Amg::AngleAxis3D(0-delta[ia], Amg::Vector3D(0,1,0));
      transform_p *= Amg::AngleAxis3D(0+delta[ia], Amg::Vector3D(0,1,0));
      break;
    case 5:
      transform_m *= Amg::AngleAxis3D(0-delta[ia], Amg::Vector3D(0,0,1));
      transform_p *= Amg::AngleAxis3D(0+delta[ia], Amg::Vector3D(0,0,1));
      break;
    default:
      ATH_MSG_FATAL("Unknown alignment parameter" );
      break;
  }

  auto local3 = Amg::Vector3D(loc_pos.x(),loc_pos.y(),0.);
  Amg::Vector3D glo_pos = trans1 * local3;
  Amg::Vector3D local_m = (trans1 * transform_m).inverse()*glo_pos;
  Amg::Vector3D local_p = (trans1 * transform_p).inverse()*glo_pos;
  if(side!=1)
    return (local_p.x()-local_m.x())/2./delta[ia];
  else{
    Amg::Vector3D local_m2=trans2.inverse()*trans1*local_m;
    Amg::Vector3D local_p2=trans2.inverse()*trans1*local_p;
    return (local_p2.x()-local_m2.x())/2./delta[ia];
  }
}


double CKF2Alignment::getDerivation(const EventContext& ctx, Acts::BoundTrackParameters& fitParameters, Amg::Transform3D trans1, Amg::Vector2D loc_pos, int ia, int side , Amg::Transform3D trans2, int global)const {
  double delta[6]={0.001,0.001,0.001,0.0001,0.0001,0.0001};
  auto translation_m = Amg::Translation3D(0,0,0);
  Amg::Transform3D transform_m= translation_m* Amg::RotationMatrix3D::Identity();
  auto translation_p = Amg::Translation3D(0,0,0);
  Amg::Transform3D transform_p= translation_p* Amg::RotationMatrix3D::Identity();
  switch (ia)
  {
    case 0:
      transform_m *= Amg::Translation3D(0-delta[ia],0,0);
      transform_p *= Amg::Translation3D(0+delta[ia],0,0);
      break;
    case 1:
      transform_m *= Amg::Translation3D(0,0-delta[ia],0);
      transform_p *= Amg::Translation3D(0,0+delta[ia],0);
      break;
    case 2:
      transform_m *= Amg::Translation3D(0,0,0-delta[ia]);
      transform_p *= Amg::Translation3D(0,0,0+delta[ia]);
      break;
    case 3:
      transform_m *= Amg::AngleAxis3D(0-delta[ia], Amg::Vector3D(1,0,0));
      transform_p *= Amg::AngleAxis3D(0+delta[ia], Amg::Vector3D(1,0,0));
      break;
    case 4:
      transform_m *= Amg::AngleAxis3D(0-delta[ia], Amg::Vector3D(0,1,0));
      transform_p *= Amg::AngleAxis3D(0+delta[ia], Amg::Vector3D(0,1,0));
      break;
    case 5:
      transform_m *= Amg::AngleAxis3D(0-delta[ia], Amg::Vector3D(0,0,1));
      transform_p *= Amg::AngleAxis3D(0+delta[ia], Amg::Vector3D(0,0,1));
      break;
    default:
      ATH_MSG_FATAL("Unknown alignment parameter" );
      break;
  }

  auto local3 = Amg::Vector3D(loc_pos.x(),loc_pos.y(),0.);
  Amg::Vector3D glo_pos = trans2 * local3;
  auto trans_m = trans1 * transform_m;
  auto trans_p = trans1 * transform_p;
  if(side==1 && global!=1){
    trans_m = trans1 * transform_m * trans1.inverse() * trans2;
    trans_p = trans1 * transform_p * trans1.inverse() * trans2;
  }
  if (global==1){
    Amg::Transform3D trans_g = Amg::Translation3D(0,0, glo_pos.z()) * Amg::RotationMatrix3D::Identity();
    auto trans_l2g = trans_g.inverse()*trans1;
    trans_m = trans1 * trans_l2g.inverse() * transform_m * trans_l2g;
    trans_p = trans1 * trans_l2g.inverse() * transform_p * trans_l2g;
    if(side==1){
      auto tmpm = trans_m;
      auto tmpp = trans_p;
      trans_m = tmpm * trans1.inverse() * trans2;
      trans_p = tmpp * trans1.inverse() * trans2;
    }
  }

  Amg::Transform3D ini_trans = Amg::Translation3D(0,0, glo_pos.z()-10) * Amg::RotationMatrix3D::Identity();
  auto ini_surface = Acts::Surface::makeShared<Acts::PlaneSurface>(ini_trans, std::make_shared<const Acts::RectangleBounds>(1000.,1000.));
  std::unique_ptr<const Acts::BoundTrackParameters> ini_Param = m_extrapolationTool->propagate( ctx, fitParameters, *ini_surface, Acts::backward);
  if(ini_Param==nullptr)return -9999.;


  auto shift_m_surface = Acts::Surface::makeShared<Acts::PlaneSurface>(trans_m, std::make_shared<const Acts::RectangleBounds>(1000.,1000.));
  auto shift_p_surface = Acts::Surface::makeShared<Acts::PlaneSurface>(trans_p, std::make_shared<const Acts::RectangleBounds>(1000.,1000.));
  auto shift_m_Param = m_extrapolationTool->propagate( ctx, *ini_Param, *shift_m_surface, Acts::forward);
  auto shift_p_Param = m_extrapolationTool->propagate( ctx, *ini_Param, *shift_p_surface, Acts::forward);
  if(shift_m_Param!=nullptr||shift_p_Param!=nullptr){
    auto shift_m_parameter = shift_m_Param->parameters();
    auto shift_p_parameter = shift_p_Param->parameters();
    Amg::Vector2D local_m(shift_m_parameter[Acts::eBoundLoc0],shift_m_parameter[Acts::eBoundLoc1]);
    Amg::Vector2D local_p(shift_p_parameter[Acts::eBoundLoc0],shift_p_parameter[Acts::eBoundLoc1]);
    return (local_p.x()-local_m.x())/2./delta[ia];
  }
  return -99999.;
}

