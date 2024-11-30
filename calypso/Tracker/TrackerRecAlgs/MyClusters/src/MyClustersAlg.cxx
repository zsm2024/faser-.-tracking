#include "MyClustersAlg.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerIdentifier/FaserSCT_ID.h"


MyClustersAlg::MyClustersAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator), AthHistogramming(name),
      m_histSvc("THistSvc/THistSvc", name) {}


StatusCode MyClustersAlg::initialize() {
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));

  ATH_CHECK(m_clusterContainerKey.initialize());

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
  m_tree->Branch("lx", &m_lx, "lx/D");
  m_tree->Branch("ly", &m_ly, "ly/D");
  ATH_CHECK(histSvc()->regTree("/HIST/clusters", m_tree));
  return StatusCode::SUCCESS;
}


StatusCode MyClustersAlg::execute(const EventContext& ctx) const {
  m_run = ctx.eventID().run_number();
  m_event = ctx.eventID().event_number();

  SG::ReadHandle<Tracker::FaserSCT_ClusterContainer> clusterContainer {m_clusterContainerKey, ctx};
  ATH_CHECK(clusterContainer.isValid());

  for (const auto& clusterCollection : *clusterContainer) {
    for (const auto& cluster : *clusterCollection) {
      auto id = cluster->identify();
      m_station = m_idHelper->station(id);
      m_layer = m_idHelper->layer(id);
      m_eta = m_idHelper->eta_module(id);
      m_phi = m_idHelper->phi_module(id);
      m_side = m_idHelper->side(id);
      auto localPos = cluster->localPosition();
      m_lx = localPos.x();
      m_ly = localPos.y();
      auto localCov = cluster->localCovariance();
      auto pos = cluster->globalPosition();
      m_x = pos.x();
      m_y = pos.y();
      m_z = pos.z();
      m_tree->Fill();
      ATH_MSG_DEBUG("global position\n" << pos);
      ATH_MSG_DEBUG("local position\n" << localPos);
      ATH_MSG_DEBUG("local covariance\n" << localCov);
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode MyClustersAlg::finalize() {
  return StatusCode::SUCCESS;
}
