#ifndef FASERACTSKALMANFILTER_KALMANFITTERTOOL_H
#define FASERACTSKALMANFILTER_KALMANFITTERTOOL_H

#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/TrackFitting/KalmanFitter.hpp"
#include "Acts/EventData/MeasurementHelpers.hpp"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "FaserActsKalmanFilter/IndexSourceLink.h"
#include "FaserActsKalmanFilter/Measurement.h"
#include "FaserActsRecMultiTrajectory.h"
#include "MagFieldConditions/FaserFieldCacheCondObj.h"
#include "RootTrajectoryStatesWriterTool.h"
#include "RootTrajectorySummaryWriterTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "CreateTrkTrackTool.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "Identifier/Identifier.h"
#include "FaserActsTrack.h"
#include "FaserActsGeometry/FASERMagneticFieldWrapper.h"

#include "TrackFitterFunction.h"

struct TSOS4Residual{
  double fit_local_x;
  double fit_local_y;
  double fit_global_x;
  double fit_global_y;
  double fit_global_z;
  const Tracker::FaserSCT_Cluster* cluster;
  double residual;
	Acts::BoundTrackParameters parameter;

};

class FaserSCT_ID;



class KalmanFitterTool : virtual public AthAlgTool {
public:
  KalmanFitterTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~KalmanFitterTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual Acts::MagneticFieldContext getMagneticFieldContext(const EventContext& ctx) const;

  std::unique_ptr<Trk::Track> fit(const EventContext &ctx, const Acts::GeometryContext &gctx,
                                  Trk::Track *inputTrack,
                                  const Acts::BoundVector& inputVector = Acts::BoundVector::Zero(),
                                  bool isMC=false) const;

  std::vector<TSOS4Residual> getUnbiasedResidual(const EventContext &ctx, const Acts::GeometryContext &gctx,
                                  Trk::Track *inputTrack,
                                  const Acts::BoundVector& inputVector = Acts::BoundVector::Zero(),
                                  bool isMC=false, double origin=0) const;

  std::vector<TSOS4Residual> getUnbiasedResidual(const EventContext &ctx, const Acts::GeometryContext &gctx,
                                  Trk::Track *inputTrack, double clusz,
                                  const Acts::BoundVector& inputVector ,
                                  bool isMC, double origin) const;

  std::vector<TSOS4Residual> getUnbiasedResidual(const EventContext &ctx, const Acts::GeometryContext &gctx,
                                  Trk::Track *inputTrack,
                                  const Acts::BoundVector& inputVector ,
                                  bool isMC, double origin, std::vector<const Tracker::FaserSCT_Cluster*>& clus,const Acts::BoundTrackParameters ini_Param) const;

private:
  const FaserSCT_ID* m_idHelper {nullptr};
  std::tuple<std::vector<IndexSourceLink>, std::vector<Measurement>>
  getMeasurementsFromTrack(Trk::Track *track) const;
  std::tuple<std::vector<IndexSourceLink>, std::vector<Measurement>>
  getMeasurementsFromTrack(Trk::Track *track, Identifier& wafer_id) const;
  std::tuple<std::vector<IndexSourceLink>, std::vector<Measurement>>
  getMeasurementsFromTrack(Trk::Track *track, std::vector<const Tracker::FaserSCT_Cluster*>& clusters) const;
  Acts::BoundTrackParameters getParametersFromTrack(const Trk::TrackParameters *inputParameters, const Acts::BoundVector& inputVector, double origin) const;
  std::shared_ptr<TrackFitterFunction> m_fit;
  std::unique_ptr<const Acts::Logger> m_logger;
  Gaudi::Property<std::string> m_actsLogging {this, "ActsLogging", "VERBOSE"};
  Gaudi::Property<std::size_t> m_minMeasurements {this, "MinMeasurements", 12, "minimum number of measurements of the input track"};
  Gaudi::Property<double> m_seedCovarianceScale {this, "SeedCovarianceScale", 100, "scale covariance from initial track"};
  Gaudi::Property<bool> m_isMC {this, "isMC", false};
  Gaudi::Property<bool> m_summaryWriter {this, "SummaryWriter", false};
  Gaudi::Property<bool> m_statesWriter {this, "StatesWriter", false};
  Gaudi::Property<bool> m_noDiagnostics {this, "noDiagnostics", true, "Set ACTS logging level to INFO and do not run performance writer, states writer or summary writer"};

  SG::ReadCondHandleKey<FaserFieldCacheCondObj> m_fieldCondObjInputKey {this, "FaserFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
  ToolHandle<RootTrajectoryStatesWriterTool> m_trajectoryStatesWriterTool {this, "RootTrajectoryStatesWriterTool", "RootTrajectoryStatesWriterTool"};
  ToolHandle<RootTrajectorySummaryWriterTool> m_trajectorySummaryWriterTool {this, "RootTrajectorySummaryWriterTool", "RootTrajectorySummaryWriterTool"};
  ToolHandle<CreateTrkTrackTool> m_createTrkTrackTool {this, "CreateTrkTrackTool", "CreateTrkTrackTool"};

//  Acts::KalmanFitterExtensions<Acts::VectorMultiTrajectory> getExtensions();
  
//    FaserActsOutlierFinder  m_outlierFinder{0};
//  ReverseFilteringLogic m_reverseFilteringLogic{0};
//  Acts::KalmanFitterExtensions<Acts::VectorMultiTrajectory> m_kfExtensions;
};

#endif //FASERACTSKALMANFILTER_KALMANFITTERTOOL_H
