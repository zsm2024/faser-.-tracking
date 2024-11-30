/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERBYTESTREAMINPUTSVC_H
#define FASERBYTESTREAMINPUTSVC_H

/** @file FaserByteStreaInputSvc.h
 *  @brief This file contains the class definition for the FaserByteStreamInputSvc class.
 *  @author Eric Torrence <torrence@uoregon.edu>
 **/

// Originally copied from ByteStreamEventStorageInputSvc

// Include files.
#include "FaserByteStreamCnvSvcBase/IFaserROBDataProviderSvc.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "FaserEventStorage/DataReader.h"

// FrameWork includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

#include "EventFormats/DAQFormats.hpp"
#include <exception>

class StoreGateSvc;
class DataHeaderElement;

/** @class ByteStreamFaserInputSvc
 *  @brief This class is the ByteStreamInputSvc for reading events written by Faser.
 **/
class FaserByteStreamInputSvc : public ::AthService {

public:
   /// Constructors:
   FaserByteStreamInputSvc(const std::string& name, ISvcLocator* svcloc);

   /// Destructor.
   virtual ~FaserByteStreamInputSvc();

   /// Retrieve interface ID
   static const InterfaceID& interfaceID();

   /// Required of all Gaudi Services
   virtual StatusCode initialize();
   virtual StatusCode stop();
   virtual StatusCode finalize();

   /// Required of all Gaudi services:  see Gaudi documentation for details
   StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// Implementation of the ByteStreamInputSvc interface methods.
   virtual const DAQFormats::EventFull* currentEvent() const;
   virtual const DAQFormats::EventFull* nextEvent();            //!< ++, new
   virtual const DAQFormats::EventFull* previousEvent();        //!< --, old
   virtual void setEvent(void* data, unsigned int eventStatus);

   /// Return the current event status
   virtual unsigned int currentEventStatus() const;
   virtual void validateEvent(); 

   virtual long positionInBlock();
   virtual std::pair<long,std::string> getBlockIterator(const std::string fileName);
   void         closeBlockIterator(bool clearMetadata=true);
   bool         setSequentialRead();
   bool         ready() const;
   StatusCode   generateDataHeader();

private: // data
   std::mutex m_readerMutex;

   struct EventCache {
     std::unique_ptr<DAQFormats::EventFull> rawEvent = NULL;
     unsigned int       eventStatus = 0;   //!< check_tree() status of the current event
     long long int      eventOffset = 0;   //!< event offset within a file, can be -1          
     void               releaseEvent(); //!< deletes event
     virtual            ~EventCache();  //!< calls releaseEvent
   };

   SG::SlotSpecificObj<EventCache> m_eventsCache;
   std::unique_ptr<FaserEventStorage::DataReader>  m_reader; //!< DataReader from EventStorage

   mutable std::vector<long long int> m_evtOffsets;  //!< offset for event i in that file
   unsigned int                 m_evtInFile;
   long long int      m_evtFileOffset = 0;   //!< last read in event offset within a file, can be -1     
   // Event back navigation info
   std::string        m_fileGUID;      //!< current file GUID


private: // properties
   ServiceHandle<StoreGateSvc>                m_storeGate;     //!< StoreGateSvc
   //ServiceHandle<StoreGateSvc>                m_inputMetadata; //!< StoreGateSvc
   ServiceHandle<IFaserROBDataProviderSvc>         m_robProvider;


   Gaudi::Property<bool>                      m_sequential;    //!< enable sequential reading.
   Gaudi::Property<bool>                      m_dump;
   Gaudi::Property<float>                     m_wait;
   Gaudi::Property<bool>                      m_valEvent;
   Gaudi::Property<std::string>               m_eventInfoKey;

private: // internal helper functions

   // void buildFragment( EventCache* cache, void* data, uint32_t eventSize, bool validate ) const;
   void buildEvent( EventCache* cache, DAQFormats::EventFull* theEvent, bool validate ) const;
   bool readerReady() const;
   unsigned validateEvent( const DAQFormats::EventFull* rawEvent ) const;
   void setEvent( const EventContext& context, void* data, unsigned int eventStatus );
   
   enum Advance{ PREVIOUS = -1, NEXT = 1 };
   const DAQFormats::EventFull* getEvent( Advance step );
   std::unique_ptr<DataHeaderElement> makeBSProvenance() const;


   template<typename T>
     StatusCode deleteEntry(const std::string& key)
     {
       if (m_storeGate->contains<T>(key)) {
	 const T* tmp = m_storeGate->tryConstRetrieve<T>(key);
	 if (tmp != nullptr) ATH_CHECK(m_storeGate->remove<T>(tmp));
       }
       return StatusCode::SUCCESS;
     }

};

#endif
