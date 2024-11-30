/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserActsKalmanFilterAlg.h"
#include "FaserActsGeometry/FASERMagneticFieldWrapper.h"

// ATHENA
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/PhysicalConstants.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorDesign.h"
#include "TrackerReadoutGeometry/SiLocalPosition.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerSpacePoint/SpacePointForSeedCollection.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "FaserDetDescr/FaserDetectorID.h"

// ACTS
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Surfaces/RectangleBounds.hpp"
#include "Acts/Definitions/Units.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/detail/periodic.hpp"
#include "Acts/Definitions/Common.hpp"
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Definitions/TrackParametrization.hpp"
#include "Acts/Utilities/CalibrationContext.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/EventData/MultiTrajectoryHelpers.hpp"
#include "Acts/EventData/Measurement.hpp"


// PACKAGE
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "ActsInterop/Logger.h"
#include "FaserActsGeometry/FaserActsGeometryContext.h"
#include "FaserActsGeometryInterfaces/IFaserActsPropStepRootWriterSvc.h"
#include "FaserActsGeometry/FaserActsDetectorElement.h"
#include "FaserActsKalmanFilter/IdentifierLink.h"

#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"

//ROOT
#include <TTree.h>

// BOOST
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
// #include <boost/variant/static_visitor.hpp>

// STL
#include <string>
#include <fstream>
#include <cmath>


using namespace Acts::UnitLiterals;
using Acts::VectorHelpers::eta;
using Acts::VectorHelpers::perp;
using Acts::VectorHelpers::phi;
using Acts::VectorHelpers::theta;
using ThisMeasurement = Acts::Measurement<Acts::BoundIndices, 2>;
using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;

FaserActsKalmanFilterAlg::FaserActsKalmanFilterAlg(const std::string& name, ISvcLocator* pSvcLocator) :
   AthAlgorithm(name, pSvcLocator)  {}


StatusCode FaserActsKalmanFilterAlg::initialize() {
  ATH_CHECK(m_fieldCondObjInputKey.initialize());
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  ATH_CHECK(m_trackFinderTool.retrieve());
//todo  ATH_CHECK(m_trajectoryWriterTool.retrieve());
  ATH_CHECK(m_trajectoryStatesWriterTool.retrieve());
//  ATH_CHECK(m_protoTrackWriterTool.retrieve());
  ATH_CHECK(m_trackCollection.initialize());
  ATH_CHECK(detStore()->retrieve(m_idHelper,"FaserSCT_ID"));

  if (m_actsLogging == "VERBOSE") {
    m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::VERBOSE);
  } else if (m_actsLogging == "DEBUG") {
    m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::DEBUG);
  } else {
    m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::INFO);
  }
  
  auto magneticField = std::make_shared<FASERMagneticFieldWrapper>();
  double reverseFilteringMomThreshold = 0.1; //@todo: needs to be validated
  //@todo: the multiple scattering and energy loss are originallly turned off 
  m_fit = makeTrackFitterFunction(m_trackingGeometryTool->trackingGeometry(), magneticField, false, false, reverseFilteringMomThreshold, Acts::FreeToBoundCorrection(false), *m_logger);

  return StatusCode::SUCCESS;
}


//StatusCode FaserActsKalmanFilterAlg::execute(const EventContext& ctx) const {
StatusCode FaserActsKalmanFilterAlg::execute() {
  const EventContext& ctx = Gaudi::Hive::currentContext();

  //make TrackCollection
  ATH_CHECK(m_trackCollection.initialize());
  SG::WriteHandle<TrackCollection> trackContainer{m_trackCollection,ctx};
  std::unique_ptr<TrackCollection> outputTracks = std::make_unique<TrackCollection>();

  std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry
      = m_trackingGeometryTool->trackingGeometry();

  const FaserActsGeometryContext& gctx = m_trackingGeometryTool->getGeometryContext();
  auto geoctx = gctx.context();
  Acts::MagneticFieldContext magctx = getMagneticFieldContext(ctx);
  Acts::CalibrationContext calctx;

  CHECK(m_trackFinderTool->run());
  std::shared_ptr<const Acts::Surface> initialSurface =
      m_trackFinderTool->initialSurface();
  std::shared_ptr<std::vector<Acts::BoundTrackParameters>> initialTrackParameters =
      m_trackFinderTool->initialTrackParameters();
  std::shared_ptr<std::vector<std::vector<IndexSourceLink>>> sourceLinks =
      m_trackFinderTool->sourceLinks();
  std::shared_ptr<std::vector<IdentifierLink>> idLinks = m_trackFinderTool->idLinks();
  std::shared_ptr<std::vector<std::vector<Measurement>>> measurements =
      m_trackFinderTool->measurements();
  std::shared_ptr<std::vector<std::vector<Tracker::FaserSCT_SpacePoint*>>> spacePoints =
      m_trackFinderTool->spacePoints();
  std::shared_ptr<std::vector<std::vector<const Tracker::FaserSCT_Cluster*>>> clusters =
      m_trackFinderTool->clusters();

//  ATH_CHECK(m_protoTrackWriterTool->write(initialTrackParameters, measurements, geoctx));

  int n_trackSeeds = initialTrackParameters->size();

  auto actsTrackContainer = std::make_shared<Acts::VectorTrackContainer>();
  auto actsTrackStateContainer = std::make_shared<Acts::VectorMultiTrajectory>();
  FaserActsTrackContainer tracks(actsTrackContainer, actsTrackStateContainer);

  //@todo: the initialSurface should be targetSurface
  GeneralFitterOptions options{
      geoctx, magctx, calctx, &(*initialSurface),
      Acts::PropagatorPlainOptions()};

  for (int i = 0; i < n_trackSeeds; ++i) {
    ATH_MSG_DEBUG("Invoke fitter");
    auto sls = sourceLinks->at(i);
    std::vector<Acts::SourceLink> actsSls;
    for(const auto& sl: sls){
      actsSls.push_back(Acts::SourceLink{sl}); 
    }
    auto result = (*m_fit)(actsSls, initialTrackParameters->at(i), options, MeasurementCalibratorAdapter(MeasurementCalibrator(), measurements->at(i)), tracks);

    int itrack = 0;
    if (result.ok()) {
      // Get the fit output object
      //const auto& fitOutput = result.value();
      std::unique_ptr<Trk::Track> track = makeTrack(geoctx, result, clusters->at(i));
       if (track) {
         outputTracks->push_back(std::move(track));
       }

    //  // The track entry indices container. One element here.
    //  std::vector<size_t> trackTips;
    //  trackTips.reserve(1);
    //  trackTips.emplace_back(fitOutput.lastMeasurementIndex);
    //  // The fitted parameters container. One element (at most) here.
    //  IndexedParams indexedParams;

    //  if (fitOutput.fittedParameters) {
    //    const auto& params = fitOutput.fittedParameters.value();
    //    ATH_MSG_VERBOSE("Fitted paramemeters for track " << itrack);
    //    ATH_MSG_VERBOSE("  parameters: " << params);
    //    ATH_MSG_VERBOSE("  position: " << params.position(geoctx).transpose());
    //    ATH_MSG_VERBOSE("  momentum: " << params.momentum().transpose());
    //    // Push the fitted parameters to the container
    //    indexedParams.emplace(fitOutput.lastMeasurementIndex, std::move(params));
    //  } else {
    //    ATH_MSG_DEBUG("No fitted paramemeters for track " << itrack);
    //  }
    //  // Create a SimMultiTrajectory
    //  trajectories.emplace_back(std::move(fitOutput.fittedStates),
    //                            std::move(trackTips), std::move(indexedParams));
    } else {
      ATH_MSG_WARNING("Fit failed for track " << itrack << " with error" << result.error());
      // Fit failed, but still create a empty truth fit track
    //  trajectories.push_back(FaserActsRecMultiTrajectory());
    }

  }

//  std::vector<Acts::CurvilinearTrackParameters> initialTrackParametersVector {*initialTrackParameters};
//  m_trajectoryWriterTool->writeout(trajectories, geoctx, initialTrackParametersVector);
//  ATH_CHECK(m_trajectoryStatesWriterTool->write(trajectories, geoctx, *idLinks));

  ATH_CHECK(trackContainer.record(std::move(outputTracks)));

  return StatusCode::SUCCESS;
}

StatusCode FaserActsKalmanFilterAlg::finalize() {
  return StatusCode::SUCCESS;
}


Acts::MagneticFieldContext FaserActsKalmanFilterAlg::getMagneticFieldContext(const EventContext& ctx) const {
  SG::ReadCondHandle<FaserFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  if (!readHandle.isValid()) {
    std::stringstream msg;
    msg << "Failed to retrieve magnetic field condition data " << m_fieldCondObjInputKey.key() << ".";
    throw std::runtime_error(msg.str());
  }
  const FaserFieldCacheCondObj* fieldCondObj{*readHandle};
  return Acts::MagneticFieldContext(fieldCondObj);
}


std::unique_ptr<Trk::Track>
FaserActsKalmanFilterAlg::makeTrack(Acts::GeometryContext& geoCtx, TrackFitterResult& fitResult, std::vector<const Tracker::FaserSCT_Cluster*> clusters) const {
  std::unique_ptr<Trk::Track> newtrack = nullptr;
  //Get the fit output object
  const auto& track = fitResult.value();
  if (track.hasReferenceSurface()) {
    std::unique_ptr<DataVector<const Trk::TrackStateOnSurface>> finalTrajectory = std::make_unique<DataVector<const Trk::TrackStateOnSurface>>();
    std::vector<std::unique_ptr<const Acts::BoundTrackParameters>> actsSmoothedParam;
    // Loop over all the output state to create track state
    for (const auto& state : track.trackStatesReversed()) { 
      auto flag = state.typeFlags();
      if (state.referenceSurface().associatedDetectorElement() != nullptr) {
        // We need to determine the type of state
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
        std::unique_ptr<Trk::TrackParameters> parm; 

        // State is a hole (no associated measurement), use predicted para meters
        if (flag.test(Acts::TrackStateFlag::HoleFlag) == true) {
          //@todo: ParticleHypothesis? 
          const Acts::BoundTrackParameters actsParam(state.referenceSurface().getSharedPtr(),
                                                     state.predicted(),
                                                     state.predictedCovariance(), Acts::ParticleHypothesis::muon());
          parm = std::move(ConvertActsTrackParameterToATLAS(actsParam, geoCtx));
          // auto boundaryCheck = m_boundaryCheckTool->boundaryCheck(*p arm);
          typePattern.set(Trk::TrackStateOnSurface::Hole);
        }
          // The state was tagged as an outlier, use filtered parameters
        else if (flag.test(Acts::TrackStateFlag::OutlierFlag) == true) {
          //@todo: ParticleHypothesis? 
          const Acts::BoundTrackParameters actsParam(state.referenceSurface().getSharedPtr(),
                                                     state.filtered(), state.filteredCovariance(), Acts::ParticleHypothesis::muon());
          parm = std::move(ConvertActsTrackParameterToATLAS(actsParam, geoCtx));
          typePattern.set(Trk::TrackStateOnSurface::Outlier);
        }
          // The state is a measurement state, use smoothed parameters
        else {
          //@todo: ParticleHypothesis? 
          const Acts::BoundTrackParameters actsParam(state.referenceSurface().getSharedPtr(),
                                                     state.smoothed(), state.smoothedCovariance(), Acts::ParticleHypothesis::muon());
          actsSmoothedParam.push_back(std::make_unique<const Acts::BoundTrackParameters>(Acts::BoundTrackParameters(actsParam)));
          //  const auto& psurface=actsParam.referenceSurface();
          Acts::Vector2 local(actsParam.parameters()[Acts::eBoundLoc0], actsParam.parameters()[Acts::eBoundLoc1]);
          //  const Acts::Vector3 dir = Acts::makeDirectionUnitFromPhiTheta(actsParam.parameters()[Acts::eBoundPhi], actsParam.parameters()[Acts::eBoundTheta]);
          //  auto pos=actsParam.position(tgContext);
          parm = std::move(ConvertActsTrackParameterToATLAS(actsParam, geoCtx));
          typePattern.set(Trk::TrackStateOnSurface::Measurement);
        }
        std::unique_ptr<Tracker::FaserSCT_ClusterOnTrack> measState = nullptr;
        if (state.hasUncalibratedSourceLink()) {
          auto sl = state.getUncalibratedSourceLink().template get<IndexSourceLink>();
          const Tracker::FaserSCT_Cluster* fitCluster = clusters.at(sl.index());
          if (fitCluster->detectorElement() != nullptr) {
            measState = std::make_unique<Tracker::FaserSCT_ClusterOnTrack>(
                fitCluster,
                Trk::LocalParameters{
                    Trk::DefinedParameter{fitCluster->localPosition()[0], Trk::loc1},
                    Trk::DefinedParameter{fitCluster->localPosition()[1], Trk::loc2}
                },
                Amg::MatrixX(fitCluster->localCovariance()),
                m_idHelper->wafer_hash(fitCluster->detectorElement()->identify())
            );
          }
        }
        double nDoF = state.calibratedSize();
        const Trk::FitQualityOnSurface *quality = new Trk::FitQualityOnSurface(state.chi2(), nDoF);
        const Trk::TrackStateOnSurface *perState = new Trk::TrackStateOnSurface(*quality, std::move(measState), std::move(parm), nullptr, typePattern);
        // If a state was succesfully created add it to the trajectory
        if (perState) {
          (*finalTrajectory).insert((*finalTrajectory).begin(), perState);
        }
      } // state has referenceSurface
    } //end loop for all states

    // Create the track using the states
    const Trk::TrackInfo newInfo(Trk::TrackInfo::TrackFitter::KalmanFitter, Trk::ParticleHypothesis::muon);
    // Trk::FitQuality* q = nullptr;
    // newInfo.setTrackFitter(Trk::TrackInfo::TrackFitter::KalmanFitter     ); //Mark the fitter as KalmanFitter
    newtrack = std::make_unique<Trk::Track>(newInfo, std::move(finalTrajectory), nullptr);
  } // hasFittedParameters

  return newtrack;
}

std::unique_ptr<Trk::TrackParameters>
FaserActsKalmanFilterAlg ::ConvertActsTrackParameterToATLAS(const Acts::BoundTrackParameters &actsParameter, const Acts::GeometryContext& gctx) const      {
  using namespace Acts::UnitLiterals;
  std::optional<AmgSymMatrix(5)> cov = std::nullopt;
  if (actsParameter.covariance()){
    AmgSymMatrix(5) newcov(actsParameter.covariance()->topLeftCorner(5, 5));
    // Convert the covariance matrix to GeV
    for(int i=0; i < newcov.rows(); i++){
      newcov(i, 4) = newcov(i, 4)*1_MeV;
    }
    for(int i=0; i < newcov.cols(); i++){
      newcov(4, i) = newcov(4, i)*1_MeV;
    }
    cov =  std::optional<AmgSymMatrix(5)>(newcov);
  }
  const Amg::Vector3D& pos=actsParameter.position(gctx);
  double tphi=actsParameter.get<Acts::eBoundPhi>();
  double ttheta=actsParameter.get<Acts::eBoundTheta>();
  double tqOverP=actsParameter.get<Acts::eBoundQOverP>()*1_MeV;
  double p = std::abs(1. / tqOverP);
  Amg::Vector3D tmom(p * std::cos(tphi) * std::sin(ttheta), p * std::sin(tphi) * std::sin(ttheta), p * std::cos(ttheta));
  return std::make_unique<Trk::CurvilinearParameters>(pos, tmom, tqOverP>0, cov);
}

