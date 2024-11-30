#include "TruthSeededTrackFinderTool.h"

#include "Acts/Definitions/TrackParametrization.hpp"
#include "Acts/EventData/Measurement.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/RectangleBounds.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "FaserActsKalmanFilter/IndexSourceLink.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "Identifier/Identifier.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerSpacePoint/SpacePointForSeed.h"
#include <array>
#include <random>


TruthSeededTrackFinderTool::TruthSeededTrackFinderTool(
    const std::string& type, const std::string& name, const IInterface* parent) :
    base_class(type, name, parent) {}


StatusCode TruthSeededTrackFinderTool::initialize() {
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_detManager, "SCT"));
  ATH_CHECK(m_spacePointCollectionKey.initialize());
  ATH_CHECK(m_mcEventCollectionKey.initialize());
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode TruthSeededTrackFinderTool::run() {
//  SG::ReadHandle<FaserSiHitCollection> siHitCollection {m_siHitCollectionKey};
//      ATH_CHECK(siHitCollection.isValid());
//  ATH_MSG_DEBUG("Read FaserSiHitCollection with " << siHitCollection->size() << " hits");

  using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;
  std::shared_ptr<IdentifierMap> identifierMap = m_trackingGeometryTool->getIdentifierMap();

  SG::ReadHandle<SpacePointForSeedCollection> spacePointCollection {m_spacePointCollectionKey};
  ATH_CHECK(spacePointCollection.isValid());

  Acts::GeometryContext geoctx = m_trackingGeometryTool->getGeometryContext().context();

  std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry
      = m_trackingGeometryTool->trackingGeometry();

  const int kSize = 2;
  using ThisMeasurement = Acts::Measurement<Acts::BoundIndices, kSize>;
  std::array<Acts::BoundIndices, kSize> myIndices = {Acts::eBoundLoc0, Acts::eBoundLoc1};

  std::map<int, Acts::Vector3> spacePoints;

  std::vector<IndexSourceLink> sourcelinks;
  std::vector<Measurement> measurements;

  for (const SpacePointForSeed* sp : *spacePointCollection) {
    const Tracker::FaserSCT_SpacePoint* spacePoint = sp->SpacePoint();
    Identifier id = spacePoint->associatedSurface().associatedDetectorElementIdentifier();
    Acts::GeometryIdentifier geoId = identifierMap->at(id);
    // const Acts::Surface *surface = m_trackingGeometryTool->trackingGeometry()->findSurface(geoId);

    auto par = spacePoint->localParameters();
    ATH_MSG_DEBUG("par " << par);
    auto cov = spacePoint->localCovariance();
    ATH_MSG_DEBUG("cov " << cov);
    ATH_MSG_DEBUG(cov(0, 0) << ", " << cov(0, 1) << ", " << cov(1, 0) << ", " << cov(1, 1));
    Acts::ActsSquareMatrix<2> myCov = Acts::ActsSquareMatrix<2>::Zero();
    myCov(0, 0) = m_covMeas00;
    myCov(1, 1) = m_covMeas11;
    myCov(0, 1) = m_covMeas01;
    myCov(1, 0) = m_covMeas10;

    IndexSourceLink sourceLink(geoId, measurements.size());
    Acts::SourceLink sl{sourceLink};
    ThisMeasurement meas(std::move(sl), myIndices, par, myCov);
    sourcelinks.push_back(sourceLink);
    measurements.emplace_back(std::move(meas));

    Amg::Vector3D globalPosition = spacePoint->globalPosition();
    int station = m_idHelper->station(id);
    int layer = m_idHelper->layer(id);
    // TODO check if map already contains a space point for this layer
    spacePoints[3*(station-1) + layer] = Acts::Vector3(globalPosition.x(), globalPosition.y(), globalPosition.z());

    if (station == 1 && layer == 0) {
      const TrackerDD::SiDetectorElement *element = m_detManager->getDetectorElement(id);
      // Construct a plane surface as the target surface
      const TrackerDD::SiDetectorDesign &design = element->design();
      double hlX = design.width()/2. * 1_mm;
      double hlY = design.length()/2. * 1_mm;
      auto rectangleBounds = std::make_shared<const Acts::RectangleBounds>(hlX, hlY);
      Amg::Transform3D g2l = element->getMaterialGeom()->getDefAbsoluteTransform()
                             * Amg::CLHEPTransformToEigen(element->recoToHitTransform());
      m_initialSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(g2l, rectangleBounds);
    }
  }

  // check if there is atleast one space point in each station

  if (map2vector(spacePoints, 1).empty() || map2vector(spacePoints, 2).empty() || map2vector(spacePoints, 0).empty()) {
    return StatusCode::RECOVERABLE;
  }

  Acts::Vector4 smearedPosition4;
  Acts::Vector3 smearedDirection;
  double smearedAbsoluteMomentum;
  double charge;
  if (m_useTrueInitialParameters) {
    // get initial parameters from generated particle
    SG::ReadHandle<McEventCollection> mcEventCollection {m_mcEventCollectionKey};
    ATH_CHECK(mcEventCollection.isValid());
    for (const HepMC::GenEvent* event : *mcEventCollection) {
      for (const HepMC::GenParticle* particle : event->particle_range()) {
        const HepMC::FourVector& momentum = particle->momentum();
        double abs_momentum = momentum.rho() * Acts::UnitConstants::MeV;
        Acts::Vector3 direction = Acts::Vector3(momentum.x(), momentum.y(), momentum.z());
        const HepMC::FourVector vertex = particle->production_vertex()->position();
        charge = particle->pdg_id() > 0 ? -1 : 1;

        std::random_device rd;
        std::default_random_engine rng {rd()};
        std::normal_distribution<> norm;

        auto theta = Acts::VectorHelpers::theta(direction);
        auto phi = Acts::VectorHelpers::phi(direction);
        auto angles = Acts::detail::normalizePhiTheta(
            phi + m_sigmaPhi * norm(rng),
            theta + m_sigmaTheta * norm(rng));
        smearedDirection = Acts::Vector3(
            std::sin(angles.second) * std::cos(angles.first),
            std::sin(angles.second) * std::sin(angles.first),
            std::cos(angles.second));
        smearedAbsoluteMomentum = abs_momentum * (1 + m_sigmaP * norm(rng));

        smearedPosition4 = Acts::Vector4(vertex.x() + m_sigmaLoc0 * norm(rng), vertex.y() + m_sigmaLoc1 * norm(rng), vertex.z(), 0);
      }
    }
  } else {
    // get initial parameters from track seed
    auto [p, q] = momentum2(spacePoints);
    double abs_momentum = p;
    charge = q;
    Acts::Vector3 initPos {0, 0, 0};
    if (spacePoints.count(0)) {
      initPos = spacePoints.at(0);
    } else {
      ATH_MSG_ERROR("Could not find space point on first layer");
    }
    smearedPosition4 = Acts::Vector4(initPos.x(), initPos.y(), initPos.z(), 0);

    auto [pos1, dir1] = linear_fit(map2vector(spacePoints, 1));
    auto [pos2, dir2] = linear_fit(map2vector(spacePoints, 2));
    smearedDirection = pos2 - pos1;
//    smearedDirection = dir;
    smearedAbsoluteMomentum = p;
  }

  Acts::BoundSymMatrix cov = Acts::BoundSymMatrix::Zero();
  cov(Acts::eBoundLoc0, Acts::eBoundLoc0) = m_covLoc0;
  cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = m_covLoc1;
  cov(Acts::eBoundPhi, Acts::eBoundPhi) = m_covPhi;
  cov(Acts::eBoundTheta, Acts::eBoundTheta) = m_covTheta;
  cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = m_covQOverP;
  cov(Acts::eBoundTime, Acts::eBoundTime) = m_covTime;

  const auto surface = Acts::Surface::makeShared<Acts::PlaneSurface>(
    Acts::Vector3 {0, 0, initPos.z()}, Acts::Vector3{0, 0, -1});
  Acts::BoundVector params = Acts::BoundVector::Zero();
  params[Acts::eBoundLoc0] = initPos.x();
  params[Acts::eBoundLoc1] = initPos.y();
  params[Acts::eBoundPhi] = Acts::VectorHelpers::phi(smearedDirection.normalized());
  params[Acts::eBoundTheta] = Acts::VectorHelpers::theta(smearedDirection.normalized());
  params[Acts::eBoundQOverP] = charge/smearedAbsoluteMomentum;
  params[Acts::eBoundTime] = 0;

  //@todo: make the particle hypothesis configurable
  auto initialParameters = Acts::BoundTrackParameters(surface, params, cov, Acts::ParticleHypothesis::muon()); 

  // write out
  double initialCharge = initialParameters.charge();
  double initialAbsoluteMomentum = initialParameters.absoluteMomentum();
  Acts::Vector3 initialPosition = initialParameters.position(geoctx);
  Acts::Vector3 initialMomentum = initialParameters.momentum();
  ATH_MSG_DEBUG("initial charge: " << initialCharge);
  ATH_MSG_DEBUG("initial absolute momentum: " << initialAbsoluteMomentum);
  ATH_MSG_DEBUG("initial position: x=" << initialPosition.x() << ", y=" << initialPosition.y() << ", z=" << initialPosition.z());
  ATH_MSG_DEBUG("initial momentum: x=" << initialMomentum.x() << ", y=" << initialMomentum.y() << ", z=" << initialMomentum.z());
  m_initialTrackParameters = std::make_shared<const Acts::BoundTrackParameters>(initialParameters);

  m_sourceLinks = std::make_shared<std::vector<IndexSourceLink>>(sourcelinks);
  m_measurements = std::make_shared<std::vector<Measurement>>(measurements);

  return StatusCode::SUCCESS;
}

StatusCode TruthSeededTrackFinderTool::finalize() {
  return StatusCode::SUCCESS;
}

Acts::Vector3 TruthSeededTrackFinderTool::average(const std::vector<Acts::Vector3>& spacePoints) {
  Acts::Vector3 ret {0, 0, 0};
  if (!spacePoints.empty()) {
    for (const Acts::Vector3& spacePoint : spacePoints) {
      ret += spacePoint;
    }
    ret /= spacePoints.size();
  }
  return ret;
}

std::pair<Acts::Vector3, Acts::Vector3>
TruthSeededTrackFinderTool::linear_fit(const std::vector<Acts::Vector3>& hits) {
  size_t n_hits = hits.size();
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> centers(n_hits, 3);
  for (size_t i = 0; i < n_hits; ++i) centers.row(i) = hits[i];

  Acts::Vector3 origin = centers.colwise().mean();
  Eigen::MatrixXd centered = centers.rowwise() - origin.transpose();
  Eigen::MatrixXd cov = centered.adjoint() * centered;
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(cov);
  Acts::Vector3 axis = eig.eigenvectors().col(2).normalized();

  return std::make_pair(origin, axis);
}


std::vector<Acts::Vector3>
TruthSeededTrackFinderTool::map2vector(const std::map<int, Acts::Vector3>& map, int station) {
  std::vector<Acts::Vector3> vec;
  for (int layer = station * 3; layer < station * 3 + 3; ++layer) {
    if (map.count(layer)) {
      vec.push_back(map.at(layer));
    }
  }
  return vec;
}


std::pair<double, double> TruthSeededTrackFinderTool::momentum2(const std::map<int, Acts::Vector3>& hits, double B) {
  Acts::Vector3 pos1 = average(map2vector(hits, 0));
  Acts::Vector3 pos2 = average(map2vector(hits, 1));
  Acts::Vector3 pos3 = average(map2vector(hits, 2));

  Acts::Vector3 vec_l = pos3 - pos1;
  double abs_l = std::sqrt(vec_l.y() * vec_l.y() + vec_l.z() * vec_l.z());
  double t = (pos2.z() - pos1.z()) / (pos3.z() - pos1.z());
  Acts::Vector3 vec_m = pos1 + t * vec_l;
  Acts::Vector3 vec_s = pos2 - vec_m;
  double abs_s = std::sqrt(vec_s.y() * vec_s.y() + vec_s.z() * vec_s.z());
  double p_yz = 0.3 * abs_l * abs_l * B / (8 * abs_s * 1000); // in GeV
  // double charge = vec_s.y() < 0 ? 1 : -1;
  double charge = 1;

  return std::make_pair(p_yz, charge);
}
