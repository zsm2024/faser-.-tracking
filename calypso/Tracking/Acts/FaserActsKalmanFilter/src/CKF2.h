#ifndef FASERACTSKALMANFILTER_CKF2_H
#define FASERACTSKALMANFILTER_CKF2_H

#include "TrackFitterFunction.h"
#include "TrackFinderFunction.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "Acts/TrackFitting/KalmanFitter.hpp"
#include "Acts/TrackFinding/CombinatorialKalmanFilter.hpp"
#include "Acts/TrackFinding/MeasurementSelector.hpp"
#include "FaserActsKalmanFilter/Measurement.h"
#include "MagFieldConditions/FaserFieldCacheCondObj.h"
#include "TrajectoryWriterTool.h"
#include "TrkTrack/TrackCollection.h"
#include "FaserActsKalmanFilter/ITrackSeedTool.h"
#include "RootSeedWriteTool.h"
#include "RootTrajectoryStatesWriterTool.h"
#include "RootTrajectorySummaryWriterTool.h"
#include "PerformanceWriterTool.h"
#include "KalmanFitterTool.h"
#include <boost/dynamic_bitset.hpp>
#include "CreateTrkTrackTool.h"
#include "xAODEventInfo/EventInfo.h"


using ClusterSet = boost::dynamic_bitset<>;

class FaserSCT_ID;

namespace Trk {
class TrackStateOnSurface;
}

namespace TrackerDD {
class SCT_DetectorManager;
}

class CKF2 : public AthAlgorithm {
public:
  CKF2(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CKF2() = default;

  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

  struct TrackInfo {
    TrackInfo(const FaserActsTrackContainer::TrackProxy& trk) :
      clusterSet{nClusters} {
      
      nMeasurements = trk.nMeasurements();
      chi2 = trk.chi2();
      //@todo: Is this unique?
      index = trk.index();
      for (const auto& state : trk.trackStatesReversed()) {
        auto typeFlags = state.typeFlags();
        if (not typeFlags.test(Acts::TrackStateFlag::MeasurementFlag)) {
           continue; 
        }
        auto sl = state.getUncalibratedSourceLink().template get<IndexSourceLink>();
        clusterSet.set(sl.index());
      }
    }

    static size_t nClusters;
    ClusterSet clusterSet;
    size_t nMeasurements;
    double chi2;
    size_t index;
  };


  virtual Acts::MagneticFieldContext getMagneticFieldContext(const EventContext& ctx) const;

private:
  size_t m_numberOfEvents {0};
  size_t m_numberOfTrackSeeds {0};
  size_t m_numberOfFittedTracks {0};
  size_t m_numberOfSelectedTracks {0};

  void computeSharedHits(std::vector<IndexSourceLink>* sourceLinks, FaserActsTrackContainer& tracks) const;
  std::shared_ptr<TrackFinderFunction> m_finder;
  std::shared_ptr<TrackFitterFunction> m_fitter;
  std::unique_ptr<const Acts::Logger> m_logger;
  const FaserSCT_ID* m_idHelper {nullptr};
  const TrackerDD::SCT_DetectorManager* m_detManager {nullptr};

  Gaudi::Property<std::string> m_actsLogging {this, "ActsLogging", "VERBOSE"};
  Gaudi::Property<int> m_minNumberMeasurements {this, "MinNumberMeasurements", 12};
  Gaudi::Property<bool> m_backwardPropagation {this, "BackwardPropagation", false};
  Gaudi::Property<bool> m_performanceWriter {this, "PerformanceWriter", true};
  Gaudi::Property<bool> m_summaryWriter {this, "SummaryWriter", true};
  Gaudi::Property<bool> m_noDiagnostics {this, "noDiagnostics", true, "Set ACTS logging level to INFO and do not run performance writer, states writer or summary writer"};
  Gaudi::Property<bool> m_statesWriter {this, "StatesWriter", false};
  //own modify**************************************************************************
  Gaudi::Property<bool> m_seedWriter {this, "seedWriter", true};
  Gaudi::Property<bool> m_resolvePassive {this, "resolvePassive", false};
  Gaudi::Property<bool> m_resolveMaterial {this, "resolveMaterial", true};
  Gaudi::Property<bool> m_resolveSensitive {this, "resolveSensitive", true};
  Gaudi::Property<bool> m_addFittedParamsToTrack {this, "addFittedParamsToTrack", true};
  Gaudi::Property<double> m_maxSteps {this, "maxSteps", 10000};
  Gaudi::Property<double> m_chi2Max {this, "chi2Max", 15};
  Gaudi::Property<unsigned long> m_nMax {this, "nMax", 10};
  Gaudi::Property<std::vector<int>> m_maskedLayers {this, "maskedLayers", {}};
  SG::ReadCondHandleKey<FaserFieldCacheCondObj> m_fieldCondObjInputKey {this, "FaserFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
  ToolHandle<ITrackSeedTool> m_trackSeedTool {this, "TrackSeed", "ClusterTrackSeedTool"};
  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
//todo  ToolHandle<PerformanceWriterTool> m_performanceWriterTool {this, "PerformanceWriterTool", "PerformanceWriterTool"};
  ToolHandle<RootTrajectoryStatesWriterTool> m_trajectoryStatesWriterTool {this, "RootTrajectoryStatesWriterTool", "RootTrajectoryStatesWriterTool"};
  ToolHandle<RootTrajectorySummaryWriterTool> m_trajectorySummaryWriterTool {this, "RootTrajectorySummaryWriterTool", "RootTrajectorySummaryWriterTool"};
  //ToolHandle<CircleFitTrackSeedTool> m_selectedSeedTool {this, "selectedSeed", "CircleFitTrackSeedTool"};
  ToolHandle<RootSeedWriterTool> m_seedWriterTool {this, "RootSeedWriterTool", "RootSeedWriterTool"};
  ToolHandle<KalmanFitterTool> m_kalmanFitterTool1 {this, "KalmanFitterTool1", "KalmanFitterTool"};
  ToolHandle<CreateTrkTrackTool> m_createTrkTrackTool {this, "CreateTrkTrackTool", "CreateTrkTrackTool"};
  Gaudi::Property<bool> m_isMC {this, "isMC", false};
  SG::WriteHandleKey<TrackCollection> m_trackCollection { this, "OutputCollection", "CKFTrackCollection", "Output track collection name" };
  // SG::WriteHandleKey<TrackCollection> m_allTrackCollection { this, "AllTrackCollection", "CKFAllTrackCollection", "Output all track collection name" };
  SG::WriteDecorHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfoKey", "EventInfo"};
};

#endif // FASERACTSKALMANFILTER_CKF2_H
