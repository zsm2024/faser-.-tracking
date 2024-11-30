#include "SegmentFitClusterTrackFinderTool.h"

#include "Acts/Definitions/TrackParametrization.hpp"
#include "Acts/EventData/Measurement.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "FaserActsKalmanFilter/IndexSourceLink.h"
#include "Identifier/Identifier.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
//#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include <random>


SegmentFitClusterTrackFinderTool::SegmentFitClusterTrackFinderTool(const std::string& type, const std::string& name, const IInterface* parent)
    : base_class(type, name, parent) {}


StatusCode SegmentFitClusterTrackFinderTool::initialize() {
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_detManager, "SCT"));
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  ATH_CHECK(m_trackCollection.initialize());
  return StatusCode::SUCCESS;
}


StatusCode SegmentFitClusterTrackFinderTool::run() {
  SG::ReadHandle<TrackCollection> trackCollection {m_trackCollection};
  ATH_CHECK(trackCollection.isValid());

  using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;
  std::shared_ptr<IdentifierMap> identifierMap = m_trackingGeometryTool->getIdentifierMap();

  // create source links and measurements
  const int kSize = 1;
  using ThisMeasurement = Acts::Measurement<Acts::BoundIndices, kSize>;
  std::array<Acts::BoundIndices, kSize> Indices = {Acts::eBoundLoc0};
  std::vector<IndexSourceLink> sourceLinks;
  std::vector<Measurement> measurements;
  std::map<int, Amg::Vector3D> positions;
  std::map<int, Amg::Vector3D> directions;
  std::map<Index, Identifier> identifierLinkMap;
  std::vector<const Tracker::FaserSCT_Cluster*> clusters {};
  // TODO only take best track (most hits, smallest chi2)
  // for now I assume that there is only one track in each station
  for (const Trk::Track* track : *trackCollection) {
    auto fitParameters = track->trackParameters()->front();
    const Amg::Vector3D fitPosition = fitParameters->position();
    const Amg::Vector3D fitMomentum = fitParameters->momentum();
    Identifier id;
    for (const Trk::TrackStateOnSurface* state : *(track->trackStateOnSurfaces())) {
      auto clusterOnTrack = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*> (
          state->measurementOnTrack());
      if (clusterOnTrack) {
        const Tracker::FaserSCT_Cluster* cluster = clusterOnTrack->prepRawData();
        id = cluster->detectorElement()->identify();
        // create source link
        Acts::GeometryIdentifier geoId = identifierMap->at(id);
        ATH_MSG_DEBUG(geoId);
        ATH_MSG_DEBUG(m_idHelper->station(id) << "/" <<
          m_idHelper->layer(id) << "/" <<
          m_idHelper->phi_module(id) << "/" <<
          m_idHelper->eta_module(id) << "/" <<
          m_idHelper->side(id)
        );
        identifierLinkMap[measurements.size()] = id;
        IndexSourceLink sourceLink(geoId, measurements.size());
        // create measurement
        const auto& par = cluster->localPosition();
        const auto& cov = cluster->localCovariance();
        Eigen::Matrix<double, 1, 1> myCov {m_sigmaCluster * m_sigmaCluster,};
        Acts::SourceLink sl{sourceLink};
        ThisMeasurement meas(std::move(sl), Indices, par.head(1), myCov);
//        ThisMeasurement meas(sourceLink, Indices, par.head(1), cov.block<1,1>(0,0));
        sourceLinks.push_back(sourceLink);
        measurements.emplace_back(std::move(meas));
        clusters.push_back(cluster);
      }
    }
    int station = m_idHelper->station(id);
    positions[station] = fitPosition;
    directions[station] = fitMomentum;
  }
  std::vector<std::vector<IndexSourceLink>> sourceLinkVector {sourceLinks};
  m_sourceLinks = std::make_shared<std::vector<std::vector<IndexSourceLink>>>(sourceLinkVector);
  std::vector<std::vector<Measurement>> measurementVector {measurements};
  m_measurements = std::make_shared<std::vector<std::vector<Measurement>>>(measurementVector);
  std::vector<std::map<Index, Identifier>> idLinkVector {identifierLinkMap};
  m_idLinks = std::make_shared<std::vector<std::map<Index, Identifier>>>(idLinkVector);
  std::vector<std::vector<const Tracker::FaserSCT_Cluster*>> clusterVector {clusters};
  m_clusters = std::make_shared<std::vector<std::vector<const Tracker::FaserSCT_Cluster*>>>(clusterVector);

  // TODO how should we handle events without a track in each station?
  if (positions.size() != 3) {
    return StatusCode::RECOVERABLE;
  }

  // create initial surface
  m_initialSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3 {0, 0, 0}, Acts::Vector3{0, 0, 1});

  // create initial parameters
  Acts::Vector3 pos = positions[1] - positions[1].z()/directions[1].z() * directions[1];
  Acts::Vector4 pos4 {pos.x(), pos.y(), pos.z(), 0};
  Acts::Vector3 dir = positions[2] - positions[1];
  auto [abs_momentum, charge] = momentum(positions);
//  Acts::Vector3 pos = positions[1] - positions[1].z()/directions[1].z() * directions[1];
//  Acts::Vector4 pos4 {pos.x(), pos.y(), pos.z(), 0};
//  Acts::Vector3 dir = {directions[1].x(), directions[1].y(), directions[1].z()};
//  double abs_momentum = 1000;
//  double charge = 1;

  Acts::BoundSymMatrix cov = Acts::BoundSymMatrix::Zero();
  cov(Acts::eBoundLoc0, Acts::eBoundLoc0) = m_covLoc0;
  cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = m_covLoc1;
  cov(Acts::eBoundPhi, Acts::eBoundPhi) = m_covPhi;
  cov(Acts::eBoundTheta, Acts::eBoundTheta) = m_covTheta;
  cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = m_covQOverP;
  cov(Acts::eBoundTime, Acts::eBoundTime) = m_covTime;

  
  const auto surface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3 {0, 0, pos.z()}, Acts::Vector3{0, 0, -1});

  Acts::BoundVector params = Acts::BoundVector::Zero();
  params[Acts::eBoundLoc0] = pos.x();
  params[Acts::eBoundLoc1] = pos.y();
  params[Acts::eBoundPhi] = Acts::VectorHelpers::phi(dir.normalized());
  params[Acts::eBoundTheta] = Acts::VectorHelpers::theta(dir.normalized());
  params[Acts::eBoundQOverP] = charge/abs_momentum;
  params[Acts::eBoundTime] = 0;

   //@todo: make the particle hypothesis configurable
  auto initialParameters = Acts::BoundTrackParameters(
      surface, params, cov, Acts::ParticleHypothesis::muon());
  std::vector<Acts::BoundTrackParameters> paramVector {initialParameters};
  m_initialTrackParameters = std::make_shared<std::vector<Acts::BoundTrackParameters>>(paramVector);

  return StatusCode::SUCCESS;
}


StatusCode SegmentFitClusterTrackFinderTool::finalize() {
  return StatusCode::SUCCESS;
}


std::pair<double, double> SegmentFitClusterTrackFinderTool::momentum(const std::map<int, Amg::Vector3D>& pos, double B) {
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
