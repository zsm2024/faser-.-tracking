#include "SeedingAlg.h"

SeedingAlg::SeedingAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator) {}


StatusCode SeedingAlg::initialize() {
  ATH_CHECK(m_trackSeedTool.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode SeedingAlg::execute() {
  // const EventContext& ctx = Gaudi::Hive::currentContext();
  ATH_CHECK(m_trackSeedTool->run());
  return StatusCode::SUCCESS;
}


StatusCode SeedingAlg::finalize() {
  return StatusCode::SUCCESS;
}
