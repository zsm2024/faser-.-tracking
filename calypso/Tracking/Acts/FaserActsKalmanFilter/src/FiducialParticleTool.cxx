#include "FiducialParticleTool.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"

constexpr float NaN = std::numeric_limits<double>::quiet_NaN();

FiducialParticleTool::FiducialParticleTool(const std::string &type, const std::string &name,
                                           const IInterface *parent)
    : base_class(type, name, parent) {}

StatusCode FiducialParticleTool::initialize() {
  ATH_CHECK(m_siHitCollectionKey.initialize());
  ATH_CHECK(detStore()->retrieve(m_sctHelper, "FaserSCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_detMgr, "SCT"));
  return StatusCode::SUCCESS;
}

StatusCode FiducialParticleTool::finalize() { return StatusCode::SUCCESS; }

bool FiducialParticleTool::isFiducial(int barcode) const {
  auto positions = getTruthPositions(barcode);
  bool isFiducial{true};
  for (int station = 1; station < 4; ++station) {
    double x{positions[station].x()};
    double y{positions[station].y()};
    if (std::isnan(x) || std::isnan(y) || (x * x + y * y > 100 * 100))
      isFiducial = false;
  }
  return isFiducial;
}

HepGeom::Point3D<double> FiducialParticleTool::getGlobalPosition(const FaserSiHit &hit) const {
  Identifier waferId = m_sctHelper->wafer_id(hit.getStation(), hit.getPlane(), hit.getRow(),
                                             hit.getModule(), hit.getSensor());
  auto localStartPos = hit.localStartPosition();
  auto localEndPos = hit.localEndPosition();
  HepGeom::Point3D<double> localPos = 0.5 * (localEndPos + localStartPos);
  const TrackerDD::SiDetectorElement *element = m_detMgr->getDetectorElement(waferId);
  auto globalPosition = Amg::EigenTransformToCLHEP(element->transformHit()) * localPos;
  return globalPosition;
}

HepGeom::Point3D<double> FiducialParticleTool::getMomentum(const FaserSiHit &hit) const {
  Identifier waferId = m_sctHelper->wafer_id(hit.getStation(), hit.getPlane(), hit.getRow(),
                                             hit.getModule(), hit.getSensor());
  auto localStartPos = hit.localStartPosition();
  auto localEndPos = hit.localEndPosition();
  const TrackerDD::SiDetectorElement *element = m_detMgr->getDetectorElement(waferId);
  auto globalStartPosition = Amg::EigenTransformToCLHEP(element->transformHit()) * localStartPos;
  auto globalEndPosition = Amg::EigenTransformToCLHEP(element->transformHit()) * localEndPos;
  double p = hit.particleLink()->momentum().rho();
  HepGeom::Point3D<double> globalDirection = globalEndPosition - globalStartPosition;
  HepGeom::Point3D<double> momentum = globalDirection / globalDirection.mag() * p;
  return momentum;
}

std::array<HepGeom::Point3D<double>, 4> FiducialParticleTool::getTruthPositions(int barcode) const {
  // initialize positions as NaN
  std::array<HepGeom::Point3D<double>, 4> positions{};
  for (auto &station : positions) {
    station.setX(NaN);
    station.setY(NaN);
    station.setZ(NaN);
  }

  // get simulated hits
  SG::ReadHandle<FaserSiHitCollection> siHitCollection(m_siHitCollectionKey);
  if (!siHitCollection.isValid()) {
    ATH_MSG_WARNING("FaserSiHitCollection not valid.");
    return positions;
  }

  // create map with truth positions in each station
  std::array<std::vector<HepGeom::Point3D<double>>, 4> hitMap{};
  for (const FaserSiHit &hit : *siHitCollection) {
    if (hit.trackNumber() == barcode) {
      auto position = getGlobalPosition(hit);
      hitMap[hit.getStation()].push_back(position);
    }
  }

  // calculate average position in each station
  for (int station = 0; station < 4; ++station) {
    std::vector<HepGeom::Point3D<double>> &hits{hitMap[station]};
    if (hits.empty()) {
      continue;
    } else {
      // calculate average position of all FaserSiHits in a station
      auto const count = static_cast<double>(hits.size());
      HepGeom::Point3D<double> sums{};
      for (const HepGeom::Point3D<double> &hit : hits)
        sums += hit;
      positions[station] = sums / count;
    }
  }
  return positions;
}

std::array<HepGeom::Point3D<double>, 4> FiducialParticleTool::getTruthMomenta(int barcode) const {
  // initialize positions as NaN
  std::array<HepGeom::Point3D<double>, 4> momenta{};
  for (auto &station : momenta) {
    station.setX(NaN);
    station.setY(NaN);
    station.setZ(NaN);
  }

  // get simulated hits
  SG::ReadHandle<FaserSiHitCollection> siHitCollection(m_siHitCollectionKey);
  if (!siHitCollection.isValid()) {
    ATH_MSG_WARNING("FaserSiHitCollection not valid.");
    return momenta;
  }

  // create map with truth momenta in each station
  std::array<std::vector<HepGeom::Point3D<double>>, 4> hitMap{};
  for (const FaserSiHit &hit : *siHitCollection) {
    if (hit.trackNumber() == barcode) {
      HepGeom::Point3D<double> momentum = getMomentum(hit);
      hitMap[hit.getStation()].push_back(momentum);
    }
  }

  // calculate average momentum in each station
  for (int station = 0; station < 4; ++station) {
    std::vector<HepGeom::Point3D<double>> &hits{hitMap[station]};
    if (hits.empty()) {
      continue;
    } else {
      // calculate average momentum of all FaserSiHits in a station
      auto const count = static_cast<double>(hits.size());
      HepGeom::Point3D<double> sums{};
      for (const HepGeom::Point3D<double> &hit : hits) {
        sums += hit;
      }
      momenta[station] = sums / count;
    }
  }
  return momenta;
}
