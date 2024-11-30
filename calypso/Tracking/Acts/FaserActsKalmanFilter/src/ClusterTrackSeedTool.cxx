#include "ClusterTrackSeedTool.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "Identifier/Identifier.h"
#include "Acts/Geometry/GeometryIdentifier.hpp"


// FIXME make this a method of ClusterTrackSeedTool
std::pair<double, double> momentum(const std::map<int, Amg::Vector3D>& pos, double B=0.57) {
  Acts::Vector3 vec_l = pos.at(3) - pos.at(1);
  double abs_l = std::sqrt(vec_l.y() * vec_l.y() + vec_l.z() * vec_l.z());
  double t = (pos.at(2).z() - pos.at(1).z()) / (pos.at(3).z() - pos.at(1).z());
  Acts::Vector3 vec_m = pos.at(1) + t * vec_l;
  Acts::Vector3 vec_s = pos.at(2) - vec_m;
  double abs_s = std::sqrt(vec_s.y() * vec_s.y() + vec_s.z() * vec_s.z());
  double p_yz = 0.3 * abs_l * abs_l * B / (8 * abs_s * 1000);
  double charge = vec_s.y() < 0 ? 1 : -1;
  return std::make_pair(p_yz, charge);
}


ClusterTrackSeedTool::ClusterTrackSeedTool(
    const std::string& type, const std::string& name, const IInterface* parent)
    : base_class(type, name, parent) {}


StatusCode ClusterTrackSeedTool::initialize() {
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_detManager, "SCT"));
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  ATH_CHECK(m_trackCollection.initialize());
  ATH_CHECK(m_clusterContainerKey.initialize());
  return StatusCode::SUCCESS;
}


StatusCode ClusterTrackSeedTool::run() {
  // create track seeds for multiple tracks
  SG::ReadHandle<TrackCollection> trackCollection {m_trackCollection};
  ATH_CHECK(trackCollection.isValid());

  SG::ReadHandle<Tracker::FaserSCT_ClusterContainer> clusterContainer {m_clusterContainerKey};
  ATH_CHECK(clusterContainer.isValid());

  using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;
  std::shared_ptr<IdentifierMap> identifierMap = m_trackingGeometryTool->getIdentifierMap();

  const int kSize = 1;
  using ThisMeasurement = Acts::Measurement<IndexSourceLink, Acts::BoundIndices, kSize>;
  std::array<Acts::BoundIndices, kSize> Indices = {Acts::eBoundLoc0};
  std::vector<IndexSourceLink> sourceLinks;
  std::vector<Measurement> measurements;
  std::map<Index, Identifier> identifierLinkMap;
  std::vector<const Tracker::FaserSCT_Cluster*> clusters {};
  for (const Tracker::FaserSCT_ClusterCollection* clusterCollection : *clusterContainer) {
    for (const Tracker::FaserSCT_Cluster* cluster : *clusterCollection) {
      Identifier id = cluster->detectorElement()->identify();
      identifierLinkMap[measurements.size()] = id;
      Acts::GeometryIdentifier geoId = identifierMap->at(id);
      IndexSourceLink sourceLink(geoId, measurements.size());
      // create measurement
      const auto& par = cluster->localPosition();
      Eigen::Matrix<double, 1, 1> pos {par.x(),};
      Eigen::Matrix<double, 1, 1> cov {m_std_cluster * m_std_cluster,};
      ThisMeasurement meas(sourceLink, Indices, pos, cov);
      sourceLinks.push_back(sourceLink);
      measurements.emplace_back(std::move(meas));
      clusters.push_back(cluster);
    }
  }


  std::map<int, std::vector<Amg::Vector3D>> station_position_map;
  for (const Trk::Track* track : *trackCollection) {
    for (const Trk::TrackStateOnSurface* trackState : *(track->trackStateOnSurfaces())) {
      auto clusterOnTrack = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*> (trackState->measurementOnTrack());
      if (clusterOnTrack) {
        Identifier id = clusterOnTrack->identify();
        int station = m_idHelper->station(id);
        auto fitParameters = track->trackParameters()->front();
        Amg::Vector3D fitPosition = fitParameters->position();
        station_position_map[station].push_back(fitPosition);
        break;
      }
    }
  }

  Acts::BoundSymMatrix cov = Acts::BoundSymMatrix::Zero();
  cov(Acts::eBoundLoc0, Acts::eBoundLoc0) = m_covLoc0;
  cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = m_covLoc1;
  cov(Acts::eBoundPhi, Acts::eBoundPhi) = m_covPhi;
  cov(Acts::eBoundTheta, Acts::eBoundTheta) = m_covTheta;
  cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = m_covQOverP;
  cov(Acts::eBoundTime, Acts::eBoundTime) = m_covTime;

  std::vector<Acts::CurvilinearTrackParameters> initParams {};
  for (const Amg::Vector3D& pos1 : station_position_map[1]) {
    for (const Amg::Vector3D& pos2 : station_position_map[2]) {
      initParams.push_back(get_params(pos1, pos2, cov, m_origin));
    }
  }

  m_initialTrackParameters = std::make_shared<std::vector<Acts::CurvilinearTrackParameters>>(initParams);
  m_sourceLinks = std::make_shared<std::vector<IndexSourceLink>>(sourceLinks);
  m_idLinks = std::make_shared<IdentifierLink>(identifierLinkMap);
  m_measurements = std::make_shared<std::vector<Measurement>>(measurements);
  m_initialSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3 {0, 0, m_origin}, Acts::Vector3{0, 0, 1});
  m_clusters = std::make_shared<std::vector<const Tracker::FaserSCT_Cluster*>>(clusters);

  return StatusCode::SUCCESS;
}


StatusCode ClusterTrackSeedTool::finalize() {
  return StatusCode::SUCCESS;
}


Acts::CurvilinearTrackParameters ClusterTrackSeedTool::get_params(
    const Amg::Vector3D& position_st1, const Amg::Vector3D& position_st2, const Acts::BoundSymMatrix& cov, double origin) {
  Acts::Vector3 dir = position_st2 - position_st1;
  Acts::Vector3 pos = position_st1 - (position_st1.z() - origin)/dir.z() * dir;
  Acts::Vector4 pos4 {pos.x(), pos.y(), pos.z(), 0};
  return Acts::CurvilinearTrackParameters(pos4, dir, 100000000, 1, cov);
}
