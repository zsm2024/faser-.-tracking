#include "TruthTrackFinderTool.h"

#include "Acts/Definitions/TrackParametrization.hpp"
#include "Acts/EventData/Measurement.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "FaserActsKalmanFilter/IndexSourceLink.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "Identifier/Identifier.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include <array>
#include <random>


TruthTrackFinderTool::TruthTrackFinderTool(const std::string& type, const std::string& name, const IInterface* parent)
    : base_class(type, name, parent) {}


StatusCode TruthTrackFinderTool::initialize() {
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_detManager, "SCT"));
  ATH_CHECK(m_siHitCollectionKey.initialize());
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode TruthTrackFinderTool::run() {
  SG::ReadHandle<FaserSiHitCollection> siHitCollection {m_siHitCollectionKey};
  ATH_CHECK(siHitCollection.isValid());
  ATH_MSG_DEBUG("Read FaserSiHitCollection with " << siHitCollection->size() << " hits");

  std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry
      = m_trackingGeometryTool->trackingGeometry();

  using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;
  std::shared_ptr<IdentifierMap> identifierMap = m_trackingGeometryTool->getIdentifierMap();
  Acts::GeometryContext gctx = m_trackingGeometryTool->getGeometryContext().context();
  const int kSize = 2;
  using ParametersVector = Acts::ActsVector<kSize>;
  using CovarianceMatrix = Acts::ActsSymMatrix<kSize>;
  using ThisMeasurement = Acts::Measurement<Acts::BoundIndices, kSize>;
  std::array<Acts::BoundIndices, kSize> myIndices = {Acts::eBoundLoc0, Acts::eBoundLoc1};

  std::vector<IndexSourceLink> sourcelinks;
  std::vector<Measurement> measurements;

  std::vector<Acts::GeometryIdentifier> geoIds = {};
  int station, layer, phi, eta, side;
  for (const FaserSiHit &hit: *siHitCollection) {
    if (!hit.particleLink() or std::abs(hit.particleLink()->pdg_id()) != 13) {
      continue;
    }

    station = hit.getStation();
    layer = hit.getPlane();
    phi = hit.getRow();
    eta = hit.getModule();
    side = hit.getSensor();

    if (side == 0) {
      continue;
    }

    ATH_MSG_DEBUG(station << "/" << layer << "/" << phi << "/" << eta << "/" << side);

    Identifier id = m_idHelper->wafer_id(station, layer, phi, eta, side);
    // TODO check if there can be multiple simulated muon hits in the same wafer
    // currently I take the first hit in each wafer
    Acts::GeometryIdentifier geoId = identifierMap->at(id);
    if (std::find(geoIds.begin(), geoIds.end(), geoId) != geoIds.end()) {
      continue;
    }
    geoIds.push_back(geoId);
    ATH_MSG_DEBUG(geoIds);

    auto momentum = hit.particleLink()->momentum();
    double abs_momentum = momentum.rho() * Acts::UnitConstants::MeV;
    double QOverP = 1 / abs_momentum;
    auto direction = Acts::Vector3(momentum.x(), momentum.y(), momentum.z()).normalized();

    const TrackerDD::SiDetectorElement *element = m_detManager->getDetectorElement(id);

    // create source links and measurements
    const Acts::Surface *surface = m_trackingGeometryTool->trackingGeometry()->findSurface(geoId);
    if (surface == nullptr) {
      ATH_MSG_FATAL("Could not find surface");
      continue;
    }

    const HepGeom::Point3D<double> localStartPos = hit.localStartPosition();
    if (element) {
      const HepGeom::Point3D<double> globalStartPos =
          Amg::EigenTransformToCLHEP(element->transformHit()) * HepGeom::Point3D<double>(localStartPos);
      auto center = surface->center(gctx);
      Acts::Vector3 position = {globalStartPos.x(), globalStartPos.y(), center.z()};
      Acts::Result<Acts::BoundVector> boundParamsRes
          = Acts::detail::transformFreeToBoundParameters(position, hit.meanTime(), direction, QOverP, *surface, gctx);
      if (!boundParamsRes.ok()) {
        ATH_MSG_FATAL("Could not construct bound parameters");
        return StatusCode::FAILURE;
      }
      const auto &boundParams = boundParamsRes.value();
      ATH_MSG_DEBUG(boundParams[0] << ", " << boundParams[1]);

      std::random_device rd;
      std::default_random_engine rng {rd()};
      std::normal_distribution<> norm;

      ParametersVector smearedBoundParams = {
          boundParams[0] + norm(rng) * m_sigma0, boundParams[1] + norm(rng) * m_sigma1};

      ParametersVector par = ParametersVector::Zero();
      par[0] = smearedBoundParams[0];
      par[1] = smearedBoundParams[1];

      ATH_MSG_DEBUG("bound parameters: par0=" << boundParams[0] << ", par1=" << boundParams[1]);
      ATH_MSG_DEBUG("smeared parameters: par0=" << smearedBoundParams[0] << ", par1=" << smearedBoundParams[1]);

      CovarianceMatrix cov = CovarianceMatrix::Zero();
      cov(0, 0) = std::max(m_covMeasLoc0.value(), m_sigma0 * m_sigma0);
      cov(1, 1) = std::max(m_covMeasLoc1.value(), m_sigma1 * m_sigma1);

      IndexSourceLink sourceLink(geoId, measurements.size());
      sourcelinks.emplace_back(sourceLink);
      Acts::SourceLink sl{sourceLink};
      ThisMeasurement meas(std::move(sl), myIndices, par, cov);
      measurements.emplace_back(std::move(meas));

      // create initial parameters from hit in first layer
      if ((station == m_first_station) && (layer == m_first_layer) && (side == m_first_side)) {

        // smear initial direction
         auto theta = Acts::VectorHelpers::theta(direction);
         auto phi = Acts::VectorHelpers::phi(direction);
         auto angles = Acts::detail::normalizePhiTheta(
             phi + m_sigmaPhi * M_PI/180. * norm(rng),
             theta + m_sigmaTheta * M_PI/180. * norm(rng));
         Acts::Vector3 smearedDirection(
             std::sin(angles.second) * std::cos(angles.first),
             std::sin(angles.second) * std::sin(angles.first),
             std::cos(angles.second));
         double smearedAbsoluteMomentum = abs_momentum * (1 + m_sigmaP * norm(rng));

        double z_init = 0;
        m_initialSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(Acts::Vector3 {0, 0, z_init}, Acts::Vector3{0, 0, 1});

        // extrapolate position on first layer to initial position
        // TODO use first layer as initial layer instead?
        Acts::Vector3 initPosition = position + (z_init - position.z()) / direction.z() * direction;
        Acts::Vector3 smearedPosition {initPosition.x() + m_sigmaLoc0 * norm(rng), initPosition.y() + m_sigmaLoc1 * norm(rng), initPosition.z()};
        Acts::Vector4 smearedPosition4 {smearedPosition.x(), smearedPosition.y(), smearedPosition.z(), 0};
        double charge = hit.particleLink()->pdg_id() > 0 ? -1 : 1;

        Acts::BoundSymMatrix cov = Acts::BoundSymMatrix::Zero();
        cov(Acts::eBoundLoc0, Acts::eBoundLoc0) = m_covLoc0;
        cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = m_covLoc1;
        cov(Acts::eBoundPhi, Acts::eBoundPhi) = m_covPhi;
        cov(Acts::eBoundTheta, Acts::eBoundTheta) = m_covTheta;
        cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = m_covQOverP;
        cov(Acts::eBoundTime, Acts::eBoundTime) = m_covTime;

        const auto surface = Acts::Surface::makeShared<Acts::PlaneSurface>(
          Acts::Vector3 {0, 0, smearedPosition.z()}, Acts::Vector3{0, 0, -1});
        Acts::BoundVector params = Acts::BoundVector::Zero();
        params[Acts::eBoundLoc0] = smearedPosition.x();
        params[Acts::eBoundLoc1] = smearedPosition.y();
        params[Acts::eBoundPhi] = Acts::VectorHelpers::phi(smearedDirection.normalized());
        params[Acts::eBoundTheta] = Acts::VectorHelpers::theta(smearedDirection.normalized());
        params[Acts::eBoundQOverP] = charge/smearedAbsoluteMomentum;
        params[Acts::eBoundTime] = 0;
      
        //@todo: make the particle hypothesis configurable
        auto initialParameters = Acts::BoundTrackParameters(surface, params, cov, Acts::ParticleHypothesis::muon());

        // write out
        double initialCharge = initialParameters.charge();
        double initialAbsoluteMomentum = initialParameters.absoluteMomentum();
        Acts::Vector3 initialPosition = initialParameters.position(gctx);
        Acts::Vector3 initialMomentum = initialParameters.momentum();
        ATH_MSG_DEBUG("initial charge: " << initialCharge);
        ATH_MSG_DEBUG("initial absolute momentum: " << initialAbsoluteMomentum);
        ATH_MSG_DEBUG("initial position: x=" << initialPosition.x() << ", y=" << initialPosition.y() << ", z=" << initialPosition.z());
        ATH_MSG_DEBUG("initial momentum: x=" << initialMomentum.x() << ", y=" << initialMomentum.y() << ", z=" << initialMomentum.z());
        m_initialTrackParameters = std::make_shared<const Acts::BoundTrackParameters>(initialParameters);
      }
    }
  }

  m_sourceLinks = std::make_shared<std::vector<IndexSourceLink>>(sourcelinks);
  m_measurements = std::make_shared<std::vector<Measurement>>(measurements);

  return StatusCode::SUCCESS;
}

StatusCode TruthTrackFinderTool::finalize() {
  return StatusCode::SUCCESS;
}
