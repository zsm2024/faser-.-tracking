#include "CKF2.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointCollection.h"
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
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "TrackSelection.h"
#include <algorithm>

#include "FaserActsGeometry/FASERMagneticFieldWrapper.h"

#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/TrackFitting/GainMatrixSmoother.hpp"
#include "Acts/TrackFitting/GainMatrixUpdater.hpp"
#include "Acts/EventData/Measurement.hpp"
#include "Acts/Propagator/PropagatorError.hpp"
#include "Acts/TrackFinding/CombinatorialKalmanFilterError.hpp"
#include "Acts/EventData/ProxyAccessor.hpp"
#include "CircleFitTrackSeedTool.h"

size_t CKF2::TrackInfo::nClusters {0};


CKF2::CKF2(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator) {}


StatusCode CKF2::initialize() {
  ATH_CHECK(m_fieldCondObjInputKey.initialize());
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  ATH_CHECK(m_trackSeedTool.retrieve());
  ATH_CHECK(m_kalmanFitterTool1.retrieve());
  ATH_CHECK(m_createTrkTrackTool.retrieve());
  ATH_CHECK(m_trackCollection.initialize());
  // ATH_CHECK(m_allTrackCollection.initialize());
  ATH_CHECK(m_eventInfoKey.initialize());
//  ATH_CHECK(m_selectedSeedTool.retrieve());
  
//todo  if (m_performanceWriter && !m_noDiagnostics) {
//todo    ATH_CHECK(m_performanceWriterTool.retrieve());
//todo  }
  
  if (m_statesWriter && !m_noDiagnostics) {
    ATH_CHECK(m_seedWriterTool.retrieve());
  }
  if (m_statesWriter && !m_noDiagnostics) {
    ATH_CHECK(m_trajectoryStatesWriterTool.retrieve());
  }
  if (m_summaryWriter && !m_noDiagnostics) {
    ATH_CHECK(m_trajectorySummaryWriterTool.retrieve());
  }
  ATH_CHECK(detStore()->retrieve(m_idHelper,"FaserSCT_ID"));

  // FIXME fix Acts logging level
  if (m_actsLogging == "VERBOSE") {
    m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::VERBOSE);
  } else if (m_actsLogging == "DEBUG") {
    m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::DEBUG);
  } else {
    m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::INFO);
  }

  auto magneticField = std::make_shared<FASERMagneticFieldWrapper>();
  std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry
    = m_trackingGeometryTool->trackingGeometry();
  double reverseFilteringMomThreshold = 0.1; //@todo: needs to be validated
  //@todo: the multiple scattering and energy loss are originallly turned off 
  m_fitter = makeTrackFitterFunction(trackingGeometry, magneticField, 
                                  true, true, reverseFilteringMomThreshold, Acts::FreeToBoundCorrection(false), *m_logger);
 
  m_finder = makeTrackFinderFunction(trackingGeometry, magneticField,
                                  m_resolvePassive, m_resolveMaterial, m_resolveSensitive, *m_logger);

  return StatusCode::SUCCESS;
}


StatusCode CKF2::execute() {
  const EventContext& ctx = Gaudi::Hive::currentContext();
  m_numberOfEvents++;

  // Should work, but won't compile
  // SG::ReadHandle<xAOD::EventInfo> eventInfo { m_eventInfoKey, ctx };

  SG::WriteHandle trackContainer{m_trackCollection, ctx};
  std::unique_ptr<TrackCollection> outputTracks = std::make_unique<TrackCollection>();

  // SG::WriteHandle allTrackContainer{m_allTrackCollection, ctx};
  // std::unique_ptr<TrackCollection> outputAllTracks = std::make_unique<TrackCollection>();

  std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry
      = m_trackingGeometryTool->trackingGeometry();

  const FaserActsGeometryContext& faserActsGeometryContext = m_trackingGeometryTool->getGeometryContext();
  auto gctx = faserActsGeometryContext.context();
  Acts::MagneticFieldContext magFieldContext = getMagneticFieldContext(ctx);
  Acts::CalibrationContext calibContext;

  CHECK(m_trackSeedTool->run(m_maskedLayers, m_backwardPropagation));
  std::shared_ptr<const Acts::Surface> targetSurface =
      m_trackSeedTool->initialSurface();
  std::shared_ptr<std::vector<Acts::BoundTrackParameters>> initialParameters =
      m_trackSeedTool->initialTrackParameters();
  std::shared_ptr<std::vector<IndexSourceLink>> sourceLinks =
      m_trackSeedTool->sourceLinks();
  double targetZposition = m_trackSeedTool->targetZPosition();
  
  std::shared_ptr<std::vector<Measurement>> measurements = m_trackSeedTool->measurements();
  std::shared_ptr<std::vector<const Tracker::FaserSCT_Cluster*>> clusters = m_trackSeedTool->clusters();
  std::shared_ptr<std::vector<const Tracker::FaserSCT_SpacePoint*>> spacePoints = m_trackSeedTool->spacePoints();
  std::shared_ptr<std::vector<std::array<std::vector<const Tracker::FaserSCT_Cluster*>, 3>>> seedClusters = m_trackSeedTool->seedClusters();

  TrackInfo::nClusters = sourceLinks->size();


  CircleFitTrackSeedTool* Tool = dynamic_cast<CircleFitTrackSeedTool*>(m_trackSeedTool.get());
  if(Tool){
    std::shared_ptr<std::vector<CircleFitTrackSeedTool::Seed>> selectedseeds = Tool->seeds();
    if (m_seedWriter && !m_noDiagnostics) {
      ATH_CHECK(m_seedWriterTool->write(gctx, *selectedseeds, m_isMC));
    }
  }
  else{
    ATH_MSG_ERROR("Failed to cast to CircleFitTrackSeedTool");
  }
  
  /*CHECK(m_selectedSeedTool->run(m_maskedLayers, m_backwardPropagation));
  std::shared_ptr<std::vector<CircleFitTrackSeedTool::Seed>> selectedseeds = m_selectedSeedTool->seeds();
  if (m_seedWriter && !m_noDiagnostics) {
    ATH_CHECK(m_seedWriterTool->write(*selectedseeds, m_isMC));
  }*/

  // construct the container 
  auto allTrackContainer = std::make_shared<Acts::VectorTrackContainer>();
  auto allTrackStateContainer = std::make_shared<Acts::VectorMultiTrajectory>();
  auto selectedTrackContainer = std::make_shared<Acts::VectorTrackContainer>();
  auto selectedTrackStateContainer = std::make_shared<Acts::VectorMultiTrajectory>();

  auto trackContainerTemp = std::make_shared<Acts::VectorTrackContainer>();
  auto trackStateContainerTemp =
      std::make_shared<Acts::VectorMultiTrajectory>();

  FaserActsTrackContainer tracks(allTrackContainer, allTrackStateContainer);
  FaserActsTrackContainer tracksTemp(trackContainerTemp, trackStateContainerTemp);
  FaserActsTrackContainer selectedTracks(selectedTrackContainer, selectedTrackStateContainer);

  tracks.addColumn<unsigned int>("trackGroup");
  tracksTemp.addColumn<unsigned int>("trackGroup");
  selectedTracks.addColumn<unsigned int>("trackGroup");
  Acts::ProxyAccessor<unsigned int> seedNumber("trackGroup");
  

  //1) calibrator
  MeasurementCalibratorAdapter calibrator(MeasurementCalibrator(), *measurements);

  //2) slAccessor 
  IndexSourceLinkContainer tmp;
  for (const auto& sl : *sourceLinks) {
    tmp.emplace_hint(tmp.end(), sl);
  }
  IndexSourceLinkAccessor slAccessor;
  slAccessor.container = &tmp;

  //3) measurement selector 
  Acts::MeasurementSelector::Config measurementSelectorCfg = {
      {Acts::GeometryIdentifier(), {{}, {m_chi2Max}, {m_nMax}}},
  };

  // configuration of the targetSurface of CKF in GeneralFitterOptions 
  Acts::RotationMatrix3 rotation = Acts::RotationMatrix3::Identity();
  rotation.col(0) = Acts::Vector3(0, 1, 0);
  rotation.col(1) = Acts::Vector3(1, 0, 0);
  rotation.col(2) = Acts::Vector3(0, 0, -1);
  Acts::Translation3 trans(0., 0., targetZposition);
  Acts::Transform3 trafo(trans*rotation);
  ATH_MSG_DEBUG("ACTS CKF target surface has center at " << trafo.translation().transpose());
  targetSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(trafo);
  
  Acts::PropagatorPlainOptions pOptions;
  pOptions.maxSteps = m_maxSteps;
  if (m_backwardPropagation) {
    pOptions.direction = Acts::Direction::Backward;
  } else {
    pOptions.direction = Acts::Direction::Forward;
  }

  GeneralFitterOptions options{
    gctx, magFieldContext, calibContext, &(*targetSurface), pOptions};

  // Perform the track finding for all initial parameters
  m_numberOfTrackSeeds += initialParameters->size();
  ATH_MSG_DEBUG("Invoke track finding with " << initialParameters->size() << " seeds.");

  unsigned int nSeed = 0;

  for (std::size_t iseed = 0; iseed < (*initialParameters).size(); ++iseed) {
    ATH_MSG_DEBUG("  position: " << (*initialParameters)[iseed].position(gctx).transpose());
    ATH_MSG_DEBUG("  momentum: " << (*initialParameters)[iseed].momentum().transpose());
    ATH_MSG_DEBUG("  charge:   " << (*initialParameters)[iseed].charge());
    // Clear trackContainerTemp and trackStateContainerTemp
    tracksTemp.clear();

    auto result = (*m_finder)((*initialParameters).at(iseed), options, calibrator, slAccessor, Acts::MeasurementSelector(measurementSelectorCfg), tracksTemp);
   
    if (!result.ok()) {
      ATH_MSG_WARNING("Track finding failed for seed " << iseed << " with error" << result.error());
      continue;
    }

    auto& tracksForSeed = result.value();
    m_numberOfFittedTracks += tracksForSeed.size();
    ATH_MSG_DEBUG("Find " << tracksForSeed.size() << " tracks for this seed");
    for (auto& track : tracksForSeed) {
      // Set the seed number, this number decrease by 1 since the seed number
      // has already been updated
      seedNumber(track) = nSeed - 1;
      auto destProxy = tracks.getTrack(tracks.addTrack());
      destProxy.copyFrom(track, true);  // make sure we copy track states!
    }
  }
  
  std::list<TrackInfo> allTracks;
  for(const auto& track: tracks){
    allTracks.push_back(track); 
  }

  // the list of tracks is sorted by the number of measurements using the chi2 value as a tie-breaker
  allTracks.sort([](const TrackInfo &left, const TrackInfo &right) {
    if (left.nMeasurements > right.nMeasurements) return true;
    if (left.nMeasurements < right.nMeasurements) return false;
    if (left.chi2 < right.chi2) return true;
    else return false;
  });

  while (not allTracks.empty()) {
    TrackInfo selected = allTracks.front();
    
    auto destProxy = selectedTracks.getTrack(selectedTracks.addTrack());
    destProxy.copyFrom(tracks.getTrack(selected.index), true);  // make sure we copy track states!

    allTracks.remove_if([&](const TrackInfo &p) {
      return (p.nMeasurements <= 12) || ((p.clusterSet & selected.clusterSet).count() > 6);
    });
  }
  ATH_MSG_DEBUG("There are " << selectedTracks.size() << " selected tracks for this event.");

  for (const auto & track : selectedTracks) {
    std::optional<Acts::BoundTrackParameters> fittedParams = std::nullopt;
    if(track.hasReferenceSurface()){ 
      const auto& params = track.parameters();
      //const auto& qOp = track.qOverP();
      //const auto& charge = track.charge();
      ATH_MSG_DEBUG("Fitted parameters");
      ATH_MSG_DEBUG("  params:   " << params.transpose());
      if(m_addFittedParamsToTrack){
        fittedParams = Acts::BoundTrackParameters(track.referenceSurface().getSharedPtr(), track.parameters(), track.covariance(), Acts::ParticleHypothesis::muon()); 
      }
    } else {
      ATH_MSG_WARNING("No fitted parameters at target position" << targetZposition);
    }

    std::unique_ptr<Trk::Track> trk = std::move(m_createTrkTrackTool->createTrack(gctx, track, fittedParams, m_backwardPropagation));
    m_numberOfSelectedTracks++;
    //@todo: make the KF refit configurable 
    std::unique_ptr<Trk::Track> trk2 = std::move(m_kalmanFitterTool1->fit(
      ctx, gctx, trk.get(), Acts::BoundVector::Zero(), m_isMC));
    if (trk2 != nullptr) {
      outputTracks->push_back(std::move(trk2));
    } else {
      outputTracks->push_back(std::move(trk));
      ATH_MSG_WARNING("Re-Fit failed.");
    }
  }

    //write out all acts found tracks for debugging 
//  // @todo run the performance writer
//  if  (m_performanceWriter && !m_noDiagnostics) {
//    ATH_CHECK(m_performanceWriterTool->write(gctx, tracks));
//  }

  if (m_statesWriter && !m_noDiagnostics) {
    ATH_CHECK(m_trajectoryStatesWriterTool->write(gctx, selectedTracks, m_isMC));
  }
  if (m_summaryWriter && !m_noDiagnostics) {
    ATH_CHECK(m_trajectorySummaryWriterTool->write(gctx, selectedTracks, m_isMC));
  }
  // ATH_CHECK(allTrackContainer.record(std::move(outputAllTracks)));
  ATH_CHECK(trackContainer.record(std::move(outputTracks)));


  return StatusCode::SUCCESS;
}


StatusCode CKF2::finalize() {
  ATH_MSG_INFO("CombinatorialKalmanFilterAlg::finalize()");
  ATH_MSG_INFO("BackwardPropagation: " << m_backwardPropagation);
  ATH_MSG_INFO(m_numberOfEvents << " events processed.");
  ATH_MSG_INFO(m_numberOfTrackSeeds << " seeds.");
  ATH_MSG_INFO(m_numberOfFittedTracks << " fitted tracks.");
  ATH_MSG_INFO(m_numberOfSelectedTracks << " selected and re-fitted tracks.");
  return StatusCode::SUCCESS;
}


Acts::MagneticFieldContext CKF2::getMagneticFieldContext(const EventContext& ctx) const {
  SG::ReadCondHandle<FaserFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  if (!readHandle.isValid()) {
    std::stringstream msg;
    msg << "Failed to retrieve magnetic field condition data " << m_fieldCondObjInputKey.key() << ".";
    throw std::runtime_error(msg.str());
  }
  const FaserFieldCacheCondObj* fieldCondObj{*readHandle};
  return Acts::MagneticFieldContext(fieldCondObj);
}



void CKF2::computeSharedHits(std::vector<IndexSourceLink>* sourceLinks, FaserActsTrackContainer& tracks) const {

  std::vector<std::size_t> firstTrackOnTheHit(
      sourceLinks->size(), std::numeric_limits<std::size_t>::max());
  std::vector<std::size_t> firstStateOnTheHit(
      sourceLinks->size(), std::numeric_limits<std::size_t>::max());

  for (auto track : tracks) {
    for (auto state : track.trackStatesReversed()) {
      if (!state.typeFlags().test(Acts::TrackStateFlag::MeasurementFlag)) {
        continue;
      }

      std::size_t hitIndex = state.getUncalibratedSourceLink()
                                 .template get<IndexSourceLink>()
                                 .index();

      // Check if hit not already used
      if (firstTrackOnTheHit.at(hitIndex) ==
          std::numeric_limits<std::size_t>::max()) {
        firstTrackOnTheHit.at(hitIndex) = track.index();
        firstStateOnTheHit.at(hitIndex) = state.index();
        continue;
      }

      // if already used, control if first track state has been marked
      // as shared
      int indexFirstTrack = firstTrackOnTheHit.at(hitIndex);
      int indexFirstState = firstStateOnTheHit.at(hitIndex);
  
      auto firstState = tracks.getTrack(indexFirstTrack)
                            .container()
                            .trackStateContainer()
                            .getTrackState(indexFirstState);
      if (!firstState.typeFlags().test(Acts::TrackStateFlag::SharedHitFlag)) {
        firstState.typeFlags().set(Acts::TrackStateFlag::SharedHitFlag);
      }

      // Decorate this track state
      state.typeFlags().set(Acts::TrackStateFlag::SharedHitFlag);
    }
  }
}

