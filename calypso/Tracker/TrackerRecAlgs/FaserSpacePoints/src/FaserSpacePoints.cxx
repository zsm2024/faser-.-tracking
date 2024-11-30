#include "FaserSpacePoints.h"
#include "Identifier/Identifier.h"
#include "TrackerSimEvent/FaserSiHit.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"


namespace Tracker {

FaserSpacePoints::FaserSpacePoints(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator), AthHistogramming(name),
    m_histSvc("THistSvc/THistSvc", name) {}


StatusCode FaserSpacePoints::initialize() {
  ATH_CHECK(m_spacePointContainerKey.initialize());
  ATH_CHECK(m_siHitCollectionKey.initialize());
  ATH_CHECK(m_simDataCollectionKey.initialize());
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_detMgr, "SCT"));
  m_tree = new TTree("tree", "tree");
  m_tree->Branch("run", &m_run, "run/I");
  m_tree->Branch("event", &m_event, "event/I");
  m_tree->Branch("station", &m_station, "station/I");
  m_tree->Branch("layer", &m_layer, "layer/I");
  m_tree->Branch("phi", &m_phi, "phi/I");
  m_tree->Branch("eta", &m_eta, "eta/I");
  m_tree->Branch("side", &m_side, "side/I");
  m_tree->Branch("x", &m_x, "x/D");
  m_tree->Branch("y", &m_y, "y/D");
  m_tree->Branch("z", &m_z, "z/D");
  m_tree->Branch("muon_hit", &m_muon_hit, "muon_hit/B");
  m_tree->Branch("tx", &m_tx, "tx/D");
  m_tree->Branch("ty", &m_ty, "ty/D");
  m_tree->Branch("tz", &m_tz, "tz/D");
  ATH_CHECK(histSvc()->regTree("/HIST/spacePoints", m_tree));
  return StatusCode::SUCCESS;
}


StatusCode FaserSpacePoints::finalize() {
  return StatusCode::SUCCESS;
}

bool FaserSpacePoints::muon_hit(const TrackerSimDataCollection* simDataMap, Identifier id) const {
  // check if Identifier in simDataMap
  if (!simDataMap->count(id)) {
    return false;
  }
  // check if hit originates from a muon (if the pdg code of the hit is -13)
  TrackerSimData simData = simDataMap->at(id);
  const std::vector<TrackerSimData::Deposit>& deposits = simData.getdeposits();
  auto it = std::find_if(
      deposits.begin(), deposits.end(), [](TrackerSimData::Deposit deposit) {
        return deposit.first->pdg_id() == -13;
      });
  return it != deposits.end();
}

StatusCode FaserSpacePoints::execute(const EventContext& ctx) const {
  m_run = ctx.eventID().run_number();
  m_event = ctx.eventID().event_number();

  SG::ReadHandle<FaserSCT_SpacePointContainer> spacePointContainer {m_spacePointContainerKey, ctx};
  ATH_CHECK(spacePointContainer.isValid());

  SG::ReadHandle<FaserSiHitCollection> siHitCollection {m_siHitCollectionKey, ctx};
  ATH_CHECK(siHitCollection.isValid());

  SG::ReadHandle<TrackerSimDataCollection> simDataCollection {m_simDataCollectionKey, ctx};
  ATH_CHECK(simDataCollection.isValid());


  // create map of the IdentifierHash of a SCT sensor and the true position of the corresponding hit
  std::map<IdentifierHash, HepGeom::Point3D<double>> truePositions;
  for (const FaserSiHit& hit: *siHitCollection) {
    if ((!hit.particleLink()) || (hit.particleLink()->pdg_id() != -13)) {
      continue;
    }
    Identifier waferId = m_idHelper->wafer_id(hit.getStation(), hit.getPlane(), hit.getRow(), hit.getModule(), hit.getSensor());
    IdentifierHash waferHash = m_idHelper->wafer_hash(waferId);
    const HepGeom::Point3D<double> localPosition = hit.localStartPosition();
    const TrackerDD::SiDetectorElement* element = m_detMgr->getDetectorElement(waferId);
    if (element) {
      const HepGeom::Point3D<double> globalPosition =
          Amg::EigenTransformToCLHEP(element->transformHit()) * localPosition;
      truePositions[waferHash] = globalPosition;
    }
  }


  for (const FaserSCT_SpacePointCollection* spacePointCollection : *spacePointContainer) {
    for (const FaserSCT_SpacePoint* spacePoint : *spacePointCollection) {
      Identifier id1 = spacePoint->cluster1()->identify();
      Identifier id2 = spacePoint->cluster2()->identify();
      m_station = m_idHelper->station(id1);
      m_layer = m_idHelper->layer(id1);
      m_phi = m_idHelper->phi_module(id1);
      m_eta = m_idHelper->eta_module(id1);
      m_side = m_idHelper->side(id1);
      // check if both clusters originate from a muon
      m_muon_hit = (muon_hit(simDataCollection.get(), id1) && muon_hit(simDataCollection.get(), id2));
      // global reconstructed position of space point
      Amg::Vector3D position = spacePoint->globalPosition();
      m_x = position.x();
      m_y = position.y();
      m_z = position.z();
      // global true position of simulated hit
      IdentifierHash waferHash = spacePoint->elementIdList().first;
      if (truePositions.count(waferHash)) {
        auto truePosition = truePositions[waferHash];
        m_tx = truePosition.x();
        m_ty = truePosition.y();
        m_tz = truePosition.z();
      } else {
        m_tx = -1;
        m_ty = -1;
        m_tz = -1;
      }
      m_tree->Fill();
    }
  }
  return StatusCode::SUCCESS;
}

}
