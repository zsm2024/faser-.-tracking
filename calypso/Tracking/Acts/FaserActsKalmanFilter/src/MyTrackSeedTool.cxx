#include "MyTrackSeedTool.h"

#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointCollection.h"
#include "Identifier/Identifier.h"
#include "Acts/Geometry/GeometryIdentifier.hpp"


MyTrackSeedTool::MyTrackSeedTool( const std::string& type, const std::string& name, const IInterface* parent) :
    base_class(type, name, parent) {}


StatusCode MyTrackSeedTool::initialize() {
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_detManager, "SCT"));
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  ATH_CHECK(m_trackCollection.initialize());
  ATH_CHECK(m_clusterContainerKey.initialize());
  ATH_CHECK(m_spacePointContainerKey.initialize());
  return StatusCode::SUCCESS;
}


StatusCode MyTrackSeedTool::run(std::vector<int> /*maskedLayers*/, bool /*backward*/) {

  // create track seeds for multiple tracks
  SG::ReadHandle<TrackCollection> trackCollection {m_trackCollection};
  ATH_CHECK(trackCollection.isValid());

  SG::ReadHandle<Tracker::FaserSCT_ClusterContainer> clusterContainer {m_clusterContainerKey};
  ATH_CHECK(clusterContainer.isValid());

  SG::ReadHandle<FaserSCT_SpacePointContainer> spacePointContainer {m_spacePointContainerKey};
  ATH_CHECK(spacePointContainer.isValid());

  using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;
  std::shared_ptr<IdentifierMap> identifierMap = m_trackingGeometryTool->getIdentifierMap();

  std::map<Identifier, const Tracker::FaserSCT_SpacePoint*> spacePointMap {};
  for (const FaserSCT_SpacePointCollection* spacePointCollection : *spacePointContainer) {
    for (const Tracker::FaserSCT_SpacePoint *spacePoint: *spacePointCollection) {
      Identifier id1 = spacePoint->cluster1()->identify();
      spacePointMap[id1] = spacePoint;
      Identifier id2 = spacePoint->cluster2()->identify();
      spacePointMap[id2] = spacePoint;
    }
  }

  const int kSize = 1;
  using ThisMeasurement = Acts::Measurement<Acts::BoundIndices, kSize>;
  std::array<Acts::BoundIndices, kSize> Indices = {Acts::eBoundLoc0};
  std::vector<IndexSourceLink> sourceLinks;
  std::vector<Measurement> measurements;
  std::map<Index, Identifier> identifierLinkMap;
  std::vector<const Tracker::FaserSCT_SpacePoint*> spacePoints {};
  std::vector<const Tracker::FaserSCT_Cluster*> clusters {};
  for (const Tracker::FaserSCT_ClusterCollection* clusterCollection : *clusterContainer) {
    for (const Tracker::FaserSCT_Cluster* cluster : *clusterCollection) {
      Identifier id = cluster->detectorElement()->identify();
      identifierLinkMap[measurements.size()] = id;
      if (identifierMap->count(id) != 0) {
        Acts::GeometryIdentifier geoId = identifierMap->at(id);
        IndexSourceLink sourceLink(geoId, measurements.size(), cluster);
        // create measurement
        const auto& par = cluster->localPosition();
        Eigen::Matrix<double, 1, 1> pos {par.x(),};
        Eigen::Matrix<double, 1, 1> cov {m_std_cluster * m_std_cluster,};
        ThisMeasurement meas(Acts::SourceLink{sourceLink}, Indices, pos, cov);
        sourceLinks.push_back(sourceLink);
        measurements.emplace_back(std::move(meas));
        clusters.push_back(cluster);
        if (spacePointMap.count(cluster->identify()) > 0) {
          spacePoints.push_back(spacePointMap[cluster->identify()]);
        } else {
          spacePoints.push_back(nullptr);
        }
      }
    }
  }


  Acts::BoundSquareMatrix cov = Acts::BoundSquareMatrix::Zero();
  cov(Acts::eBoundLoc0, Acts::eBoundLoc0) = m_covLoc0;
  cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = m_covLoc1;
  cov(Acts::eBoundPhi, Acts::eBoundPhi) = m_covPhi;
  cov(Acts::eBoundTheta, Acts::eBoundTheta) = m_covTheta;
  cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = m_covQOverP;
  cov(Acts::eBoundTime, Acts::eBoundTime) = m_covTime;

  std::map<int, std::vector<Amg::Vector3D>> stationHitMap {};
  for (const Trk::Track* track : *trackCollection) {
    const Amg::Vector3D position = track->trackParameters()->front()->position();
    for (const Trk::TrackStateOnSurface* trackState : *(track->trackStateOnSurfaces())) {
      auto clusterOnTrack = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*> (trackState->measurementOnTrack());
      if (clusterOnTrack) {
        int station = m_idHelper->station(clusterOnTrack->identify());
        stationHitMap[station].push_back(position);
        break;
      }
    }
  }


  std::vector<Acts::BoundTrackParameters> initParams {};
  for (size_t i = 0; i < stationHitMap.size(); ++i) {
    for (size_t j = i+1; j < stationHitMap.size(); ++j) {
      for (const auto &p1 : stationHitMap[i]) {
        for (const auto &p2 : stationHitMap[j]) {
          initParams.push_back(get_params(p1, p2, cov, m_origin));
        }
      }
    }
  }

  m_initialTrackParameters = std::make_shared<std::vector<Acts::BoundTrackParameters>>(initParams);
  m_sourceLinks = std::make_shared<std::vector<IndexSourceLink>>(sourceLinks);
  m_idLinks = std::make_shared<IdentifierLink>(identifierLinkMap);
  m_measurements = std::make_shared<std::vector<Measurement>>(measurements);
  m_initialSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3 {0, 0, m_origin}, Acts::Vector3{0, 0, -1});
  m_clusters = std::make_shared<std::vector<const Tracker::FaserSCT_Cluster*>>(clusters);
  m_spacePoints = std::make_shared<std::vector<const Tracker::FaserSCT_SpacePoint*>>(spacePoints);
  // m_seedClusters = std::make_shared<std::vector<std::array<std::vector<const Tracker::FaserSCT_Cluster*>, 3>>>({});
  return StatusCode::SUCCESS;
}


StatusCode MyTrackSeedTool::finalize() {
  return StatusCode::SUCCESS;
}


Acts::BoundTrackParameters MyTrackSeedTool::get_params(const Amg::Vector3D& p1, const Amg::Vector3D& p2, const Acts::BoundSquareMatrix& cov, double origin) {
  Acts::Vector3 dir = p2 - p1;
  Acts::Vector3 pos = p1 - (p1.z() - origin)/dir.z() * dir;
  Acts::Vector4 pos4 {pos.x(), pos.y(), pos.z(), 0};
  
  const auto surface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3 {0, 0, pos.z()}, Acts::Vector3{0, 0, -1});

  Acts::BoundVector params = Acts::BoundVector::Zero();
  params[Acts::eBoundLoc0] = pos.x();
  params[Acts::eBoundLoc1] = pos.y();
  params[Acts::eBoundPhi] = Acts::VectorHelpers::phi(dir.normalized());
  params[Acts::eBoundTheta] = Acts::VectorHelpers::theta(dir.normalized());
  //@todo: this needs to be checked 
  params[Acts::eBoundQOverP] = 1./10000000;
  params[Acts::eBoundTime] = 0;

  //@todo: make the particle hypothesis configurable
  return Acts::BoundTrackParameters(surface, params, cov, Acts::ParticleHypothesis::muon());
}
