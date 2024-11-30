#include "TrackSeedWriterTool.h"
#include "TFile.h"
#include "TTree.h"

TrackSeedWriterTool::TrackSeedWriterTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
    : AthAlgTool( type, name, parent ) {}


StatusCode TrackSeedWriterTool::initialize() {
  ATH_MSG_INFO("TrackSeedWriterTool::initialize");

  std::string filePath = m_filePath;
  std::string treeName = m_treeName;

  m_file = TFile::Open(filePath.c_str(), "RECREATE");
  if (m_file == nullptr) {
    ATH_MSG_ERROR("Unable to open output file at " << m_filePath);
    return StatusCode::FAILURE;
  }
  m_file->cd();
  m_tree = new TTree(treeName.c_str(), "tree");
  if (m_tree == nullptr) {
    ATH_MSG_ERROR("Unable to create TTree");
    return StatusCode::FAILURE;
  }

  TrackSeedWriterTool::initializeTree();

  return StatusCode::SUCCESS;
}


StatusCode TrackSeedWriterTool::finalize() {
  m_file->cd();
  m_tree->Write();
  return StatusCode::SUCCESS;
}


void TrackSeedWriterTool::initializeTree() {
  m_tree->Branch("run_number", &m_runNumber);
  m_tree->Branch("event_number", &m_eventNumber);
  m_tree->Branch("eLOC0", &m_eLOC0);
  m_tree->Branch("eLOC1", &m_eLOC1);
  m_tree->Branch("ePHI", &m_ePHI);
  m_tree->Branch("eTHETA", &m_eTHETA);
  m_tree->Branch("eQOP", &m_eQOP);
  m_tree->Branch("err_eLOC0", &m_err_eLOC0);
  m_tree->Branch("err_eLOC1", &m_err_eLOC1);
  m_tree->Branch("err_ePHI", &m_err_ePHI);
  m_tree->Branch("err_eTHETA", &m_err_eTHETA);
  m_tree->Branch("err_eQOP", &m_err_eQOP);
  m_tree->Branch("x", &m_x);
  m_tree->Branch("y", &m_y);
  m_tree->Branch("z", &m_z);
  m_tree->Branch("px", &m_px);
  m_tree->Branch("py", &m_py);
  m_tree->Branch("pz", &m_pz);
}


void TrackSeedWriterTool::writeout(
    const Acts::GeometryContext& gctx,
    const std::shared_ptr<std::vector<Acts::CurvilinearTrackParameters>>& initialParametersContainer) const {
  m_runNumber = Gaudi::Hive::currentContext().eventID().run_number();
  m_eventNumber = Gaudi::Hive::currentContext().eventID().event_number();
  for (Acts::CurvilinearTrackParameters initialParameters : *initialParametersContainer) {
    const auto& parameters = initialParameters.parameters();
    const Acts::Vector3& center = initialParameters.referenceSurface().center(gctx);
    m_eLOC0.push_back(center[Acts::eBoundLoc0]);
    m_eLOC1.push_back(center[Acts::eBoundLoc1]);
    m_ePHI.push_back(parameters[Acts::eBoundPhi]);
    m_eTHETA.push_back(parameters[Acts::eBoundTheta]);
    m_eQOP.push_back(parameters[Acts::eBoundQOverP]);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

    const auto& covariance = *initialParameters.covariance();
    m_err_eLOC0.push_back(sqrt(covariance(Acts::eBoundLoc0, Acts::eBoundLoc0)));
    m_err_eLOC1.push_back(sqrt(covariance(Acts::eBoundLoc1, Acts::eBoundLoc1)));
    m_err_ePHI.push_back(sqrt(covariance(Acts::eBoundPhi, Acts::eBoundPhi)));
    m_err_eTHETA.push_back(sqrt(covariance(Acts::eBoundTheta, Acts::eBoundTheta)));
    m_err_eQOP.push_back(sqrt(covariance(Acts::eBoundQOverP, Acts::eBoundQOverP)));

#pragma GCC diagnostic pop

    const auto& position = initialParameters.position(gctx);
    const auto& momentum = initialParameters.momentum();
    m_x.push_back(position.x());
    m_y.push_back(position.y());
    m_z.push_back(position.z());
    m_px.push_back(momentum.x());
    m_py.push_back(momentum.y());
    m_pz.push_back(momentum.z());
  }
  m_tree->Fill();
  clearVariables();
}


void TrackSeedWriterTool::clearVariables() const {
  m_eLOC0.clear();
  m_eLOC1.clear();
  m_ePHI.clear();
  m_eTHETA.clear();
  m_eQOP.clear();
  m_err_eLOC0.clear();
  m_err_eLOC1.clear();
  m_err_ePHI.clear();
  m_err_eTHETA.clear();
  m_err_eQOP.clear();
  m_x.clear();
  m_y.clear();
  m_z.clear();
  m_px.clear();
  m_py.clear();
  m_pz.clear();
}
