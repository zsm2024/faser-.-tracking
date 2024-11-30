#ifndef FASERACTSKALMANFILTER_CLUSTERTRACKFINDERTOOL_H
#define FASERACTSKALMANFILTER_CLUSTERTRACKFINDERTOOL_H

#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "FaserActsKalmanFilter/ITrackSeedTool.h"
#include "TrkTrack/TrackCollection.h"
#include <memory>
#include <string>
#include <vector>

class FaserSCT_ID;
namespace TrackerDD { class SCT_DetectorManager; }

class ClusterTrackSeedTool : public extends<AthAlgTool, ITrackSeedTool> {
public:
  ClusterTrackSeedTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~ClusterTrackSeedTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode run() override;

  const std::shared_ptr<std::vector<Acts::CurvilinearTrackParameters>> initialTrackParameters() const override;
  const std::shared_ptr<const Acts::Surface> initialSurface() const override;
  const std::shared_ptr<std::vector<IndexSourceLink>> sourceLinks() const override;
  const std::shared_ptr<IdentifierLink> idLinks() const override;
  const std::shared_ptr<std::vector<Measurement>> measurements() const override;
  const std::shared_ptr<std::vector<const Tracker::FaserSCT_Cluster*>> clusters() const override;

private:
  std::shared_ptr<std::vector<Acts::CurvilinearTrackParameters>> m_initialTrackParameters;
  std::shared_ptr<const Acts::Surface> m_initialSurface;
  std::shared_ptr<std::vector<IndexSourceLink>> m_sourceLinks {};
  std::shared_ptr<IdentifierLink> m_idLinks {};
  std::shared_ptr<std::vector<Measurement>> m_measurements {};
  std::shared_ptr<std::vector<const Tracker::FaserSCT_Cluster*>> m_clusters {};

  const FaserSCT_ID* m_idHelper {nullptr};
  const TrackerDD::SCT_DetectorManager* m_detManager {nullptr};

  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {
      this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
  SG::ReadHandleKey<TrackCollection> m_trackCollection {
      this, "TrackCollection", "SegmentFit", "Input track collection name" };
  SG::ReadHandleKey<Tracker::FaserSCT_ClusterContainer> m_clusterContainerKey {
    this, "ClusterContainer", "SCT_ClusterContainer"};

  // position resolution of a cluster
  Gaudi::Property<double> m_std_cluster {this, "std_cluster", 0.04};

  // covariance of the initial parameters
  Gaudi::Property<double> m_covLoc0 {this, "covLoc0", 1};
  Gaudi::Property<double> m_covLoc1 {this, "covLoc1", 1};
  Gaudi::Property<double> m_covPhi {this, "covPhi", 1};
  Gaudi::Property<double> m_covTheta {this, "covTheta", 1};
  Gaudi::Property<double> m_covQOverP {this, "covQOverP", 1};
  Gaudi::Property<double> m_covTime {this, "covTime", 1};

  Gaudi::Property<double> m_origin {this, "origin", 0, "z position of the reference surface"};

  static Acts::CurvilinearTrackParameters get_params(const Amg::Vector3D& position_st1, const Amg::Vector3D& position_st2, const Acts::BoundSymMatrix& cov, double origin);
};

inline const std::shared_ptr<std::vector<Acts::CurvilinearTrackParameters>>
ClusterTrackSeedTool::initialTrackParameters() const {
  return m_initialTrackParameters;
}

inline const std::shared_ptr<const Acts::Surface>
ClusterTrackSeedTool::initialSurface() const {
  return m_initialSurface;
}

inline const std::shared_ptr<std::vector<IndexSourceLink>>
ClusterTrackSeedTool::sourceLinks() const {
  return m_sourceLinks;
}

inline const std::shared_ptr<IdentifierLink>
ClusterTrackSeedTool::idLinks() const {
  return m_idLinks;
}

inline const std::shared_ptr<std::vector<Measurement>>
ClusterTrackSeedTool::measurements() const {
  return m_measurements;
}

inline const std::shared_ptr<std::vector<const Tracker::FaserSCT_Cluster*>>
ClusterTrackSeedTool::clusters() const {
  return m_clusters;
}


#endif  // FASERACTSKALMANFILTER_CLUSTERTRACKFINDERTOOL_H
