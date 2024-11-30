/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

/*
 * FaserTriggerDataAccess.cxx
 * 
 * Simple algorithm to access TLB data from storegate.
 * Try to write a proper thread-safe algorithm.
 *
 */

#include "FaserTriggerDataAccess.h"

FaserTriggerDataAccess::FaserTriggerDataAccess(const std::string& name, ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator)
{
}

FaserTriggerDataAccess::~FaserTriggerDataAccess()
{
}

StatusCode
FaserTriggerDataAccess::initialize()
{
  ATH_MSG_DEBUG("FaserTriggerDataAccess::initialize() called");

  // Must initialize SG handles
  ATH_CHECK( m_FaserTriggerData.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode
FaserTriggerDataAccess::finalize()
{
  ATH_MSG_DEBUG("FaserTriggerDataAccess::finalize() called");

  return StatusCode::SUCCESS;
}

StatusCode
FaserTriggerDataAccess::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("FaserTriggerDataAccess::execute() called");

  ATH_MSG_DEBUG("Looking for FaserTriggerData object at " + m_FaserTriggerData.key());

  SG::ReadHandle<xAOD::FaserTriggerData> xaod(m_FaserTriggerData, ctx);
  ATH_MSG_INFO("Found FaserTriggerData");
  ATH_MSG_INFO(*xaod);

  return StatusCode::SUCCESS;
}
