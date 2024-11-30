#include "SimWriterTool.h"

#include "TFile.h"
#include "TTree.h"

SimWriterTool::SimWriterTool(const std::string& type,
   const std::string& name, const IInterface* parent)
   : AthAlgTool(type, name, parent) {}


StatusCode SimWriterTool::initialize() {
  ATH_MSG_INFO("SimWriterTool::initialize");

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

  m_tree->Branch("event_number", &m_eventNumber);
  m_tree->Branch("x", &m_x);
  m_tree->Branch("y", &m_y);
  m_tree->Branch("z", &m_z);
  m_tree->Branch("px", &m_px);
  m_tree->Branch("py", &m_py);
  m_tree->Branch("pz", &m_pz);
  
  return StatusCode::SUCCESS;
}


StatusCode SimWriterTool::finalize() {
  ATH_MSG_INFO("finalize SimWriterTool::finalize");
  m_file->cd();
  m_tree->Write();
  return StatusCode::SUCCESS;
}


void SimWriterTool::writeout(Acts::Vector3 vertex, Acts::Vector3 momentum) const {
  m_eventNumber = Gaudi::Hive::currentContext().eventID().event_number();
  m_x = vertex.x();
  m_y = vertex.y();
  m_z = vertex.z();
  m_px = momentum.x();
  m_py = momentum.y();
  m_pz = momentum.z();
  m_tree->Fill();
}
