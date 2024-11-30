#include "ProtoTrackWriterTool.h"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Identifier/Identifier.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include <TFile.h>
#include <TTree.h>


ProtoTrackWriterTool::ProtoTrackWriterTool(
    const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent) {}


StatusCode ProtoTrackWriterTool::initialize() {

  ATH_CHECK(m_trackFinderTool.retrieve());
  ATH_CHECK(m_trackingGeometryTool.retrieve());

  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));

  std::string filePath = m_filePath;
  m_file = TFile::Open(filePath.c_str(), "RECREATE");
  if (m_file == nullptr) {
    ATH_MSG_ERROR("Unable to open output file at " << m_filePath);
    return StatusCode::FAILURE;
  }
  m_file->cd();

  m_params = new TTree("parameters", "parameters");
  m_params->Branch("run_number", &m_run_number, "run_number/I");
  m_params->Branch("event_number", &m_event_number, "event_number/I");
  m_params->Branch("x", &m_x, "x/D");
  m_params->Branch("y", &m_y, "y/D");
  m_params->Branch("z", &m_z, "z/D");
  m_params->Branch("px", &m_px, "px/D");
  m_params->Branch("py", &m_py, "py/D");
  m_params->Branch("pz", &m_pz, "pz/D");

  m_meas = new TTree("measurements", "measurements");
  m_meas->Branch("run_number", &m_run_number, "run_number/I");
  m_meas->Branch("event_number", &m_event_number, "event_number/I");
  m_meas->Branch("station", &m_station, "station/I");
  m_meas->Branch("layer", &m_layer, "layer/I");
  m_meas->Branch("phi", &m_phi, "phi/I");
  m_meas->Branch("eta", &m_eta, "eta/I");
  m_meas->Branch("side", &m_side, "side/I");
  m_meas->Branch("meas_eLOC0", &m_meas_eLOC0, "meas_eLOC0/D");
  m_meas->Branch("meas_eLOC1", &m_meas_eLOC1, "meas_eLOC1/D");

  return StatusCode::SUCCESS;
}


StatusCode ProtoTrackWriterTool::finalize() {
  m_file->cd();
  m_params->Write();
  m_meas->Write();
  return StatusCode::SUCCESS;
}


StatusCode ProtoTrackWriterTool::write(
    std::shared_ptr<const Acts::CurvilinearTrackParameters> protoTrackParameters,
    std::shared_ptr<std::vector<Measurement>> measurements,
    const Acts::GeometryContext& geoctx) const {

  EventContext ctx = Gaudi::Hive::currentContext();
  m_run_number = ctx.eventID().run_number();
  m_event_number = ctx.eventID().event_number();

  using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;
  std::shared_ptr<IdentifierMap> identifierMap = m_trackingGeometryTool->getIdentifierMap();
  // flip key-value pairs of identifier map
  std::map<Acts::GeometryIdentifier, Identifier> geoIdentifierMap;
  for (const std::pair<const Identifier, Acts::GeometryIdentifier>& identifierPair : *identifierMap) {
    geoIdentifierMap[identifierPair.second] = identifierPair.first;
  }

  Acts::Vector3 position = protoTrackParameters->position(geoctx);
  Acts::Vector3 direction = protoTrackParameters->momentum();
  m_x = position.x();
  m_y = position.y();
  m_z = position.z();
  m_px = direction.x();
  m_py = direction.y();
  m_pz = direction.z();
  m_params->Fill();

  for (const Measurement& variantMeasurement : *measurements) {
    // auto meas = std::get<Acts::Measurement<IndexSourceLink, Acts::BoundIndices, 2>>(variantMeasurement);
    std::visit([&](const auto& meas) {
      Acts::GeometryIdentifier geoId = meas.sourceLink().geometryId();
      Identifier id = geoIdentifierMap.at(geoId);
      m_station = m_idHelper->station(id);
      m_layer = m_idHelper->layer(id);
      m_phi = m_idHelper->phi_module(id);
      m_eta = m_idHelper->eta_module(id);
      m_side = m_idHelper->side(id);
      auto params = meas.parameters();
      m_meas_eLOC0 = params[Acts::eBoundLoc0];
      // write out as many variables as there are
      // m_meas_eLOC1 = params[Acts::eBoundLoc1];
      m_meas->Fill();
    }, variantMeasurement);
  };

  return StatusCode::SUCCESS;
}