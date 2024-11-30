#include "TrackerTruthAlg.h"
#include "TrackerSimEvent/FaserSiHitCollection.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include <TTree.h>


namespace Tracker {

TrackerTruthAlg::TrackerTruthAlg(const std::string &name, ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator), AthHistogramming(name),
    m_histSvc("THistSvc/THistSvc", name), m_sct(nullptr), m_sID(nullptr) {}


StatusCode TrackerTruthAlg::initialize() {
  ATH_CHECK(detStore()->retrieve(m_sct, "SCT"));
  ATH_CHECK(detStore()->retrieve(m_sID, "FaserSCT_ID"));
  ATH_CHECK(m_mcEventCollectionKey.initialize());
  ATH_CHECK(m_faserSiHitKey.initialize());

  m_hits = new TTree("hits", "hits");
  m_hits->Branch("run_number", &m_run_number, "run_number/I");
  m_hits->Branch("event_number", &m_event_number, "event_number/I");
  m_hits->Branch("station", &m_station, "station/I");
  m_hits->Branch("plane", &m_plane, "plane/I");
  m_hits->Branch("module", &m_module, "module/I");
  m_hits->Branch("row", &m_row, "row/I");
  m_hits->Branch("sensor", &m_sensor, "sensor/I");
  m_hits->Branch("pdg", &m_pdg, "pdg/I");
  m_hits->Branch("local_x", &m_local_x, "local_x/D");
  m_hits->Branch("local_y", &m_local_y, "local_y/D");
  m_hits->Branch("global_x", &m_global_x, "global_x/D");
  m_hits->Branch("global_y", &m_global_y, "global_y/D");
  m_hits->Branch("global_z", &m_global_z, "global_z/D");
  ATH_CHECK(histSvc()->regTree("/HIST1/hits", m_hits));

  m_particles = new TTree("particles", "particles");
  m_particles->Branch("run_number", &m_run_number, "run_number/I");
  m_particles->Branch("event_number", &m_event_number, "event_number/I");
  m_particles->Branch("pdg", &m_pdg, "pdg/I");
  m_particles->Branch("x", &m_vertex_x, "x/D");
  m_particles->Branch("y", &m_vertex_y, "y/D");
  m_particles->Branch("z", &m_vertex_z, "z/D");
  m_particles->Branch("px", &m_px, "px/D");
  m_particles->Branch("py", &m_px, "py/D");
  m_particles->Branch("pz", &m_px, "pz/D");
  ATH_CHECK(histSvc()->regTree("/HIST1/particles", m_particles));

  return StatusCode::SUCCESS;
}


StatusCode TrackerTruthAlg::execute(const EventContext &ctx) const {

  m_run_number = ctx.eventID().run_number();
  m_event_number = ctx.eventID().event_number();

  // Fill tree with simulated particles
  SG::ReadHandle<McEventCollection> mcEventCollection(m_mcEventCollectionKey, ctx);
  ATH_CHECK(mcEventCollection.isValid());
  for (const HepMC::GenEvent* event : *mcEventCollection) {
    for (const HepMC::GenParticle* particle : event->particle_range()) {
      m_pdg = particle->pdg_id();
      const auto vertex = particle->production_vertex()->position();
      m_vertex_x = vertex.x();
      m_vertex_y = vertex.y();
      m_vertex_z = vertex.z();
      const HepMC::FourVector& momentum = particle->momentum();
      m_px = momentum.x();
      m_py = momentum.y();
      m_pz = momentum.z();
      m_particles->Fill();
    }
  }

  // Fill tree with simulated hits
  SG::ReadHandle<FaserSiHitCollection> siHitCollection(m_faserSiHitKey, ctx);
  ATH_CHECK(siHitCollection.isValid());
  for (const FaserSiHit &hit: *siHitCollection) {
    if (!hit.particleLink().isValid())
      continue;
    const HepGeom::Point3D<double> localStartPos = hit.localStartPosition();
    Identifier id = m_sID->wafer_id(hit.getStation(), hit.getPlane(), hit.getRow(), hit.getModule(), hit.getSensor());
    const TrackerDD::SiDetectorElement *geoelement = m_sct->getDetectorElement(id);
    if (!geoelement)
      continue;
    const HepGeom::Point3D<double> globalStartPos =
        Amg::EigenTransformToCLHEP(geoelement->transformHit()) * HepGeom::Point3D<double>(localStartPos);
    m_station = hit.getStation();
    m_plane = hit.getPlane();
    m_module = hit.getModule();
    m_row = hit.getRow();
    m_sensor = hit.getSensor();
    m_pdg = hit.particleLink()->pdg_id();
    m_local_x = localStartPos.x();
    m_local_y = localStartPos.y();
    m_global_x = globalStartPos.x();
    m_global_y = globalStartPos.y();
    m_global_z = globalStartPos.z();
    m_hits->Fill();
  }

  return StatusCode::SUCCESS;
}


StatusCode TrackerTruthAlg::finalize() {
  return StatusCode::SUCCESS;
}

}  // namespace Tracker
