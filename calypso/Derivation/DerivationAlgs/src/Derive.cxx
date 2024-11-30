#include "Derive.h"


Derive::Derive(const std::string& name, 
					 ISvcLocator* pSvcLocator)
  : AthFilterAlgorithm(name, pSvcLocator) { 

  //declareProperty("Tools", m_tools);

}

StatusCode 
Derive::initialize() {
  ATH_MSG_INFO(name() << "::initalize()" );

  ATH_CHECK( m_tools.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode 
Derive::finalize() {
  ATH_MSG_INFO(name() << "::finalize()");
  ATH_MSG_INFO("Derivation" << name() << " accepted " << m_passed << " out of " << m_events << " events");

  return StatusCode::SUCCESS;
}

StatusCode 
Derive::execute() {
  ATH_MSG_DEBUG("Executing ... ");

  m_events++;

  bool acceptEvent(true);  

  for (auto& tool : m_tools) {
    
    // Skimming - remove events
    if (!tool->passed()) acceptEvent = false;

    // Thinning - remove info from event
    ATH_CHECK(tool->removeBranch());

    // Augmenting - add info to an event
    ATH_CHECK(tool->addBranch());

  }

  setFilterPassed(acceptEvent);
  if (acceptEvent) m_passed++;

  return StatusCode::SUCCESS;
}
