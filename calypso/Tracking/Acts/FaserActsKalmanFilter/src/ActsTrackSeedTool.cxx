#include "ActsTrackSeedTool.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "Identifier/Identifier.h"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "boost/container/small_vector.hpp"
#include "Acts/Seeding/EstimateTrackParamsFromSeed.hpp"

using namespace Acts::UnitLiterals;


ActsTrackSeedTool::ActsTrackSeedTool(
    const std::string& type, const std::string& name, const IInterface* parent)
    : base_class(type, name, parent) {}


StatusCode ActsTrackSeedTool::initialize() {
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_detManager, "SCT"));
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  ATH_CHECK(m_trackCollection.initialize());
  ATH_CHECK(m_clusterContainerKey.initialize());
  return StatusCode::SUCCESS;
}


StatusCode ActsTrackSeedTool::run(std::vector<int> /*maskedLayers*/, bool /*backward*/) {
  SG::ReadHandle<TrackCollection> trackCollection {m_trackCollection};
  ATH_CHECK(trackCollection.isValid());

  SG::ReadHandle<Tracker::FaserSCT_ClusterContainer> clusterContainer {m_clusterContainerKey};
  ATH_CHECK(clusterContainer.isValid());

  using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;
  std::shared_ptr<IdentifierMap> identifierMap = m_trackingGeometryTool->getIdentifierMap();
  const FaserActsGeometryContext& gctx = m_trackingGeometryTool->getGeometryContext();
  Acts::GeometryContext geoctx = gctx.context();

  const int kSize = 1;
  using ThisMeasurement = Acts::Measurement<Acts::BoundIndices, kSize>;
  std::array<Acts::BoundIndices, kSize> Indices = {Acts::eBoundLoc0};
  std::vector<IndexSourceLink> sourceLinks;
  std::vector<Measurement> measurements;
  std::map<Index, Identifier> identifierLinkMap;
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
      }
    }
  }

  std::map<int, std::vector<Amg::Vector3D>> station_position_map;
  for (const Trk::Track* track : *trackCollection) {
    auto momentum = track->trackParameters()->front()->momentum();
    ATH_MSG_DEBUG("track momentum: " << momentum.x() << ", " << momentum.y() << ", " << momentum.z());
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

  Acts::BoundSquareMatrix cov = Acts::BoundSquareMatrix::Zero();
  cov(Acts::eBoundLoc0, Acts::eBoundLoc0) = m_covLoc0;
  cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = m_covLoc1;
  cov(Acts::eBoundPhi, Acts::eBoundPhi) = m_covPhi;
  cov(Acts::eBoundTheta, Acts::eBoundTheta) = m_covTheta;
  cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = m_covQOverP;
  cov(Acts::eBoundTime, Acts::eBoundTime) = m_covTime;

  std::vector<Acts::BoundTrackParameters> initParams {};
  for (const Amg::Vector3D& pos1 : station_position_map[1]) {
    for (const Amg::Vector3D& pos2 : station_position_map[2]) {
      for (const Amg::Vector3D& pos3 : station_position_map[3]) {
        initParams.push_back(get_params(geoctx, pos1, pos2, pos3, cov, m_origin));
//        auto seed = initParams.back();
//        auto seed_momentum = seed.momentum();
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

  return StatusCode::SUCCESS;
}


StatusCode ActsTrackSeedTool::finalize() {
  return StatusCode::SUCCESS;
}


Acts::BoundTrackParameters ActsTrackSeedTool::get_params(
    const Acts::GeometryContext& gctx, const Amg::Vector3D& pos1,
    const Amg::Vector3D& pos2, const Amg::Vector3D& pos3,
    const Acts::BoundSquareMatrix& cov, double /*origin*/) {
  const auto surface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3 {0, 0, pos1.z()}, Acts::Vector3{0, 0, -1});
  struct SP{
   double x_;
   double y_;
   double z_;
   double t_;
   double x() const {return x_;} 
   double y() const {return y_;} 
   double z() const {return z_;} 
   double t() const {return t_;} 
  }; 
  SP sp1{pos1.x(), pos1.y(), pos1.z(), 0};
  SP sp2{pos2.x(), pos2.y(), pos2.z(), 0};
  SP sp3{pos3.x(), pos3.y(), pos3.z(), 0};
  std::array<const SP*, 3>  spacepoints = {&sp1, &sp2, &sp3};
  //boost::container::small_vector<const Amg::Vector3D*, 3> spacepoints {};
  //spacepoints.push_back(&pos1);
  //spacepoints.push_back(&pos2);
  //spacepoints.push_back(&pos3);
  //@todo: fix the ParticleHypothesis
  auto trackParams = Acts::BoundTrackParameters(surface, Acts::BoundVector::Zero(), cov,  Acts::ParticleHypothesis::muon());
  auto optParams = Acts::estimateTrackParamsFromSeed(
      gctx, spacepoints.begin(), spacepoints.end(), *surface, Acts::Vector3{0.57_T, 0, 0}, 0.1_T);
  if (not optParams.has_value()) {
    std::cout << "Estimation of track parameters failed." << std::endl;
  } else {
    const auto& params = optParams.value();
    
    //@todo: fix the ParticleHypothesis
    trackParams = Acts::BoundTrackParameters(surface, params, cov, Acts::ParticleHypothesis::muon());
  }
  return trackParams;
}
