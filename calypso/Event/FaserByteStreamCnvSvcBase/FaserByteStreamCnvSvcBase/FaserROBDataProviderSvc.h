/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERBYTESTREAMCNVSVCBASE_ROBDATAPROVIDERSVC_H
#define FASERBYTESTREAMCNVSVCBASE_ROBDATAPROVIDERSVC_H

/** ===============================================================
 * @class    ROBDataProviderSvc.h
 * @brief  ROBDataProvider class for accessing ROBData
 *
 *    Requirements: define a ROBData class in the scope
 *                  provide a method
 *       void getROBData(const vector<uint>& ids, vector<ROBData*>& v)
 *    Implementation: Use an interal map to store all ROBs
 *                    We can not assume any ROB/ROS relationship, no easy
 *                    way to search.
 *                    This implementation is used in offline
 *
 *    Created:      Sept 19, 2002
 *         By:      Hong Ma
 *    Modified:     Aug. 18  2003 (common class for Online/Offline)
 *         By:      Werner Wiedenmann
 *    Modified:     Apr  21  2005 (remove dependency on data flow repository)
 *         By:      Werner Wiedenmann
 */

#include "FaserByteStreamCnvSvcBase/IFaserROBDataProviderSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/SlotSpecificObj.h"

#include <vector>
#include <map>

namespace DAQFormats {
  class EventFull;
}

class FaserROBDataProviderSvc :  public extends<AthService, IFaserROBDataProviderSvc> {

public:
   /// ROB Fragment class
   //typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF;

   /// Constructor
   FaserROBDataProviderSvc(const std::string& name, ISvcLocator* svcloc);
   /// Destructor
   virtual ~FaserROBDataProviderSvc();

   /// initialize the service
   virtual StatusCode initialize() override;

   /// Gaudi queryInterface method.
   //   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// Add all ROBFragments of a RawEvent to cache
   virtual void setNextEvent(const DAQFormats::EventFull* re) override;

   /// Retrieve the whole event.
   virtual const DAQFormats::EventFull* getEvent() override;

   /// Store the status for the event.
   virtual void setEventStatus(uint32_t status) override;

   /// Retrieve the status for the event.
   virtual uint32_t getEventStatus() override;


   /// MT variants 

   virtual void setNextEvent(const EventContext& context, const DAQFormats::EventFull* re) override;
   virtual const DAQFormats::EventFull* getEvent(const EventContext& context) override;
   virtual void setEventStatus(const EventContext& context, uint32_t status) override;
   virtual uint32_t getEventStatus(const EventContext& context) override;

   virtual bool isEventComplete(const EventContext& /*context*/) const override { return true; }
   virtual int collectCompleteEventData(const EventContext& /*context*/, const std::string_view /*callerName*/ ) override {  return 0; }

protected:

  struct EventCache {
    ~EventCache();
    const DAQFormats::EventFull* event = 0;
    uint32_t eventStatus = 0;    
    uint32_t currentLvl1ID = 0;    
  };
  SG::SlotSpecificObj<EventCache> m_eventsCache;

};

#endif
