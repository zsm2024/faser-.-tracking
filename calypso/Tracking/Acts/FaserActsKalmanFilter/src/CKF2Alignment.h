#ifndef FASERACTSKALMANFILTER_CKF2ALIGNMENT_H
#define FASERACTSKALMANFILTER_CKF2ALIGNMENT_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsExtrapolationTool.h"
#include "Acts/TrackFitting/KalmanFitter.hpp"
#include "Acts/TrackFinding/CombinatorialKalmanFilter.hpp"
#include "Acts/TrackFinding/MeasurementSelector.hpp"
#include "FaserActsKalmanFilter/Measurement.h"
#include "MagFieldConditions/FaserFieldCacheCondObj.h"
#include "TrkTrack/TrackCollection.h"
#include "FaserActsKalmanFilter/ITrackSeedTool.h"
#include "KalmanFitterTool.h"
#include <boost/dynamic_bitset.hpp>
#include "GaudiKernel/ITHistSvc.h"
#include "CreateTrkTrackTool.h"

using ConstTrackStateProxy = Acts::detail_lt::TrackStateProxy<IndexSourceLink, 6, true>;
using ClusterSet = boost::dynamic_bitset<>;

class TTree;
class FaserSCT_ID;


namespace TrackerDD {
  class SCT_DetectorManager;
}

class CKF2Alignment : public AthAlgorithm {
  public:
    CKF2Alignment(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~CKF2Alignment() = default;

    StatusCode initialize() override;
    StatusCode execute() override;
    StatusCode finalize() override;
    void initializeTree();
    void clearVariables();

    using TrackFinderOptions =
      Acts::CombinatorialKalmanFilterOptions<IndexSourceLinkAccessor,
      MeasurementCalibrator,
      Acts::MeasurementSelector>;
    using CKFResult = Acts::CombinatorialKalmanFilterResult<IndexSourceLink>;
    using TrackFitterResult = Acts::Result<CKFResult>;
    using TrackFinderResult = std::vector<TrackFitterResult>;

    using KFResult =
      Acts::Result<Acts::KalmanFitterResult<IndexSourceLink>>;

    using TrackFitterOptions =
      Acts::KalmanFitterOptions<MeasurementCalibrator, Acts::VoidOutlierFinder,
      Acts::VoidReverseFilteringLogic>;

    using TrackParameters = Acts::CurvilinearTrackParameters;
    using TrackParametersContainer = std::vector<TrackParameters>;

    // Track Finding
    class TrackFinderFunction {
      public:
	virtual ~TrackFinderFunction() = default;
	virtual TrackFinderResult operator()(const IndexSourceLinkContainer&,
	    const TrackParametersContainer&,
	    const TrackFinderOptions&) const = 0;
    };

    static std::shared_ptr<TrackFinderFunction> makeTrackFinderFunction(
	std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
	bool resolvePassive, bool resolveMaterial, bool resolveSensitive);

    // Track Fitting
    class TrackFitterFunction {
      public:
	virtual ~TrackFitterFunction() = default;
	virtual KFResult operator()(const std::vector<IndexSourceLink>&,
	    const Acts::BoundTrackParameters&,
	    const TrackFitterOptions&) const = 0;
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

    static std::shared_ptr<TrackFitterFunction> makeTrackFitterFunction(
	std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry);

    virtual Acts::MagneticFieldContext getMagneticFieldContext(const EventContext& ctx) const;

  private:
    int  m_numberOfEvents {0};
    int  m_numberOfTrackSeeds {0};
    int  m_numberOfFittedTracks {0};
    int  m_numberOfSelectedTracks {0};

    void computeSharedHits(std::vector<IndexSourceLink>* sourceLinks, TrackFinderResult& results) const;
    std::shared_ptr<TrackFinderFunction> m_fit;
    std::shared_ptr<TrackFitterFunction> m_kf;
    std::unique_ptr<const Acts::Logger> m_logger;
    const FaserSCT_ID* m_idHelper {nullptr};
    const TrackerDD::SCT_DetectorManager* m_detManager {nullptr};

    Gaudi::Property<bool> m_biased{this, "BiasedResidual", true};
    Gaudi::Property<std::string> m_actsLogging {this, "ActsLogging", "VERBOSE"};
    Gaudi::Property<int> m_minNumberMeasurements {this, "MinNumberMeasurements", 12};
    Gaudi::Property<bool> m_resolvePassive {this, "resolvePassive", false};
    Gaudi::Property<bool> m_resolveMaterial {this, "resolveMaterial", true};
    Gaudi::Property<bool> m_resolveSensitive {this, "resolveSensitive", true};
    Gaudi::Property<double> m_maxSteps {this, "maxSteps", 10000};
    Gaudi::Property<double> m_chi2Max {this, "chi2Max", 15};
    Gaudi::Property<unsigned long> m_nMax {this, "nMax", 10};
    SG::ReadCondHandleKey<FaserFieldCacheCondObj> m_fieldCondObjInputKey {this, "FaserFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
    ToolHandle<ITrackSeedTool> m_trackSeedTool {this, "TrackSeed", "ClusterTrackSeedTool"};
    ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
    ToolHandle<KalmanFitterTool> m_kalmanFitterTool1 {this, "KalmanFitterTool1", "KalmanFitterTool"};
    ToolHandle<CreateTrkTrackTool> m_createTrkTrackTool {this, "CreateTrkTrackTool", "CreateTrkTrackTool"};
    Gaudi::Property<bool> m_isMC {this, "isMC", false};
    ToolHandle<IFaserActsExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "FaserActsExtrapolationTool"};

    double getDerivation(const EventContext& ctx, Acts::BoundTrackParameters& fitParameters, Amg::Transform3D trans1, Amg::Vector2D loc_pos, int ia, int side , Amg::Transform3D trans2, int global)const ;
    double getLocalDerivation(Amg::Transform3D trans1, Amg::Vector2D loc_pos, int ia, int side , Amg::Transform3D trans2)const;
    //output ntuple
    ServiceHandle<ITHistSvc>  m_thistSvc;
    TTree* m_tree;
    mutable int m_eventId=0;
    mutable int m_trackId=0;
    mutable std::vector<double> m_fitParam_x;
    mutable std::vector<double> m_fitParam_charge;
    mutable std::vector<double> m_fitParam_y;
    mutable std::vector<double> m_fitParam_z;
    mutable std::vector<double> m_fitParam_px;
    mutable std::vector<double> m_fitParam_py;
    mutable std::vector<double> m_fitParam_pz;
    mutable std::vector<double> m_fitParam_chi2;
    mutable std::vector<int> m_fitParam_ndf;
    mutable std::vector<int> m_fitParam_nHoles;
    mutable std::vector<int> m_fitParam_nOutliers;
    mutable std::vector<int> m_fitParam_nStates;
    mutable std::vector<int> m_fitParam_nMeasurements;
    mutable std::vector<std::vector<double>> m_align_stationId_sp;
    mutable std::vector<std::vector<double>> m_align_centery_sp;
    mutable std::vector<std::vector<double>> m_align_layerId_sp;
    mutable std::vector<std::vector<double>> m_align_moduleId_sp;
    mutable std::vector<std::vector<double>> m_align_clustersize_sp;
    mutable std::vector<std::vector<double>> m_align_global_residual_y_sp;
    mutable std::vector<std::vector<double>> m_align_global_measured_x_sp;
    mutable std::vector<std::vector<double>> m_align_global_measured_y_sp;
    mutable std::vector<std::vector<double>> m_align_global_measured_z_sp;
    mutable std::vector<std::vector<double>> m_align_global_measured_ye_sp;
    mutable std::vector<std::vector<double>> m_align_global_fitted_ye_sp;
    mutable std::vector<std::vector<double>> m_align_global_fitted_y_sp;
    mutable std::vector<std::vector<double>> m_align_global_fitted_x_sp;
    mutable std::vector<std::vector<double>> m_align_local_residual_x_sp;
    mutable std::vector<std::vector<double>> m_align_unbiased_sp;
    mutable std::vector<std::vector<double>> m_align_local_pull_x_sp;
    mutable std::vector<std::vector<double>> m_align_local_measured_x_sp;
    mutable std::vector<std::vector<double>> m_align_local_measured_xe_sp;
    mutable std::vector<std::vector<double>> m_align_local_fitted_xe_sp;
    mutable std::vector<std::vector<double>> m_align_local_fitted_x_sp;
    mutable std::vector<std::vector<double>> m_align_derivation_x_x;
    mutable std::vector<std::vector<double>> m_align_derivation_x_y;
    mutable std::vector<std::vector<double>> m_align_derivation_x_z;
    mutable std::vector<std::vector<double>> m_align_derivation_x_rx;
    mutable std::vector<std::vector<double>> m_align_derivation_x_ry;
    mutable std::vector<std::vector<double>> m_align_derivation_x_rz;
    mutable std::vector<std::vector<double>> m_align_derivation_global_y_x;
    mutable std::vector<std::vector<double>> m_align_derivation_global_y_y;
    mutable std::vector<std::vector<double>> m_align_derivation_global_y_z;
    mutable std::vector<std::vector<double>> m_align_derivation_global_y_rx;
    mutable std::vector<std::vector<double>> m_align_derivation_global_y_ry;
    mutable std::vector<std::vector<double>> m_align_derivation_global_y_rz;
    mutable std::vector<std::vector<double>> m_align_stereoId;
};

#endif // FASERACTSKALMANFILTER_CKF2Alignment_H

