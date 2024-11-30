#ifndef FASERACTSKALMANFILTER_THREESTATIONTRACKSEEDTOOL_H
#define FASERACTSKALMANFILTER_THREESTATIONTRACKSEEDTOOL_H

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

class ThreeStationTrackSeedTool : public extends<AthAlgTool, ITrackSeedTool> {
public:
  ThreeStationTrackSeedTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~ThreeStationTrackSeedTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode run(std::vector<int> /*maskedLayers*/, bool /*backward*/) override;

  const std::shared_ptr<std::vector<Acts::BoundTrackParameters>> initialTrackParameters() const override;
  const std::shared_ptr<const Acts::Surface> initialSurface() const override;
  const std::shared_ptr<std::vector<IndexSourceLink>> sourceLinks() const override;
  const std::shared_ptr<IdentifierLink> idLinks() const override;
  const std::shared_ptr<std::vector<Measurement>> measurements() const override;
  const std::shared_ptr<std::vector<const Tracker::FaserSCT_Cluster*>> clusters() const override;
  const std::shared_ptr<std::vector<std::array<std::vector<const Tracker::FaserSCT_Cluster*>, 3>>> seedClusters() const override;
  const std::shared_ptr<std::vector<const Tracker::FaserSCT_SpacePoint*>> spacePoints() const override;
  double targetZPosition() const override;

private:

  struct Tracklet {
   public:
    Tracklet(const Amg::Vector3D& position, const std::vector<const Tracker::FaserSCT_Cluster*>& clusters)
      : m_position(position), m_clusters(clusters) {}
    inline const Amg::Vector3D position() const { return m_position; }
    inline const std::vector<const Tracker::FaserSCT_Cluster*> clusters() const { return m_clusters; }
    inline int size() const { return m_clusters.size(); }
  private:
    const Amg::Vector3D m_position;
    const std::vector<const Tracker::FaserSCT_Cluster*> m_clusters;
  };

  std::shared_ptr<std::vector<Acts::BoundTrackParameters>> m_initialTrackParameters;
  std::shared_ptr<const Acts::Surface> m_initialSurface;
  std::shared_ptr<std::vector<IndexSourceLink>> m_sourceLinks {};
  std::shared_ptr<IdentifierLink> m_idLinks {};
  std::shared_ptr<std::vector<Measurement>> m_measurements {};
  std::shared_ptr<std::vector<const Tracker::FaserSCT_Cluster*>> m_clusters {};
  std::shared_ptr<std::vector<std::array<std::vector<const Tracker::FaserSCT_Cluster*>, 3>>> m_seedClusters {};
  std::shared_ptr<std::vector<const Tracker::FaserSCT_SpacePoint*>> m_spacePoints {};
  double m_targetZPosition {0.};

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

  static Acts::BoundTrackParameters get_params(
      const Amg::Vector3D& position_st1, const Amg::Vector3D& position_st2, const Amg::Vector3D& position_st3, const Acts::BoundSquareMatrix& cov, double origin);
  static std::pair<double, double> momentum(const std::map<int, Amg::Vector3D>& pos, double B=0.57);
};

inline const std::shared_ptr<std::vector<Acts::BoundTrackParameters>>
ThreeStationTrackSeedTool::initialTrackParameters() const {
  return m_initialTrackParameters;
}

inline const std::shared_ptr<const Acts::Surface>
ThreeStationTrackSeedTool::initialSurface() const {
  return m_initialSurface;
}

inline const std::shared_ptr<std::vector<IndexSourceLink>>
ThreeStationTrackSeedTool::sourceLinks() const {
  return m_sourceLinks;
}

inline const std::shared_ptr<IdentifierLink>
ThreeStationTrackSeedTool::idLinks() const {
  return m_idLinks;
}

inline const std::shared_ptr<std::vector<Measurement>>
ThreeStationTrackSeedTool::measurements() const {
  return m_measurements;
}

inline const std::shared_ptr<std::vector<const Tracker::FaserSCT_Cluster*>>
ThreeStationTrackSeedTool::clusters() const {
  return m_clusters;
}

inline const std::shared_ptr<std::vector<std::array<std::vector<const Tracker::FaserSCT_Cluster*>, 3>>>
ThreeStationTrackSeedTool::seedClusters() const {
  return m_seedClusters;
}

inline const std::shared_ptr<std::vector<const Tracker::FaserSCT_SpacePoint*>>
ThreeStationTrackSeedTool::spacePoints() const {
  return m_spacePoints;
}

inline double ThreeStationTrackSeedTool::targetZPosition() const {
  return m_targetZPosition;
}

#endif  // FASERACTSKALMANFILTER_THREESTATIONTRACKSEEDTOOL_H
