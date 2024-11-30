#ifndef COMBINATORIALKALMANFILTERALG_H
#define COMBINATORIALKALMANFILTERALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
//#include "SPSimpleInitialParameterTool.h"
//#include "SPSeedBasedInitialParameterTool.h"
//todo#include "Acts/TrackFinding/CombinatorialKalmanFilter.hpp"
#include "Acts/TrackFinding/MeasurementSelector.hpp"
#include "FaserActsKalmanFilter/Measurement.h"
#include "MagFieldConditions/FaserFieldCacheCondObj.h"
#include "TrajectoryWriterTool.h"
#include "TrkTrack/TrackCollection.h"
#include "FaserActsKalmanFilter/ITrackSeedTool.h"
#include "RootTrajectoryStatesWriterTool.h"
#include "RootTrajectorySummaryWriterTool.h"
//#todo#include "PerformanceWriterTool.h"
#include "FaserActsTrack.h"
#include <boost/dynamic_bitset.hpp>
using ConstTrackStateProxy = Acts::detail_lt::TrackStateProxy<IndexSourceLink, 6, true>;
using ClusterSet = boost::dynamic_bitset<>;


class FaserSCT_ID;

namespace Trk {
class TrackStateOnSurface;
}

namespace TrackerDD {
class SCT_DetectorManager;
} 

class CombinatorialKalmanFilterAlg : public AthAlgorithm {
public:
  CombinatorialKalmanFilterAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CombinatorialKalmanFilterAlg() = default;

  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

  using TrackFinderOptions =
      Acts::CombinatorialKalmanFilterOptions<IndexSourceLinkAccessor,
                                             MeasurementCalibrator,
                                             Acts::MeasurementSelector>;
  using CKFResult = Acts::CombinatorialKalmanFilterResult<IndexSourceLink>;
  using TrackFitterResult = Acts::Result<CKFResult>;
  using TrackFinderResult = std::vector<TrackFitterResult>;
  using TrackParameters = Acts::CurvilinearTrackParameters;
  using TrackParametersContainer = std::vector<TrackParameters>;

  class TrackFinderFunction {
  public:
    virtual ~TrackFinderFunction() = default;
    virtual TrackFinderResult operator()(const IndexSourceLinkContainer&,
                                         const TrackParametersContainer&,
                                         const TrackFinderOptions&) const = 0;
  };

  struct TrajectoryInfo {
    TrajectoryInfo(const FaserActsRecMultiTrajectory &traj) :
        trajectory{traj}, clusterSet{nClusters} {
      auto state = Acts::MultiTrajectoryHelpers::trajectoryState(traj.multiTrajectory(), traj.tips().front());
      traj.multiTrajectory().visitBackwards(traj.tips().front(), [&](const ConstTrackStateProxy& state) {
        auto typeFlags = state.typeFlags();
        if (not typeFlags.test(Acts::TrackStateFlag::MeasurementFlag)) {
          return true;
        }
        clusterSet.set(state.uncalibrated().index());
        return true;
      });
      nMeasurements = state.nMeasurements;
      chi2 = state.chi2Sum;
    }

    static size_t nClusters;
    FaserActsRecMultiTrajectory trajectory;
    ClusterSet clusterSet;
    size_t nMeasurements;
    double chi2;
  };

  static std::shared_ptr<TrackFinderFunction> makeTrackFinderFunction(
      std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
      bool resolvePassive, bool resolveMaterial, bool resolveSensitive);
  virtual Acts::MagneticFieldContext getMagneticFieldContext(const EventContext& ctx) const;

 private:
  size_t m_numberOfEvents {0};
  size_t m_numberOfTrackSeeds {0};
  size_t m_numberOfFittedTracks {0};
  size_t m_numberOfSelectedTracks {0};

  void computeSharedHits(std::vector<IndexSourceLink>* sourceLinks, TrackFinderResult& results) const;
  std::shared_ptr<TrackFinderFunction> m_fit;
  std::unique_ptr<const Acts::Logger> m_logger;
  const FaserSCT_ID* m_idHelper {nullptr};
  const TrackerDD::SCT_DetectorManager* m_detManager {nullptr};

  Gaudi::Property<std::string> m_actsLogging {this, "ActsLogging", "VERBOSE"};
  Gaudi::Property<int> m_minNumberMeasurements {this, "MinNumberMeasurements", 12};
  Gaudi::Property<bool> m_backwardPropagation {this, "BackwardPropagation", false};
  Gaudi::Property<bool> m_performanceWriter {this, "PerformanceWriter", true};
  Gaudi::Property<bool> m_summaryWriter {this, "SummaryWriter", true};
  Gaudi::Property<bool> m_statesWriter {this, "StatesWriter", false};
  Gaudi::Property<bool> m_resolvePassive {this, "resolvePassive", false};
  Gaudi::Property<bool> m_resolveMaterial {this, "resolveMaterial", true};
  Gaudi::Property<bool> m_resolveSensitive {this, "resolveSensitive", true};
  Gaudi::Property<bool> m_noDiagnostics {this, "noDiagnostics", true, "Set ACTS logging level to INFO and do not run performance writer, states writer or summary writer"};
  Gaudi::Property<double> m_maxSteps {this, "maxSteps", 1000};
  Gaudi::Property<double> m_chi2Max {this, "chi2Max", 15};
  Gaudi::Property<unsigned long> m_nMax {this, "nMax", 10};
  Gaudi::Property<bool> m_isMC {this, "isMC", false};

  SG::ReadCondHandleKey<FaserFieldCacheCondObj> m_fieldCondObjInputKey {this, "FaserFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
  ToolHandle<ITrackSeedTool> m_trackSeedTool {this, "TrackSeed", "ClusterTrackSeedTool"};
  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
  ToolHandle<PerformanceWriterTool> m_performanceWriterTool {this, "PerformanceWriterTool", "PerformanceWriterTool"};
  ToolHandle<RootTrajectoryStatesWriterTool> m_trajectoryStatesWriterTool {this, "RootTrajectoryStatesWriterTool", "RootTrajectoryStatesWriterTool"};
  ToolHandle<RootTrajectorySummaryWriterTool> m_trajectorySummaryWriterTool {this, "RootTrajectorySummaryWriterTool", "RootTrajectorySummaryWriterTool"};

  std::unique_ptr<Trk::Track> makeTrack(Acts::GeometryContext& tgContext, TrackFitterResult& fitResult) const;
  std::unique_ptr<Trk::Track> makeTrack(const Acts::GeometryContext &geoCtx, const FaserActsRecMultiTrajectory &traj) const;
  const Trk::TrackParameters* ConvertActsTrackParameterToATLAS(const Acts::BoundTrackParameters &actsParameter, const Acts::GeometryContext& gctx) const;
  SG::WriteHandleKey<TrackCollection> m_trackCollection { this, "CKFTrackCollection", "CKFTrackCollection" };
};

#endif // COMBINATORIALKALMANFILTERALG_H

