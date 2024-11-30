#include "CircleFitTrackSeedTool.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "Identifier/Identifier.h"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "CircleFit.h"
#include "LinearFit.h"
#include "TrackClassification.h"
#include <array>
#include <algorithm>


std::map<Identifier,Index> CircleFitTrackSeedTool::s_indexMap {};
std::map<Identifier, const Tracker::FaserSCT_SpacePoint*> CircleFitTrackSeedTool::s_spacePointMap {};

CircleFitTrackSeedTool::CircleFitTrackSeedTool(
    const std::string& type, const std::string& name, const IInterface* parent)
    : base_class(type, name, parent) {}


StatusCode CircleFitTrackSeedTool::initialize() {
      ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
      ATH_CHECK(detStore()->retrieve(m_detManager, "SCT"));
      ATH_CHECK(m_trackingGeometryTool.retrieve());
      ATH_CHECK(m_trackCollection.initialize());
      ATH_CHECK(m_clusterContainerKey.initialize());
      ATH_CHECK(m_spacePointContainerKey.initialize());
  // ATH_CHECK(m_simDataCollectionKey.initialize());
  // ATH_CHECK(m_mcEventCollectionKey.initialize());
  return StatusCode::SUCCESS;
}


StatusCode CircleFitTrackSeedTool::run(std::vector<int> maskedLayers, bool backward) {

  SG::ReadHandle<Tracker::FaserSCT_ClusterContainer> clusterContainer {m_clusterContainerKey};
      ATH_CHECK(clusterContainer.isValid());

  SG::ReadHandle<FaserSCT_SpacePointContainer> spacePointContainer {m_spacePointContainerKey};
      ATH_CHECK(spacePointContainer.isValid());

  // SG::ReadHandle<TrackerSimDataCollection> simData {m_simDataCollectionKey};
  // ATH_CHECK(simData.isValid());

  // SG::ReadHandle<McEventCollection> mcEvents {m_mcEventCollectionKey};
  // ATH_CHECK(mcEvents.isValid());

  using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;
  std::shared_ptr<IdentifierMap> identifierMap = m_trackingGeometryTool->getIdentifierMap();
  // std::map<int, const HepMC::GenParticle*> particles {};
  // for (const HepMC::GenParticle* particle : mcEvents->front()->particle_range()) {
  //   particles[particle->barcode()] = particle;
  // }

  // std::map<Identifier, const Tracker::FaserSCT_SpacePoint*> s_spacePointMap {};
  std::vector<const Tracker::FaserSCT_SpacePoint*> spacePoints {};
  for (const FaserSCT_SpacePointCollection* spacePointCollection : *spacePointContainer) {
    for (const Tracker::FaserSCT_SpacePoint *spacePoint: *spacePointCollection) {
      spacePoints.push_back(spacePoint);
      Identifier id1 = spacePoint->cluster1()->identify();
      CircleFitTrackSeedTool::s_spacePointMap[id1] = spacePoint;
      Identifier id2 = spacePoint->cluster2()->identify();
      CircleFitTrackSeedTool::s_spacePointMap[id2] = spacePoint;
    }
  }


  using ThisMeasurement = Acts::Measurement<Acts::BoundIndices, 1>;
  std::array<Acts::BoundIndices, 1> Indices = {Acts::eBoundLoc0};
  std::vector<IndexSourceLink> sourceLinks {};
  std::vector<Measurement> measurements {};
  std::vector<const Tracker::FaserSCT_Cluster*> clusters {};
  for (const Tracker::FaserSCT_ClusterCollection* clusterCollection : *clusterContainer) {
    for (const Tracker::FaserSCT_Cluster* cluster : *clusterCollection) {
      clusters.push_back(cluster);
      Identifier id = cluster->detectorElement()->identify();
      if (std::find(maskedLayers.begin(), maskedLayers.end(), 3 * m_idHelper->station(id) + m_idHelper->layer(id)) != maskedLayers.end()) {
        continue;
      }
//      if(m_idHelper->station(id)==0) continue;
      CircleFitTrackSeedTool::s_indexMap[cluster->identify()] = measurements.size();
      if (identifierMap->count(id) != 0) {
        Acts::GeometryIdentifier geoId = identifierMap->at(id);
        IndexSourceLink sourceLink(geoId, measurements.size(), cluster);
        // create measurement
        const auto& par = cluster->localPosition();
	auto cova = cluster->localCovariance();
        Eigen::Matrix<double, 1, 1> pos {par.x(),};
        Eigen::Matrix<double, 1, 1> cov {0.0231 * 0.0231,};
        Acts::SourceLink sl{sourceLink}; 
        ThisMeasurement meas(std::move(sl), Indices, pos, cov);
        sourceLinks.push_back(sourceLink);
        measurements.emplace_back(std::move(meas));
      }
    }
  }

  SG::ReadHandle<TrackCollection> trackCollection {m_trackCollection};
      ATH_CHECK(trackCollection.isValid());

  std::array<std::vector<Segment>, 4> segments {};
  for (const Trk::Track* track : *trackCollection) {
    auto s = Segment(track, m_idHelper, maskedLayers);
    //remove IFT in seeding
    if(m_removeIFT){
    if (s.station != -1 &&s.station != 0) segments[s.station].push_back(s);
    }
    else{
    if (s.station != -1) segments[s.station].push_back(s);
    }
  }

  std::vector<Segment> combination {};
  std::vector<Seed> seeds {};
  // create seeds from four stations
  go(segments, combination, seeds, 0, 4, backward);
  if (seeds.size() < 2) {
    // create seeds from three stations
    go(segments, combination, seeds, 0, 3, backward);
  }
  // create seeds from two stations
  if (seeds.size() < 2) {
    go(segments, combination, seeds, 0, 2, backward);
  }
  if (seeds.size() < 2) {
    go(segments, combination, seeds, 0, 1, backward);
  }

  std::list<Seed> allSeeds;
  for (const Seed &seed : seeds) allSeeds.push_back(seed);

  // allSeeds.sort([](const Seed &left, const Seed &right) {
  //   if (left.size > right.size) return true;
  //   if (left.size < right.size) return false;
  //   if (left.chi2 < right.chi2) return true;
  //   else return false;
  // });
  allSeeds.sort([](const Seed &left, const Seed &right) {
    if (left.stations > right.stations) return true;
    if (left.stations < right.stations) return false;
    if (left.chi2/std::max(left.positions.size() + left.fakePositions.size() - left.constraints, 1UL) < right.chi2/std::max(right.positions.size() + right.fakePositions.size() - right.constraints, 1UL)) return true;
    else return false;
  });

  std::vector<Seed> selectedSeeds {};
  while (not allSeeds.empty()) {
    Seed selected = allSeeds.front();
    selectedSeeds.push_back(selected);
    // allSeeds.remove_if([&](const Seed &p) {
    //   return ((p.clusterSet & selected.clusterSet).count() == p.clusterSet.count());
    // });
    allSeeds.remove_if([&](const Seed &p) {
      return (p.size < 10) || ((p.clusterSet & selected.clusterSet).count() > 6);
    });
  }

  Acts::BoundSquareMatrix cov = Acts::BoundSquareMatrix::Zero();
  cov(Acts::eBoundLoc0, Acts::eBoundLoc0) = m_covLoc0;
  cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = m_covLoc1;
  cov(Acts::eBoundPhi, Acts::eBoundPhi) = m_covPhi;
  cov(Acts::eBoundTheta, Acts::eBoundTheta) = m_covTheta;
  cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = m_covQOverP;
  cov(Acts::eBoundTime, Acts::eBoundTime) = m_covTime;

  double origin;
  if (!backward) {
    auto minSeed = std::min_element(
        selectedSeeds.begin(), selectedSeeds.end(), [](const Seed &lhs, const Seed &rhs) {
          return lhs.minZ < rhs.minZ;
        });
    origin = !selectedSeeds.empty() ? minSeed->minZ - 10 : 0;
  } else {
    origin = 2470;
  }
  m_targetZPosition = origin;
  std::vector<Acts::BoundTrackParameters> initParams {};
  ATH_MSG_DEBUG("Sorted seed properties:");
  for (const Seed &seed : selectedSeeds) {
    ATH_MSG_DEBUG("seed size: " << seed.size << ", chi2: " << seed.chi2);
    initParams.push_back(seed.get_params(origin, cov, backward));
  }
  m_seeds = std::make_shared<std::vector<Seed>>(selectedSeeds);
  m_initialTrackParameters = std::make_shared<std::vector<Acts::BoundTrackParameters>>(initParams);
  m_sourceLinks = std::make_shared<std::vector<IndexSourceLink>>(sourceLinks);
  // m_idLinks = std::make_shared<IdentifierLink>(identifierLinkMap);
  m_measurements = std::make_shared<std::vector<Measurement>>(measurements);
  //@todo make sure this is correct 
  m_initialSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3 {0, 0, origin}, Acts::Vector3{0, 0, -1});
  m_clusters = std::make_shared<std::vector<const Tracker::FaserSCT_Cluster*>>(clusters);
  m_spacePoints = std::make_shared<std::vector<const Tracker::FaserSCT_SpacePoint*>>(spacePoints);
  s_indexMap.clear();


  return StatusCode::SUCCESS;
}


StatusCode CircleFitTrackSeedTool::finalize() {
  return StatusCode::SUCCESS;
}


void CircleFitTrackSeedTool::go(const std::array<std::vector<Segment>, 4> &v,
                                std::vector<Segment> &combination,
                                std::vector<Seed> &seeds,
                                int offset, int k,
                                bool backward) {
  if (k == 0) {
    seeds.push_back(Seed(combination, backward));
    return;
  }
  for (std::size_t i = offset; i < v.size() + 1 - k; ++i) {
    for (const auto& ve : v[i]) {
      combination.push_back(ve);
      go(v, combination, seeds, i+1, k-1, backward);
      combination.pop_back();
    }
  }
}

CircleFitTrackSeedTool::Segment::Segment(const Trk::Track* track, const FaserSCT_ID *idHelper, const std::vector<int> &maskedLayers) :
    clusterSet(CircleFitTrackSeedTool::s_indexMap.size()) {
  for (const Trk::TrackStateOnSurface* trackState : *(track->trackStateOnSurfaces())) {
    auto clusterOnTrack = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*> (trackState->measurementOnTrack());
    if (clusterOnTrack) {
      Measurements.push_back(trackState->measurementOnTrack());
      const Tracker::FaserSCT_Cluster* cluster = clusterOnTrack->prepRawData();
      Identifier id = cluster->identify();
      if (std::find(maskedLayers.begin(), maskedLayers.end(), 3 * idHelper->station(id) + idHelper->layer(id)) != maskedLayers.end()) {
        continue;
      }

      /*for(auto& meas : (CircleFitTrackSeedTool::Meas)){
        if (meas.sourceLink().identifier()==id){
          Measurements.push_back(meas);
	}
      }*/
      clusters.push_back(cluster);
      if (CircleFitTrackSeedTool::s_spacePointMap.count(id) > 0) {
        const Tracker::FaserSCT_SpacePoint *sp = CircleFitTrackSeedTool::s_spacePointMap.at(cluster->identify());
        if (std::find(spacePoints.begin(), spacePoints.end(), sp) == spacePoints.end()) {
          spacePoints.push_back(sp);
        }
      }
      station = idHelper->station(id);
//      if(station==0)continue;
      clusterSet.set(CircleFitTrackSeedTool::s_indexMap.at(id));
      auto fitParameters = track->trackParameters()->front();
      position = fitParameters->position();
      momentum = fitParameters->momentum();
    }
  }
  fakePositions.push_back(position);
  fakePositions.push_back(position - 30 * momentum.normalized());
  fakePositions.push_back(position + 30 * momentum.normalized());
}

CircleFitTrackSeedTool::Seed::Seed(const std::vector<Segment> &segments, bool backward) :
    clusterSet(CircleFitTrackSeedTool::s_indexMap.size()) {
  for (const Segment &seg : segments) {
    segs.push_back(seg);//get the segments of one Seed*********************************
    clusters.insert(clusters.end(), seg.clusters.begin(), seg.clusters.end());
    //get measurements of every Seed**********
    Measurements.insert(Measurements.end(),seg.Measurements.begin(),seg.Measurements.end());
    //***********************************************************************************
    spacePoints.insert(spacePoints.end(), seg.spacePoints.begin(), seg.spacePoints.end());
    positions.push_back(seg.position);
    
    // TODO use reconstruct space points instead of fake positions
    fakePositions.insert(fakePositions.end(), seg.fakePositions.begin(), seg.fakePositions.end());
    for (size_t i = 0; i < seg.clusterSet.size(); ++i) {
      if (seg.clusterSet[i]) clusterSet.set(i);
    }
  }

  auto minCluster = std::min_element(
      clusters.begin(), clusters.end(), [](const Tracker::FaserSCT_Cluster *lhs, const Tracker::FaserSCT_Cluster *rhs) {
        return lhs->globalPosition().z() < rhs->globalPosition().z();
      } );
  minZ = (*minCluster)->globalPosition().z();

  if ((segments.size() > 1) && (!backward)) {
    direction = positions[1] - positions[0];
  } else if ((segments.size() > 1) && (backward)) {
    size_t size = segments.size();
    direction = positions[size-1] - positions[size-2];
  } else {
    direction = segments[0].momentum;
  }

  std::vector<Acts::Vector2> pointsZX {};
  for (const Acts::Vector3 &pos : fakePositions) {
    pointsZX.push_back({pos.z(), pos.x()});
  }
  linearFit(pointsZX);

  if (segments.size() > 1) {
    fakeFit();
    constraints = 5;
  } else {
    momentum = 9999999.;
    charge = 1;
    constraints = 2;
  }

  getChi2();
  size = clusters.size();
  stations = segments.size();
}


void CircleFitTrackSeedTool::Seed::fit() {
  CircleFit::CircleData circleData(positions);
  CircleFit::Circle circle = CircleFit::circleFit(circleData);
  momentum = circle.r > 0 ? circle.r * 0.001 * 0.3 * 0.55 : 9999999.;
  charge = circle.cy < 0 ? 1 : -1;
}

void CircleFitTrackSeedTool::Seed::fakeFit(double B) {
  CircleFit::CircleData circleData(fakePositions);
  CircleFit::Circle circle = CircleFit::circleFit(circleData);
  cx = circle.cx;
  cy = circle.cy;
  r = circle.r;
  momentum = r * 0.3 * B * m_MeV2GeV;
  charge = circle.cy < 0 ? 1 : -1;
}

void CircleFitTrackSeedTool::Seed::linearFit(const std::vector<Acts::Vector2> &points) {
  auto [origin, dir] = LinearFit::linearFit(points);
  c1 = dir[1]/dir[0];
  c0 = origin[1] - origin[0] * c1;
}


double CircleFitTrackSeedTool::Seed::getY(double z) {
  double sqt = std::sqrt(-cx*cx + 2*cx*z + r*r - z*z);
  return abs(cy - sqt) < abs(cy + sqt) ? cy - sqt : cy + sqt;
}


double CircleFitTrackSeedTool::Seed::getX(double z) {
  return c0 + z * c1;
}

void CircleFitTrackSeedTool::Seed::getChi2() {
  chi2 = 0;
  for (const Acts::Vector3 &pos : fakePositions) {
    m_dy = pos.y() - getY(pos.z());
    chi2 += (m_dy * m_dy) / (m_sigma_y * m_sigma_y);
  }

  for (const Acts::Vector3 &pos : fakePositions) {
    m_dx = pos.x() - getX(pos.z());
    chi2 += (m_dx * m_dx) / (m_sigma_x * m_sigma_x);
  }
}

Acts::BoundTrackParameters CircleFitTrackSeedTool::Seed::get_params(double origin, Acts::BoundSquareMatrix cov, bool backward) const {
  double theta = Acts::VectorHelpers::theta(direction);
  double phi = Acts::VectorHelpers::phi(direction);

  Acts::BoundVector params = Acts::BoundVector::Zero();
  params[Acts::eBoundLoc0] = 0;
  params[Acts::eBoundLoc1] = 0;
  params[Acts::eBoundTime] = 0;
  params[Acts::eBoundPhi] = phi;
  params[Acts::eBoundTheta] = theta;
  //params[Acts::eBoundQOverP] = particleHypothesis.qOverP(p, charge);
  params[Acts::eBoundQOverP] = charge/momentum;

  Acts::Vector3 pos(0,0,0);
  if (!backward) {
    pos = positions[0] - (positions[0].z() - origin)/direction.z() * direction;
  } else {
    size_t size = positions.size();
    pos = positions[size-1] + (origin - positions[size-1].z())/direction.z() * direction;
  }
  auto refPlane = Acts::Surface::makeShared<Acts::PlaneSurface>(pos, direction.normalized()); 
 //@todo make the particle hypothesis configurable
  return Acts::BoundTrackParameters(refPlane, params, cov, Acts::ParticleHypothesis::muon()); 
}
