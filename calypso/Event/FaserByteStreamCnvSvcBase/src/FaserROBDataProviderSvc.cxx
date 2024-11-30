/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//===================================================================
//  Implementation of ROBDataProviderSvc
//  Revision: November 2017
//      MT readiness
//  Revision:  July 11, 2002
//      Modified for eformat
//  Revision:  Aug 18, 2003
//      Modified to use ROBFragments directly and include methods
//      for online
//  Revision:  Apr 21, 2005
//      Remove dependency on Level-2 Data Collector, create special
//      version for online
//  Revision:  Oct 29, 2006
//      Increase MAX_ROBFRAGMENTS to 2048 to cover the special case
//      when in a "localhost" partition the complete event is given
//      to the Event Filter as one single ROS fragment (this case
//      should not happen for normal running when several ROSes are
//      used and the ROB fragments are grouped under different ROS
//      fragments)
//  Revision:  Nov 10, 2008
//      Mask off the module ID from the ROB source identifier of the
//      L2 and EF result when storing it in the ROB map. This is necessary
//      when the L2/PT node ID is stored in the source identifier as
//      module ID. With this modification the L2 and EF result can still be
//      found as 0x7b0000 and 0x7c0000
//  Revision:  Jan 12, 2009
//      Allow removal of individual ROBs and ROBs from given subdetectors
//      from the internal ROB map according to a given status code.
//      This may be necessary when corrupted and incomplete ROB fragments
//      are forwarded to the algorithms and the converters are not yet
//      prepared to handle the specific cases.
//      The filtering can be configured with job options as:
//
//      for individual ROBs as :
//      ------------------------
//      ROBDataProviderSvc.filterRobWithStatus = [ (ROB SourceId, StatusCode to remove),
//                                                 (ROB SourceId 2, StatusCode to remove 2), ... ]
//      and:
//      ROBDataProviderSvc.filterRobWithStatus += [ (ROB SourceId n, StatusCode to remove n) ]
//
//      Example:
//      ROBDataProviderSvc.filterRobWithStatus  = [ (0x42002a,0x0000000f), (0x42002e,0x00000008) ]
//      ROBDataProviderSvc.filterRobWithStatus += [ (0x42002b,0x00000000) ]
//
//      for all ROBs of a given sub detector as :
//      -----------------------------------------
//      ROBDataProviderSvc.filterSubDetWithStatus = [ (Sub Det Id, StatusCode to remove),
//                                                    (Sub Det Id 2, StatusCode to remove 2), ... ]
//      and:
//      ROBDataProviderSvc.filterSubDetWithStatus += [ (Sub Det Id n, StatusCode to remove n) ]
//
//      Example:
//      ROBDataProviderSvc.filterSubDetWithStatus  = [ (0x41,0x00000000), (0x42,0x00000000) ]
//      ROBDataProviderSvc.filterSubDetWithStatus += [ (0x41,0xcb0002) ]
//
//      For valid ROB Source Ids, Sub Det Ids and ROB Status elements see the event format
//      document ATL-D-ES-0019 (EDMS)
//  Revision:  Jan 28, 2014
//      For Run 1 the module ID from the ROB source identifier of the
//      L2 and EF result needed to be masked off before storing the ROB fragment 
//      in the ROB map. The module ID for these fragments contained an identifier 
//      of the machine on which they were produced. This produced as many different ROB IDs
//      for these fragments as HLT processors were used. The module IDs were not useful 
//      for analysis and needed to be masked off from these fragments in the ROB map in order 
//      to allow the access to the L2 or the EF result with the generic identifiers  
//      0x7b0000 and 0x7c0000. Also an event contained only one L2 and EF result.
//      From Run 2 on (eformat version 5) the HLT processor identifier is not anymore
//      stored in the module ID of the HLT result. Further there can be in one event several HLT result
//      records with the source identifier 0x7c. The different HLT results are distinguished
//      now with the module ID. A module ID 0 indicates a physiscs HLT result as before, while
//      HLT results with module IDs different from zero are produced by data scouting chains. 
//      In Run 2 the module ID should be therefore not any more masked.
//      The masking of the moduleID is switched on when a L2 result is found in the event or the
//      event header contains L2 trigger info words. This means the data were produced with run 1 HLT system.
//
//===================================================================

// Include files.
#include "FaserByteStreamCnvSvcBase/FaserROBDataProviderSvc.h"
#include "EventFormats/DAQFormats.hpp"

using DAQFormats::EventFull;

// Constructor.
FaserROBDataProviderSvc::FaserROBDataProviderSvc(const std::string& name, ISvcLocator* svcloc) 
  : base_class(name, svcloc) {

}

// Destructor.
FaserROBDataProviderSvc::~FaserROBDataProviderSvc() {
  // the eventsCache take care of cleaaning itself
}

// Initialization
StatusCode FaserROBDataProviderSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name());
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }
   m_eventsCache = SG::SlotSpecificObj<EventCache>( SG::getNSlots() );

   return(StatusCode::SUCCESS);
}


// - add a new Raw event
void FaserROBDataProviderSvc::setNextEvent(const EventFull* re) {
  // obtain context and redirect to the real implementation
  const EventContext context{ Gaudi::Hive::currentContext() };
  return setNextEvent( context, re );
}

void FaserROBDataProviderSvc::setNextEvent( const EventContext& context, const EventFull* re ) {

  EventCache* cache = m_eventsCache.get( context );
  
  cache->event = re;

  ATH_MSG_DEBUG(" ---> setNextEvent offline for " << name() );
  ATH_MSG_DEBUG("      current LVL1 id   = " << cache->currentLvl1ID );
  
  return;
}

/// Retrieve the whole event.
const EventFull* FaserROBDataProviderSvc::getEvent() {
  const EventContext context{ Gaudi::Hive::currentContext() };
  return getEvent( context );
}

const EventFull* FaserROBDataProviderSvc::getEvent( const EventContext& context ) {
  return m_eventsCache.get( context )->event;
}


/// Set the status for the event.
void FaserROBDataProviderSvc::setEventStatus(uint32_t status) {
  const EventContext context{ Gaudi::Hive::currentContext() };
  setEventStatus( context, status );
}

void FaserROBDataProviderSvc::setEventStatus(const EventContext& context, uint32_t status) {
  m_eventsCache.get(context)->eventStatus = status;
}
/// Retrieve the status for the event.
uint32_t FaserROBDataProviderSvc::getEventStatus() {
  const EventContext context{ Gaudi::Hive::currentContext() };
  return getEventStatus( context );
}

uint32_t FaserROBDataProviderSvc::getEventStatus( const EventContext& context ) {
  return m_eventsCache.get( context )->eventStatus;
}


FaserROBDataProviderSvc::EventCache::~EventCache() {
  delete event;
  //FaserROBDataProviderSvc::robmapClear( robmap );
}

