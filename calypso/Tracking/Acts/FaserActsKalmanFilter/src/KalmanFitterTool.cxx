#include "KalmanFitterTool.h"

#include "FaserActsGeometry/FASERMagneticFieldWrapper.h"
#include "FaserActsKalmanFilter/IndexSourceLink.h"

#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/TrackFitting/GainMatrixSmoother.hpp"
#include "Acts/TrackFitting/GainMatrixUpdater.hpp"
#include "TrackerIdentifier/FaserSCT_ID.h"

#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"


KalmanFitterTool::KalmanFitterTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent) {}


StatusCode KalmanFitterTool::initialize() {
  ATH_CHECK(m_fieldCondObjInputKey.initialize());
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  ATH_CHECK(m_createTrkTrackTool.retrieve());
  ATH_CHECK(detStore()->retrieve(m_idHelper,"FaserSCT_ID"));
  if (m_statesWriter && !m_noDiagnostics) ATH_CHECK(m_trajectoryStatesWriterTool.retrieve());
  if (m_summaryWriter && !m_noDiagnostics) ATH_CHECK(m_trajectorySummaryWriterTool.retrieve());
  if (m_actsLogging == "VERBOSE" && !m_noDiagnostics) {
    m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::VERBOSE);
  } else if (m_actsLogging == "DEBUG" && !m_noDiagnostics) {
    m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::DEBUG);
  } else {
    m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::INFO);
  }
  auto magneticField = std::make_shared<FASERMagneticFieldWrapper>();
  double reverseFilteringMomThreshold = 0.1; //@todo: needs to be validated
  //@todo: the multiple scattering and energy loss are originallly turned off 
  m_fit = makeTrackFitterFunction(m_trackingGeometryTool->trackingGeometry(), magneticField, true, true, reverseFilteringMomThreshold, Acts::FreeToBoundCorrection(false), *m_logger); 
  return StatusCode::SUCCESS;
}


StatusCode KalmanFitterTool::finalize() {
  return StatusCode::SUCCESS;
}

//get the unbiased residual for the cluster at Z =  clusz
//output is a vector of the outliers
//
std::vector<TSOS4Residual>
KalmanFitterTool::getUnbiasedResidual(const EventContext &ctx, const Acts::GeometryContext &gctx,
                      Trk::Track* inputTrack, double clusz, const Acts::BoundVector& inputVector,
                      bool /*isMC*/, double origin) const {
  std::vector<TSOS4Residual> resi;
  resi.clear();
  ATH_MSG_DEBUG("start kalmanfilter "<<inputTrack->measurementsOnTrack()->size()<<" "<<origin<<" "<<clusz);

  if (!inputTrack->measurementsOnTrack() || inputTrack->measurementsOnTrack()->size() < m_minMeasurements) {
    ATH_MSG_DEBUG("Input track has no or too little measurements and cannot be fitted");
    return resi;
  }

  if (!inputTrack->trackParameters() || inputTrack->trackParameters()->empty()) {
    ATH_MSG_DEBUG("Input track has no track parameters and cannot be fitted");
    return resi;
  }

  auto pSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3 {0, 0, origin}, Acts::Vector3{0, 0, -1});

  Acts::MagneticFieldContext mfContext = getMagneticFieldContext(ctx);
  Acts::CalibrationContext calibContext = Acts::CalibrationContext();

  auto [sourceLinks, measurements] = getMeasurementsFromTrack(inputTrack);
  auto trackParameters = getParametersFromTrack(inputTrack->trackParameters()->front(), inputVector, origin);
  ATH_MSG_DEBUG("trackParameters: " << trackParameters.parameters().transpose());
  ATH_MSG_DEBUG("position: " << trackParameters.position(gctx).transpose());
  ATH_MSG_DEBUG("momentum: " << trackParameters.momentum().transpose());
  ATH_MSG_DEBUG("charge: " << trackParameters.charge());
  ATH_MSG_DEBUG("size of measurements : " << sourceLinks.size());


  auto actsTrackContainer = std::make_shared<Acts::VectorTrackContainer>();
  auto actsTrackStateContainer = std::make_shared<Acts::VectorMultiTrajectory>();
  FaserActsTrackContainer tracks(actsTrackContainer, actsTrackStateContainer);

  //@todo: the initialSurface should be targetSurface
  GeneralFitterOptions options{
      gctx, mfContext, calibContext, &(*pSurface),
      Acts::PropagatorPlainOptions()};

  std::vector<Acts::SourceLink> actsSls;
  for(const auto& sl: sourceLinks){
    actsSls.push_back(Acts::SourceLink{sl});
  } 

  ATH_MSG_DEBUG("Invoke fitter");
  FaserActsOutlierFinder faserActsOutlierFinder{0};
  faserActsOutlierFinder.cluster_z=clusz;
  faserActsOutlierFinder.StateChiSquaredPerNumberDoFCut=10000.;
  auto result = (*m_fit)(actsSls, trackParameters, options, MeasurementCalibratorAdapter(MeasurementCalibrator(), measurements), faserActsOutlierFinder, tracks);

  if (result.ok()) {
    const auto& track = result.value();
    if (track.hasReferenceSurface()) {
      //const auto& params = fitOutput.fittedParameters.value();
      //ATH_MSG_DEBUG("ReFitted parameters");
      //ATH_MSG_DEBUG("  params:   " << params.parameters().transpose());
      //ATH_MSG_DEBUG("  position: " << params.position(gctx).transpose());
      //ATH_MSG_DEBUG("  momentum: " << params.momentum().transpose());
      //ATH_MSG_DEBUG("  charge:   " << params.charge());
      for (const auto& state : track.trackStatesReversed()) {
        auto flag = state.typeFlags();
        //@todo: this only check outlier state?	
        if (not flag.test(Acts::TrackStateFlag::OutlierFlag))
	{
          continue;
	}

	if (state.hasUncalibratedSourceLink()&&state.hasSmoothed()) {
          //todo: make the particle hypothesis configurable
	  Acts::BoundTrackParameters parameter(
	  state.referenceSurface().getSharedPtr(),
	  state.smoothed(),
	  state.smoothedCovariance(), Acts::ParticleHypothesis::muon());
	  // auto covariance = state.smoothedCovariance();
	  auto H = state.effectiveProjector();
	  auto residual = state.effectiveCalibrated() - H * state.smoothed();
          IndexSourceLink sl = state.getUncalibratedSourceLink().template get<IndexSourceLink>();
          const Tracker::FaserSCT_Cluster* cluster = sl.hit();
	  // const auto& surface = state.referenceSurface();
	  Acts::BoundVector meas = state.projector().transpose() * state.effectiveCalibrated();
	  Acts::Vector2 local(meas[Acts::eBoundLoc0], meas[Acts::eBoundLoc1]);
	  // const Acts::Vector3 dir = Acts::makeDirectionUnitFromPhiTheta(meas[Acts::eBoundPhi], meas[Acts::eBoundTheta]);
	  resi.push_back({local.x(),local.y(),parameter.position(gctx).x(), parameter.position(gctx).y(), parameter.position(gctx).z(), cluster,residual(Acts::eBoundLoc0),parameter});
	  ATH_MSG_DEBUG("  residual/global position:   " << residual(Acts::eBoundLoc0)<<" "<<parameter.position(gctx).x()<<" "<<parameter.position(gctx).y()<<" "<<parameter.position(gctx).z());
	}
      }
    } else {
      ATH_MSG_DEBUG("No fitted parameters for track");
    }
  }
  return resi;

}

//get the unbiased residual for IFT, i.e. remove whole IFT in the track fitting
//output is a vector of the outliers
//
std::vector<TSOS4Residual>
KalmanFitterTool::getUnbiasedResidual(const EventContext &ctx, const Acts::GeometryContext &gctx,
                      Trk::Track* inputTrack,  const Acts::BoundVector& /*inputVector*/,
                      bool /*isMC*/, double origin, std::vector<const Tracker::FaserSCT_Cluster*>& clusters, const Acts::BoundTrackParameters ini_Param) const {
  std::vector<TSOS4Residual> resi;
  resi.clear();

  if (!inputTrack->measurementsOnTrack() || inputTrack->measurementsOnTrack()->size() < m_minMeasurements) {
    ATH_MSG_DEBUG("Input track has no or too little measurements and cannot be fitted");
    return resi;
  }

  if (!inputTrack->trackParameters() || inputTrack->trackParameters()->empty()) {
    ATH_MSG_DEBUG("Input track has no track parameters and cannot be fitted");
    return resi;
  }

  auto pSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3 {0, 0, origin}, Acts::Vector3{0, 0, -1});

  Acts::MagneticFieldContext mfContext = getMagneticFieldContext(ctx);
  Acts::CalibrationContext calibContext = Acts::CalibrationContext();

  auto [sourceLinks, measurements] = getMeasurementsFromTrack(inputTrack/*, wafer_id*/, clusters);
  ATH_MSG_DEBUG("trackParameters: " << ini_Param.parameters().transpose());
  ATH_MSG_DEBUG("position: " << ini_Param.position(gctx).transpose());
  ATH_MSG_DEBUG("momentum: " << ini_Param.momentum().transpose());
  ATH_MSG_DEBUG("charge: " << ini_Param.charge());

  auto actsTrackContainer = std::make_shared<Acts::VectorTrackContainer>();
  auto actsTrackStateContainer = std::make_shared<Acts::VectorMultiTrajectory>();
  FaserActsTrackContainer tracks(actsTrackContainer, actsTrackStateContainer);

  //@todo: the initialSurface should be targetSurface
  GeneralFitterOptions options{
      gctx, mfContext, calibContext, &(*pSurface),
      Acts::PropagatorPlainOptions()};

  std::vector<Acts::SourceLink> actsSls;
  for(const auto& sl: sourceLinks){
    actsSls.push_back(Acts::SourceLink{sl});
  }

  ATH_MSG_DEBUG("Invoke fitter");
  FaserActsOutlierFinder faserActsOutlierFinder{0};
  faserActsOutlierFinder.cluster_z=-1000000.;
  faserActsOutlierFinder.StateChiSquaredPerNumberDoFCut=10000.;
 
  auto result = (*m_fit)(actsSls, ini_Param, options, MeasurementCalibratorAdapter(MeasurementCalibrator(), measurements), faserActsOutlierFinder, tracks);

  if (result.ok()) {
    const auto& track = result.value();
    if (track.hasReferenceSurface()) {
      //const auto& params = fitOutput.fittedParameters.value();
      //ATH_MSG_DEBUG("ReFitted parameters");
      //ATH_MSG_DEBUG("  params:   " << params.parameters().transpose());
      //ATH_MSG_DEBUG("  position: " << params.position(gctx).transpose());
      //ATH_MSG_DEBUG("  momentum: " << params.momentum().transpose());
      //ATH_MSG_DEBUG("  charge:   " << params.charge());
      for (const auto& state : track.trackStatesReversed()) {
        auto flag = state.typeFlags();
        //@todo: this only check outlier state? 
        if (not flag.test(Acts::TrackStateFlag::OutlierFlag))
        {
          continue;
        }

        if (state.hasUncalibratedSourceLink()&&state.hasSmoothed()) {
          //todo: make the particle hypothesis configurable
          Acts::BoundTrackParameters parameter(
          state.referenceSurface().getSharedPtr(),
          state.smoothed(),
          state.smoothedCovariance(), Acts::ParticleHypothesis::muon());
          // auto covariance = state.smoothedCovariance();
          auto H = state.effectiveProjector();
          auto residual = state.effectiveCalibrated() - H * state.smoothed();
           IndexSourceLink sl = state.getUncalibratedSourceLink().template get<IndexSourceLink>();
          const Tracker::FaserSCT_Cluster* cluster = sl.hit();
          // const auto& surface = state.referenceSurface();
          Acts::BoundVector meas = state.projector().transpose() * state.effectiveCalibrated();
          Acts::Vector2 local(meas[Acts::eBoundLoc0], meas[Acts::eBoundLoc1]);
          // const Acts::Vector3 dir = Acts::makeDirectionUnitFromPhiTheta(meas[Acts::eBoundPhi], meas[Acts::eBoundTheta]);
          resi.push_back({local.x(),local.y(),parameter.position(gctx).x(), parameter.position(gctx).y(), parameter.position(gctx).z(), cluster,residual(Acts::eBoundLoc0),parameter});
          ATH_MSG_DEBUG("  residual/global position:   " << residual(Acts::eBoundLoc0)<<" "<<parameter.position(gctx).x()<<" "<<parameter.position(gctx).y()<<" "<<parameter.position(gctx).z());
        }
      }
    } else {
      ATH_MSG_DEBUG("No fitted parameters for track");
    }
  }
  return resi;

}

//get the unbiased residual for IFT, i.e. remove whole IFT in the track fitting
//output is a vector of the outliers
//
std::vector<TSOS4Residual>
KalmanFitterTool::getUnbiasedResidual(const EventContext &ctx, const Acts::GeometryContext &gctx,
                      Trk::Track* inputTrack,  const Acts::BoundVector& inputVector,
                      bool /*isMC*/, double origin) const {
  std::vector<TSOS4Residual> resi;
  resi.clear();

  if (!inputTrack->measurementsOnTrack() || inputTrack->measurementsOnTrack()->size() < m_minMeasurements) {
    ATH_MSG_DEBUG("Input track has no or too little measurements and cannot be fitted");
    return resi;
  }

  if (!inputTrack->trackParameters() || inputTrack->trackParameters()->empty()) {
    ATH_MSG_DEBUG("Input track has no track parameters and cannot be fitted");
    return resi;
  }


  auto pSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3 {0, 0, origin}, Acts::Vector3{0, 0, -1});

  Acts::MagneticFieldContext mfContext = getMagneticFieldContext(ctx);
  Acts::CalibrationContext calibContext = Acts::CalibrationContext();

  auto [sourceLinks, measurements] = getMeasurementsFromTrack(inputTrack/*, wafer_id*/);
  auto trackParameters = getParametersFromTrack(inputTrack->trackParameters()->front(), inputVector, origin);
  ATH_MSG_DEBUG("trackParameters: " << trackParameters.parameters().transpose());
  ATH_MSG_DEBUG("position: " << trackParameters.position(gctx).transpose());
  ATH_MSG_DEBUG("momentum: " << trackParameters.momentum().transpose());
  ATH_MSG_DEBUG("charge: " << trackParameters.charge());

  auto actsTrackContainer = std::make_shared<Acts::VectorTrackContainer>();
  auto actsTrackStateContainer = std::make_shared<Acts::VectorMultiTrajectory>();
  FaserActsTrackContainer tracks(actsTrackContainer, actsTrackStateContainer);

  //@todo: the initialSurface should be targetSurface
  GeneralFitterOptions options{
      gctx, mfContext, calibContext, &(*pSurface),
      Acts::PropagatorPlainOptions()};

  std::vector<Acts::SourceLink> actsSls;
  for(const auto& sl: sourceLinks){
    actsSls.push_back(Acts::SourceLink{sl});
  }

  ATH_MSG_DEBUG("Invoke fitter");
  FaserActsOutlierFinder faserActsOutlierFinder{0};
  faserActsOutlierFinder.cluster_z=-1000000.;
  faserActsOutlierFinder.StateChiSquaredPerNumberDoFCut=10000.;
 
  auto result = (*m_fit)(actsSls, trackParameters, options, MeasurementCalibratorAdapter(MeasurementCalibrator(), measurements), faserActsOutlierFinder, tracks);

  if (result.ok()) {
    const auto& track =  result.value();
    if (track.hasReferenceSurface()) {
      //const auto& params = track.fittedParameters.value();
      //ATH_MSG_DEBUG("ReFitted parameters");
      //ATH_MSG_DEBUG("  params:   " << params.parameters().transpose());
      //ATH_MSG_DEBUG("  position: " << params.position(gctx).transpose());
      //ATH_MSG_DEBUG("  momentum: " << params.momentum().transpose());
      //ATH_MSG_DEBUG("  charge:   " << params.charge());
      for (const auto& state : track.trackStatesReversed()) {
        auto flag = state.typeFlags();
        //@todo: this only check outlier state? 
        if (not flag.test(Acts::TrackStateFlag::OutlierFlag))
        {
          continue;
        }

        if (state.hasUncalibratedSourceLink()&&state.hasSmoothed()) {
          //todo: make the particle hypothesis configurable
          Acts::BoundTrackParameters parameter(
          state.referenceSurface().getSharedPtr(),
          state.smoothed(),
          state.smoothedCovariance(), Acts::ParticleHypothesis::muon());
          // auto covariance = state.smoothedCovariance();
          auto H = state.effectiveProjector();
          auto residual = state.effectiveCalibrated() - H * state.smoothed();
          IndexSourceLink sl = state.getUncalibratedSourceLink().template get<IndexSourceLink>();
          const Tracker::FaserSCT_Cluster* cluster = sl.hit();
          // const auto& surface = state.referenceSurface();
          Acts::BoundVector meas = state.projector().transpose() * state.effectiveCalibrated();
          Acts::Vector2 local(meas[Acts::eBoundLoc0], meas[Acts::eBoundLoc1]);
          // const Acts::Vector3 dir = Acts::makeDirectionUnitFromPhiTheta(meas[Acts::eBoundPhi], meas[Acts::eBoundTheta]);
          resi.push_back({local.x(),local.y(),parameter.position(gctx).x(), parameter.position(gctx).y(), parameter.position(gctx).z(), cluster,residual(Acts::eBoundLoc0),parameter});
          ATH_MSG_DEBUG("  residual/global position:   " << residual(Acts::eBoundLoc0)<<" "<<parameter.position(gctx).x()<<" "<<parameter.position(gctx).y()<<" "<<parameter.position(gctx).z());
        }
      }
    } else {
      ATH_MSG_DEBUG("No fitted parameters for track");
    }
  }
  return resi;

}

std::unique_ptr<Trk::Track>
KalmanFitterTool::fit(const EventContext &ctx, const Acts::GeometryContext &gctx,
                      Trk::Track* inputTrack, const Acts::BoundVector& inputVector,
                      bool isMC) const {
  std::unique_ptr<Trk::Track> newTrack = nullptr;

  if (!inputTrack->measurementsOnTrack() || inputTrack->measurementsOnTrack()->size() < m_minMeasurements) {
    ATH_MSG_WARNING("Input track has only " << inputTrack->measurementsOnTrack()->size() <<" measurements and cannot be fitted");
    return nullptr;
  }

  if (!inputTrack->trackParameters() || inputTrack->trackParameters()->empty()) {
    ATH_MSG_WARNING("Input track has no track parameters and cannot be fitted");
    return nullptr;
  }


  // set the start position 5 mm in front of the first track measurement
  double origin = inputTrack->trackParameters()->front()->position().z() - 10;

  auto pSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3 {0, 0, origin}, Acts::Vector3{0, 0, -1});

  Acts::MagneticFieldContext mfContext = getMagneticFieldContext(ctx);
  Acts::CalibrationContext calibContext = Acts::CalibrationContext();

  auto [sourceLinks, measurements] = getMeasurementsFromTrack(inputTrack);
  auto trackParameters = getParametersFromTrack(inputTrack->trackParameters()->front(), inputVector, origin);
  //Inflate the covariance of the starting track parameters
  //@todo: make the inflation configurable
  if(trackParameters.covariance().has_value()){ 
    trackParameters.covariance() = (trackParameters.covariance().value())*10; 
  }
 
  ATH_MSG_DEBUG("trackParameters: " << trackParameters.parameters().transpose());
  ATH_MSG_DEBUG("position: " << trackParameters.position(gctx).transpose());
  ATH_MSG_DEBUG("momentum: " << trackParameters.momentum().transpose());
  ATH_MSG_DEBUG("charge: " << trackParameters.charge());

  auto actsTrackContainer = std::make_shared<Acts::VectorTrackContainer>();
  auto actsTrackStateContainer = std::make_shared<Acts::VectorMultiTrajectory>();
  FaserActsTrackContainer tracks(actsTrackContainer, actsTrackStateContainer);

  //@todo: the initialSurface should be targetSurface
  GeneralFitterOptions options{
      gctx, mfContext, calibContext, &(*pSurface),
      Acts::PropagatorPlainOptions()};

  std::vector<Acts::SourceLink> actsSls;
  for(const auto& sl: sourceLinks){
    actsSls.push_back(Acts::SourceLink{sl});
  }

  ATH_MSG_DEBUG("Invoke fitter");
  FaserActsOutlierFinder faserActsOutlierFinder{0};
  faserActsOutlierFinder.cluster_z=-1000000.;
  faserActsOutlierFinder.StateChiSquaredPerNumberDoFCut=10000.;
  
  auto result = (*m_fit)(actsSls, trackParameters, options, MeasurementCalibratorAdapter(MeasurementCalibrator(), measurements), faserActsOutlierFinder, tracks);

  if (result.ok()) {
    const auto& track = result.value();
    //if (track.hasReferenceSurface()) {
      //const auto& params = track.fittedParameters.value();
      //ATH_MSG_DEBUG("ReFitted parameters");
      //ATH_MSG_DEBUG("  params:   " << params.parameters().transpose());
      //ATH_MSG_DEBUG("  position: " << params.position(gctx).transpose());
      //ATH_MSG_DEBUG("  momentum: " << params.momentum().transpose());
      //ATH_MSG_DEBUG("  charge:   " << params.charge());
      //using IndexedParams = std::unordered_map<size_t, Acts::BoundTrackParameters>;
      //IndexedParams indexedParams;
      //indexedParams.emplace(fitOutput.lastMeasurementIndex, std::move(params));
      //std::vector<size_t> trackTips;
      //trackTips.reserve(1);
      //trackTips.emplace_back(fitOutput.lastMeasurementIndex);
      //myTrajectories.emplace_back(std::move(fitOutput.fittedStates),
      //                            std::move(trackTips),
      //                            std::move(indexedParams));
    //} else {
    //  ATH_MSG_DEBUG("No fitted parameters for track");
    //}
    newTrack = std::move(m_createTrkTrackTool->createTrack(gctx, track));
  } else {
    ATH_MSG_WARNING("Fit failed for parameters starting at " << trackParameters.position(gctx).transpose() <<" and " << actsSls.size()  <<" measurements with error" << result.error());
  } 


  if (m_statesWriter && !m_noDiagnostics) {
    StatusCode statusStatesWriterTool = m_trajectoryStatesWriterTool->write(gctx, tracks, isMC);
  }
  if (m_summaryWriter && !m_noDiagnostics) {
    StatusCode statusSummaryWriterTool = m_trajectorySummaryWriterTool->write(gctx, tracks, isMC);
  }

  return std::move(newTrack);
}



Acts::MagneticFieldContext KalmanFitterTool::getMagneticFieldContext(const EventContext& ctx) const {
  SG::ReadCondHandle<FaserFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  if (!readHandle.isValid()) {
    std::stringstream msg;
    msg << "Failed to retrieve magnetic field condition data " << m_fieldCondObjInputKey.key() << ".";
    throw std::runtime_error(msg.str());
  }
  const FaserFieldCacheCondObj* fieldCondObj{*readHandle};
  return Acts::MagneticFieldContext(fieldCondObj);
}


std::tuple<std::vector<IndexSourceLink>, std::vector<Measurement>>
KalmanFitterTool::getMeasurementsFromTrack(Trk::Track *track, std::vector<const Tracker::FaserSCT_Cluster*>& clusters) const {
  const int kSize = 1;
  ATH_MSG_DEBUG("clusters in refit "<<clusters.size());
  std::array<Acts::BoundIndices, kSize> Indices = {Acts::eBoundLoc0};
  using ThisMeasurement = Acts::Measurement<Acts::BoundIndices, kSize>;
  using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;

  std::shared_ptr<IdentifierMap> identifierMap = m_trackingGeometryTool->getIdentifierMap();
  std::vector<IndexSourceLink> sourceLinks;
  std::vector<Measurement> measurements;
  for(auto cluster : clusters){
      Identifier id = cluster->identify();
      int ista = m_idHelper->station(id);
      int ilay = m_idHelper->layer(id);
      ATH_MSG_DEBUG("station/layer "<<ista<<" "<<ilay);
      Identifier waferId = m_idHelper->wafer_id(id);
      if (identifierMap->count(waferId) != 0) {
      ATH_MSG_DEBUG("found the identifier "<<ista<<" "<<ilay);
        Acts::GeometryIdentifier geoId = identifierMap->at(waferId);
        IndexSourceLink sourceLink(geoId, measurements.size(), cluster);
        Eigen::Matrix<double, 1, 1> pos {cluster->localPosition().x()};
        Eigen::Matrix<double, 1, 1> cov {0.08 * 0.08 / 12};
        ThisMeasurement actsMeas(Acts::SourceLink{std::move(sourceLink)}, Indices, pos, cov);
        sourceLinks.push_back(sourceLink);
        measurements.emplace_back(std::move(actsMeas));
      }
  }
  for (const Trk::MeasurementBase *meas : *track->measurementsOnTrack()) {
    const auto* clusterOnTrack = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*>(meas);
    const Tracker::FaserSCT_Cluster* cluster = clusterOnTrack->prepRawData();
    if (clusterOnTrack) {
      Identifier id = clusterOnTrack->identify();
      Identifier waferId = m_idHelper->wafer_id(id);
      if (identifierMap->count(waferId) != 0) {
        Acts::GeometryIdentifier geoId = identifierMap->at(waferId);
        IndexSourceLink sourceLink(geoId, measurements.size(), cluster);
        Eigen::Matrix<double, 1, 1> pos {meas->localParameters()[Trk::locX],};
        Eigen::Matrix<double, 1, 1> cov {0.08 * 0.08 / 12};
        ThisMeasurement actsMeas(Acts::SourceLink{std::move(sourceLink)}, Indices, pos, cov);
        sourceLinks.push_back(sourceLink);
        measurements.emplace_back(std::move(actsMeas));
      }
    }
  }
  return std::make_tuple(sourceLinks, measurements);
}

std::tuple<std::vector<IndexSourceLink>, std::vector<Measurement>>
KalmanFitterTool::getMeasurementsFromTrack(Trk::Track *track, Identifier& /*wafer_id*/) const {
  const int kSize = 1;
  std::array<Acts::BoundIndices, kSize> Indices = {Acts::eBoundLoc0};
  using ThisMeasurement = Acts::Measurement<Acts::BoundIndices, kSize>;
  using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;

  std::shared_ptr<IdentifierMap> identifierMap = m_trackingGeometryTool->getIdentifierMap();
  std::vector<IndexSourceLink> sourceLinks;
  std::vector<Measurement> measurements;
  for (const Trk::MeasurementBase *meas : *track->measurementsOnTrack()) {
    const auto* clusterOnTrack = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*>(meas);
    const Tracker::FaserSCT_Cluster* cluster = clusterOnTrack->prepRawData();
    if (clusterOnTrack) {
      Identifier id = clusterOnTrack->identify();
      Identifier waferId = m_idHelper->wafer_id(id);

      if (identifierMap->count(waferId) != 0) {
        Acts::GeometryIdentifier geoId = identifierMap->at(waferId);
        IndexSourceLink sourceLink(geoId, measurements.size(), cluster);
        Eigen::Matrix<double, 1, 1> pos {meas->localParameters()[Trk::locX],};
        Eigen::Matrix<double, 1, 1> cov {0.08 * 0.08 / 12};
        ThisMeasurement actsMeas(Acts::SourceLink{std::move(sourceLink)}, Indices, pos, cov);
        sourceLinks.push_back(sourceLink);
        measurements.emplace_back(std::move(actsMeas));
      }
    }
  }
  return std::make_tuple(sourceLinks, measurements);
}
std::tuple<std::vector<IndexSourceLink>, std::vector<Measurement>>
KalmanFitterTool::getMeasurementsFromTrack(Trk::Track *track) const {
  const int kSize = 1;
  std::array<Acts::BoundIndices, kSize> Indices = {Acts::eBoundLoc0};
  using ThisMeasurement = Acts::Measurement<Acts::BoundIndices, kSize>;
  using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;

  std::shared_ptr<IdentifierMap> identifierMap = m_trackingGeometryTool->getIdentifierMap();
  std::vector<IndexSourceLink> sourceLinks;
  std::vector<Measurement> measurements;
  for (const Trk::MeasurementBase *meas : *track->measurementsOnTrack()) {
    const auto* clusterOnTrack = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*>(meas);
    const Tracker::FaserSCT_Cluster* cluster = clusterOnTrack->prepRawData();
    if (clusterOnTrack) {
      Identifier id = clusterOnTrack->identify();
      Identifier waferId = m_idHelper->wafer_id(id);
      if (identifierMap->count(waferId) != 0) {
        Acts::GeometryIdentifier geoId = identifierMap->at(waferId);
        IndexSourceLink sourceLink(geoId, measurements.size(), cluster);
        Eigen::Matrix<double, 1, 1> pos {meas->localParameters()[Trk::locX],};
        Eigen::Matrix<double, 1, 1> cov {0.08 * 0.08 / 12};
        ThisMeasurement actsMeas(Acts::SourceLink{std::move(sourceLink)}, Indices, pos, cov);
        sourceLinks.push_back(sourceLink);
        measurements.emplace_back(std::move(actsMeas));
      }
    }
  }
  return std::make_tuple(sourceLinks, measurements);
}


Acts::BoundTrackParameters
KalmanFitterTool::getParametersFromTrack(const Trk::TrackParameters *inputParameters,
                                         const Acts::BoundVector& inputVector, double origin) const {
  using namespace Acts::UnitLiterals;
  std::shared_ptr<const Acts::Surface> pSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3 {0, 0, origin}, Acts::Vector3{0, 0, -1});

  auto atlasParam = inputParameters->parameters();
  auto center = inputParameters->associatedSurface().center();
  // Acts::BoundVector params {center[Trk::locY], center[Trk::locX],
  //                           atlasParam[Trk::phi0], atlasParam[Trk::theta],
  //                           charge / (inputParameters->momentum().norm() * 1_MeV),
  //                           0.};
  Acts::BoundVector params;
  if (inputVector == Acts::BoundVector::Zero()) {
    params(0) = center[Trk::locY];
    params(1) = center[Trk::locX];
    params(2) = atlasParam[Trk::phi0];
    params(3) = atlasParam[Trk::theta];
    params(4) = inputParameters->charge() / (inputParameters->momentum().norm() * 1_MeV);
    params(5) = 0.;
  } else {
    params = inputVector;
  }
  Acts::BoundSquareMatrix cov = Acts::BoundSquareMatrix::Identity();
  cov.topLeftCorner(5, 5) = *inputParameters->covariance();

  for(int i=0; i < cov.rows(); i++){
    cov(i, 4) = cov(i, 4)/1_MeV;
  }
  for(int i=0; i < cov.cols(); i++){
    cov(4, i) = cov(4, i)/1_MeV;
  }
  for (int i = 0; i < 6; ++i) {
    cov(i,i) = m_seedCovarianceScale * cov(i,i);
  }

  return Acts::BoundTrackParameters(pSurface, params, cov, Acts::ParticleHypothesis::muon());
}
