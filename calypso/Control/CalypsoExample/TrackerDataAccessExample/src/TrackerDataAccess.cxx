/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

/*
 * TrackerDataAccess.cxx
 * 
 * Simple algorithm to access Tracker RDO data from storegate.
 * Try to write a proper thread-safe algorithm.
 *
 */

#include "TrackerDataAccess.h"

TrackerDataAccess::TrackerDataAccess(const std::string& name, ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator)
{
}

TrackerDataAccess::~TrackerDataAccess()
{
}

StatusCode
TrackerDataAccess::initialize()
{
  ATH_MSG_DEBUG("TrackerDataAccess::initialize() called");

  // Must initialize SG handles
  ATH_CHECK( m_TrackerContainer_HITMODE.initialize() );
  ATH_CHECK( m_TrackerContainer_LEVELMODE.initialize() );
  ATH_CHECK( m_TrackerContainer_EDGEMODE.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode
TrackerDataAccess::finalize()
{
  ATH_MSG_DEBUG("TrackerDataAccess::finalize() called");
  ATH_MSG_ALWAYS("Found " << m_goodEvents << " good events out of " << m_events << " total events.");
  ATH_MSG_ALWAYS("Found " << m_hitModeHits << " HIT mode hits, " <<
                             m_levelModeHits << " LEVEL mode hits, and " << 
                             m_edgeModeHits << " EDGE mode hits");

  return StatusCode::SUCCESS;
}

StatusCode
TrackerDataAccess::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("TrackerDataAccess::execute() called");
  
  m_events++;
  // Try reading the data in the container

  ATH_CHECK(checkContainer(ctx, m_TrackerContainer_HITMODE, m_hitModeHits));
  ATH_CHECK(checkContainer(ctx, m_TrackerContainer_LEVELMODE, m_levelModeHits));
  ATH_CHECK(checkContainer(ctx, m_TrackerContainer_EDGEMODE, m_edgeModeHits));

  m_goodEvents++;

  return StatusCode::SUCCESS;
}

StatusCode
TrackerDataAccess::checkContainer(const EventContext& ctx, const SG::ReadHandleKey<FaserSCT_RDO_Container>& handleKey, std::atomic<size_t>& counter) const
{
  SG::ReadHandle<FaserSCT_RDO_Container> hitContainerHandle(handleKey, ctx);
  if (hitContainerHandle.isValid())
  {
    ATH_MSG_INFO("Found ReadHandle for container" << hitContainerHandle.key() << ", size=" << hitContainerHandle->size());
    // Try printing out each collection stored in the container
    // Works if operator<< is defined for the collection (and it should be)
    for(auto coll: *hitContainerHandle)
      ATH_MSG_INFO(*coll);  

    for (auto collection : *hitContainerHandle)
    {
      counter += collection->size();
    }
  }
  else
  {
    ATH_MSG_ERROR("Unable to retrieve " << hitContainerHandle.key());
    return StatusCode::RECOVERABLE;
  }
  return StatusCode::SUCCESS;
}