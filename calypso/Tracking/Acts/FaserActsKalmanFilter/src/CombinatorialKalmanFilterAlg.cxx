#include "CombinatorialKalmanFilterAlg.h"

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
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "TrackSelection.h"
#include <algorithm>


size_t CombinatorialKalmanFilterAlg::TrajectoryInfo::nClusters {0};

using TrajectoriesContainer = std::vector<FaserActsRecMultiTrajectory>;
std::array<Acts::BoundIndices, 2> indices = {Acts::eBoundLoc0, Acts::eBoundLoc1};


CombinatorialKalmanFilterAlg::CombinatorialKalmanFilterAlg(
    const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator) {}


StatusCode CombinatorialKalmanFilterAlg::initialize() {
  ATH_CHECK(m_fieldCondObjInputKey.initialize());
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  ATH_CHECK(m_trackSeedTool.retrieve());
  //  ATH_CHECK(m_trackCollection.initialize());
  if (m_performanceWriter && !m_noDiagnostics) {
    ATH_CHECK(m_performanceWriterTool.retrieve());
  }
  if (m_statesWriter && !m_noDiagnostics) {
    ATH_CHECK(m_trajectoryStatesWriterTool.retrieve());
  }
  if (m_summaryWriter && !m_noDiagnostics) {
    ATH_CHECK(m_trajectorySummaryWriterTool.retrieve());
  }
  ATH_CHECK(detStore()->retrieve(m_idHelper,"FaserSCT_ID"));
  m_fit = makeTrackFinderFunction(m_trackingGeometryTool->trackingGeometry(),
                                  m_resolvePassive, m_resolveMaterial, m_resolveSensitive);
  if (m_actsLogging == "VERBOSE" && !m_noDiagnostics) {
    m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::VERBOSE);
  } else if (m_actsLogging == "DEBUG" && !m_noDiagnostics) {
    m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::DEBUG);
  } else {
    m_logger = Acts::getDefaultLogger("KalmanFitter", Acts::Logging::INFO);
  }
  return StatusCode::SUCCESS;
}


StatusCode CombinatorialKalmanFilterAlg::execute() {

  const EventContext& ctx = Gaudi::Hive::currentContext();
  m_numberOfEvents++;

  ATH_CHECK(m_trackCollection.initialize());
  SG::WriteHandle<TrackCollection> trackContainer{m_trackCollection,ctx};
  std::unique_ptr<TrackCollection> outputTracks = std::make_unique<TrackCollection>();

  std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry
      = m_trackingGeometryTool->trackingGeometry();

  const FaserActsGeometryContext& gctx = m_trackingGeometryTool->getGeometryContext();
  auto geoctx = gctx.context();
  Acts::MagneticFieldContext magFieldContext = getMagneticFieldContext(ctx);
  Acts::CalibrationContext calibContext;

  CHECK(m_trackSeedTool->run());
  std::shared_ptr<const Acts::Surface> initialSurface =
      m_trackSeedTool->initialSurface();
  std::shared_ptr<std::vector<Acts::CurvilinearTrackParameters>> initialParameters =
      m_trackSeedTool->initialTrackParameters();
  std::shared_ptr<std::vector<IndexSourceLink>> sourceLinks =
      m_trackSeedTool->sourceLinks();
  std::shared_ptr<IdentifierLink> idLinks = m_trackSeedTool->idLinks();
  std::shared_ptr<std::vector<Measurement>> measurements = m_trackSeedTool->measurements();
  std::shared_ptr<std::vector<const Tracker::FaserSCT_Cluster*>> clusters = m_trackSeedTool->clusters();
  std::shared_ptr<std::vector<std::array<std::vector<const Tracker::FaserSCT_Cluster*>, 3>>> seedClusters = m_trackSeedTool->seedClusters();

  TrajectoryInfo::nClusters = sourceLinks->size();

  TrajectoriesContainer trajectories;
  trajectories.reserve(initialParameters->size());

  Acts::PropagatorPlainOptions pOptions;
  pOptions.maxSteps = m_maxSteps;

  Acts::MeasurementSelector::Config measurementSelectorCfg = {
    {Acts::GeometryIdentifier(), {m_chi2Max, m_nMax}},
  };

  Acts::RotationMatrix3 rotation = Acts::RotationMatrix3::Identity();
  rotation.col(0) = Acts::Vector3(0, 0, -1);
  rotation.col(1) = Acts::Vector3(0, 1, 0);
  rotation.col(2) = Acts::Vector3(1, 0, 0);
  Acts::Translation3 trans(0., 0., 0.);
  Acts::Transform3 trafo(rotation * trans);
  initialSurface = Acts::Surface::makeShared<Acts::PerigeeSurface>(trafo);

  // Set the CombinatorialKalmanFilter options
  CombinatorialKalmanFilterAlg::TrackFinderOptions options(
      geoctx, magFieldContext, calibContext,
      IndexSourceLinkAccessor(), MeasurementCalibrator(*measurements),
      Acts::MeasurementSelector(measurementSelectorCfg),
      Acts::LoggerWrapper{*m_logger}, pOptions, &(*initialSurface));

  m_numberOfTrackSeeds += initialParameters->size();

  // Perform the track finding for all initial parameters
  ATH_MSG_DEBUG("Invoke track finding with " << initialParameters->size() << " seeds.");
  IndexSourceLinkContainer tmp;
  for (const auto& sl : *sourceLinks) {
    tmp.emplace_hint(tmp.end(), sl);
  }
  auto results = (*m_fit)(tmp, *initialParameters, options);

  // results contains a MultiTrajectory for each track seed with a trajectory of each branch of the CKF.
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

  // create Trk::Tracks from the trajectories
  for (const FaserActsRecMultiTrajectory &traj : selectedTrajectories) {
    std::unique_ptr<Trk::Track> track = makeTrack(geoctx, traj);
    if (track) {
      outputTracks->push_back(std::move(track));
    }
  }

  // run the performance writer
  if (m_statesWriter && !m_noDiagnostics) {
    ATH_CHECK(m_trajectoryStatesWriterTool->write(geoctx, trajectories, m_isMC));
  }
  if (m_summaryWriter && !m_noDiagnostics) {
    ATH_CHECK(m_trajectorySummaryWriterTool->write(geoctx, trajectories, m_isMC));
  }
  if  (m_performanceWriter && !m_noDiagnostics) {
    ATH_CHECK(m_performanceWriterTool->write(geoctx, trajectories));
  }
  ATH_CHECK(trackContainer.record(std::move(outputTracks)));

  return StatusCode::SUCCESS;
}


StatusCode CombinatorialKalmanFilterAlg::finalize() {
  ATH_MSG_INFO("CombinatorialKalmanFilterAlg::finalize()");
  ATH_MSG_INFO(m_numberOfEvents << " events processed.");
  ATH_MSG_INFO(m_numberOfTrackSeeds << " seeds.");
  ATH_MSG_INFO(m_numberOfFittedTracks << " fitted tracks.");
  ATH_MSG_INFO(m_numberOfSelectedTracks << " good fitted tracks.");
  return StatusCode::SUCCESS;
}


Acts::MagneticFieldContext CombinatorialKalmanFilterAlg::getMagneticFieldContext(const EventContext& ctx) const {
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
CombinatorialKalmanFilterAlg::makeTrack(const Acts::GeometryContext &geoCtx, const FaserActsRecMultiTrajectory &traj) const {
  using ConstTrackStateProxy =
      Acts::detail_lt::TrackStateProxy<IndexSourceLink, 6, true>;
  std::unique_ptr<Trk::Track> newtrack = nullptr;
  //Get the fit output object
  DataVector<const Trk::TrackStateOnSurface>* finalTrajectory = new DataVector<const Trk::TrackStateOnSurface>{};
  std::vector<std::unique_ptr<const Acts::BoundTrackParameters>> actsSmoothedParam;
  // Loop over all the output state to create track state
  traj.multiTrajectory().visitBackwards(traj.tips().front(), [&](const ConstTrackStateProxy& state) {
    auto flag = state.typeFlags();
    if (state.referenceSurface().associatedDetectorElement() != nullptr) {
      // We need to determine the type of state
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      const Trk::TrackParameters *parm;

      // State is a hole (no associated measurement), use predicted para meters
      if (flag[Acts::TrackStateFlag::HoleFlag] == true) {
        const Acts::BoundTrackParameters actsParam(state.referenceSurface().getSharedPtr(),
                                                   state.predicted(),
                                                   state.predictedCovariance());
        parm = ConvertActsTrackParameterToATLAS(actsParam, geoCtx);
        // auto boundaryCheck = m_boundaryCheckTool->boundaryCheck(*p arm);
        typePattern.set(Trk::TrackStateOnSurface::Hole);
      }
        // The state was tagged as an outlier, use filtered parameters
      else if (flag[Acts::TrackStateFlag::OutlierFlag] == true) {
        const Acts::BoundTrackParameters actsParam(state.referenceSurface().getSharedPtr(),
                                                   state.filtered(), state.filteredCovariance());
        parm = ConvertActsTrackParameterToATLAS(actsParam, geoCtx);
        typePattern.set(Trk::TrackStateOnSurface::Outlier);
      }
        // The state is a measurement state, use smoothed parameters
      else {
        const Acts::BoundTrackParameters actsParam(state.referenceSurface().getSharedPtr(),
                                                   state.smoothed(), state.smoothedCovariance());
        actsSmoothedParam.push_back(std::make_unique<const Acts::BoundTrackParameters>(Acts::BoundTrackParameters(actsParam)));
        //  const auto& psurface=actsParam.referenceSurface();
        Acts::Vector2 local(actsParam.parameters()[Acts::eBoundLoc0], actsParam.parameters()[Acts::eBoundLoc1]);
        //  const Acts::Vector3 dir = Acts::makeDirectionUnitFromPhiTheta(actsParam.parameters()[Acts::eBoundPhi], actsParam.parameters()[Acts::eBoundTheta]);
        //  auto pos=actsParam.position(tgContext);
        parm = ConvertActsTrackParameterToATLAS(actsParam, geoCtx);
        typePattern.set(Trk::TrackStateOnSurface::Measurement);
      }
      Tracker::FaserSCT_ClusterOnTrack* measState = nullptr;
      if (state.hasUncalibrated()) {
        const Tracker::FaserSCT_Cluster* fitCluster = state.uncalibrated().hit();
        if (fitCluster->detectorElement() != nullptr) {
          measState = new Tracker::FaserSCT_ClusterOnTrack{
              fitCluster,
              Trk::LocalParameters{
                  Trk::DefinedParameter{fitCluster->localPosition()[0], Trk::loc1},
                  Trk::DefinedParameter{fitCluster->localPosition()[1], Trk::loc2}
              },
              fitCluster->localCovariance(),
              m_idHelper->wafer_hash(fitCluster->detectorElement()->identify())
          };
        }
      }
      double nDoF = state.calibratedSize();
      const Trk::FitQualityOnSurface *quality = new Trk::FitQualityOnSurface(state.chi2(), nDoF);
      const Trk::TrackStateOnSurface *perState = new Trk::TrackStateOnSurface(measState, parm, quality, nullptr, typePattern);
      // If a state was succesfully created add it to the trajectory
      if (perState) {
        finalTrajectory->insert(finalTrajectory->begin(), perState);
      }
    }
    return;
  });

  // Create the track using the states
  const Trk::TrackInfo newInfo(Trk::TrackInfo::TrackFitter::KalmanFitter, Trk::ParticleHypothesis::muon);
  // Trk::FitQuality* q = nullptr;
  // newInfo.setTrackFitter(Trk::TrackInfo::TrackFitter::KalmanFitter     ); //Mark the fitter as KalmanFitter
  newtrack = std::make_unique<Trk::Track>(newInfo, std::move(*finalTrajectory), nullptr);
  return newtrack;
}

const Trk::TrackParameters*
CombinatorialKalmanFilterAlg::ConvertActsTrackParameterToATLAS(const Acts::BoundTrackParameters &actsParameter, const Acts::GeometryContext& gctx) const      {
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
  const Trk::CurvilinearParameters * curv = new Trk::CurvilinearParameters(pos,tmom,tqOverP>0, cov);
  return curv;
}

void CombinatorialKalmanFilterAlg::computeSharedHits(std::vector<IndexSourceLink>* sourceLinks, TrackFinderResult& results) const {
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
