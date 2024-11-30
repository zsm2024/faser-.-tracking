/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserByteStreamCnvSvc/FaserByteStreamInputSvc.h"

#include "DumpFrags.h"

//#include "ByteStreamData/ByteStreamMetadataContainer.h"
#include "FaserByteStreamCnvSvcBase/FaserByteStreamAddress.h"
#include "FaserEventStorage/pickFaserDataReader.h"
#include "FaserEventStorage/EventStorageIssues.h"
#include "Gaudi/Property.h"

#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModel/Token.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

// std::ostream& operator<<(std::ostream&, const DAQFormats::EventFull&);
// std::ostream& operator<<(std::ostream&, const DAQFormats::EventFragment&);
#include "EventFormats/DAQFormats.hpp"

// Don't know what these do, comment for now
// #include "eformat/HeaderMarker.h"
// #include "eformat/SourceIdentifier.h"
// #include "eformat/Issue.h"
// #include "eformat/Problem.h"
// #include "eformat/Version.h"
// #include "eformat/Status.h"
// #include "eformat/old/util.h"

#include <cstdio>
#include <string>
#include <vector>
#include <unistd.h>

using DAQFormats::EventFull;

// Constructor.
FaserByteStreamInputSvc::FaserByteStreamInputSvc(
       const std::string& name, ISvcLocator* svcloc) 
  : AthService(name, svcloc)
  , m_readerMutex()
  , m_eventsCache()
  , m_reader()
  , m_evtOffsets()
  , m_evtInFile(0)
  , m_evtFileOffset(0)
  , m_fileGUID("")
  , m_storeGate    ("StoreGateSvc", name)
    //, m_inputMetadata("StoreGateSvc/InputMetaDataStore", name)
  , m_robProvider  ("FaserROBDataProviderSvc", name)
  , m_sequential   (this, "EnableSequential",   false, "")
  , m_dump         (this, "DumpFlag",           false, "Dump fragments")
  , m_wait         (this, "WaitSecs",              0., "Seconds to wait if input is in wait state")
  , m_valEvent     (this, "ValidateEvent",       true, "switch on check_tree when reading events")
  , m_eventInfoKey (this, "EventInfoKey", "EventInfo", "Key of EventInfo in metadata store")

{
  assert(svcloc != nullptr);

  declareProperty("EventStore",    m_storeGate);
  //declareProperty("MetaDataStore", m_inputMetadata);
}
//------------------------------------------------------------------------------
FaserByteStreamInputSvc::~FaserByteStreamInputSvc() {  
}
//------------------------------------------------------------------------------
StatusCode FaserByteStreamInputSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() );

   ATH_CHECK(m_storeGate.retrieve());
   ATH_CHECK(m_robProvider.retrieve());

   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode FaserByteStreamInputSvc::stop() {
   ATH_MSG_DEBUG("Calling FaserByteStreamInputSvc::stop()");
   // close moved to EventSelector for explicit coupling with incident
   return(StatusCode::SUCCESS);
}
//------------------------------------------------------------------------------
StatusCode FaserByteStreamInputSvc::finalize() {

  ATH_CHECK(m_storeGate.release());
  ATH_CHECK(m_robProvider.release()); 

  return(StatusCode::SUCCESS);
}
//------------------------------------------------------------------------------
long FaserByteStreamInputSvc::positionInBlock()
{
  return m_evtOffsets.size();
}

const EventFull* FaserByteStreamInputSvc::previousEvent() {
  ATH_MSG_WARNING("previousEvent called, but not implemented!");
  return NULL;
}
//------------------------------------------------------------------------------
// Read the next event.
const EventFull* FaserByteStreamInputSvc::nextEvent() {

  ATH_MSG_DEBUG("FaserByteStreamInputSvc::nextEvent() called");

  std::lock_guard<std::mutex> lock( m_readerMutex );
  const EventContext context{ Gaudi::Hive::currentContext() };

  // Load event from file
  EventFull* theEvent=0;

  if (readerReady()) {

    m_evtInFile ++; // increment iterator

    if (m_evtInFile+1 > m_evtOffsets.size()) { 
      // get current event position (cast to long long until native tdaq implementation)
      // This is the usual situation, reading past previous point in file
      ATH_MSG_DEBUG("nextEvent _above_ high water mark");
      m_evtFileOffset = (long long)m_reader->getPosition();
      m_evtOffsets.push_back(m_evtFileOffset);

      // Catch truncated events
      try {
	m_reader->getData(theEvent);
      } catch (const FaserEventStorage::ES_OutOfFileBoundary& e) {
	// Attempt to read beyond end of file, likely truncated event
	ATH_MSG_WARNING("DataReader reports FaserEventStorage::ES_OutOfFileBoundary, stop reading file!");
	return NULL;
      } catch (const DAQFormats::EFormatException& e) {
	// Format error
	ATH_MSG_WARNING("DataReader reports DAQFormats::EFormatException, stop reading file!");
	ATH_MSG_WARNING(e.what());
	return NULL;
      } catch (...) {
	// rethrow any other exceptions
	throw;
      }
    }

    else {
      // Load from previous offset
      ATH_MSG_DEBUG("nextEvent below high water mark");
      m_evtFileOffset = m_evtOffsets.at( m_evtInFile-1 );
      m_reader->getData( theEvent, m_evtFileOffset );
    }

    ATH_MSG_DEBUG("Read Event:\n" << *theEvent);
  }
  else {
    ATH_MSG_ERROR("DataReader not ready. Need to getBlockIterator first");
    return 0;
  }

  
  EventCache* cache = m_eventsCache.get(context);

  // initialize before building RawEvent
  cache->releaseEvent(); 
   
  // Use buffer to build FullEventFragment
  try {
    buildEvent( cache,  theEvent, true );
  }
  catch (...) {
    // rethrow any exceptions
    throw;
  }

  if ( cache->rawEvent == NULL ) {
    ATH_MSG_ERROR("Failure to build fragment");
    return NULL;
  }

  // Set it for the data provider
  ATH_MSG_DEBUG( "call robProvider->setNextEvent ");
  m_robProvider->setNextEvent(context, cache->rawEvent.get() );
  m_robProvider->setEventStatus(context, cache->eventStatus );

  // dump
  if (m_dump) {
    DumpFrags::dump(cache->rawEvent.get());
  }

  ATH_MSG_DEBUG( "switched to next event in slot " << context );
  return( cache->rawEvent.get() );

}

void FaserByteStreamInputSvc::validateEvent() {
  const EventContext& context{ Gaudi::Hive::currentContext() };
  const EventFull* const event = m_eventsCache.get(context)->rawEvent.get();
  m_eventsCache.get(context)->eventStatus = validateEvent( event );
}

unsigned FaserByteStreamInputSvc::validateEvent( const EventFull* const rawEvent ) const
{
  unsigned int status = 0;
  if (m_valEvent) {
    ATH_MSG_WARNING("validateEvent called, but not implemented");
    ATH_MSG_INFO("event:\n" << *rawEvent);
    /*
    // check validity
    std::vector<eformat::FragmentProblem> p;
    rawEvent->problems(p);
    if (!p.empty()) {
      status += 0x01000000;
      // bad event
      ATH_MSG_WARNING("Failed to create FullEventFragment");
      for (std::vector<eformat::FragmentProblem>::const_iterator i = p.begin(), iEnd = p.end();
	        i != iEnd; i++) {
        ATH_MSG_WARNING(eformat::helper::FragmentProblemDictionary.string(*i));
      }
      //      releaseCurrentEvent();
      throw ByteStreamExceptions::badFragmentData();
    }
    if ( !ROBFragmentCheck( rawEvent ) ) {
      status += 0x02000000;
      // bad event
      //      releaseCurrentEvent();
      ATH_MSG_ERROR("Skipping bad event");
      throw ByteStreamExceptions::badFragmentData();
    }
    */
  } 
  else {
    ATH_MSG_DEBUG("Processing event without validating.");
  }
  return status;
}

void FaserByteStreamInputSvc::buildEvent(EventCache* cache, EventFull* theEvent, bool validate) const
{
  ATH_MSG_DEBUG("FaserByteStreamInputSvc::buildEvent() called");

  if (validate) {
    // Nothing to do
  }

  cache->eventStatus = 0;
  //cache->rawEvent = std::make_unique<EventFull>();
  cache->rawEvent.reset(theEvent);
}

//__________________________________________________________________________
StatusCode FaserByteStreamInputSvc::generateDataHeader()
{
  ATH_MSG_DEBUG("FaserByteStreamInputSvc::generateDataHeader() called");

  // get file GUID
  m_fileGUID = m_reader->GUID();

  // reader returns -1 when end of the file is reached
  if (m_evtFileOffset != -1) {
    ATH_MSG_DEBUG("ByteStream File GUID:" << m_fileGUID);
    ATH_MSG_DEBUG("ByteStream Event Position in File: " << m_evtFileOffset);

    // To accomodate for skipEvents option in EventSelector
    // While skipping BS event Selector does not return SUCCESS code,
    // just advances silently through events. So SG content is not refreshed
    // Lets do refresh of the event header here
    std::string key = "ByteStreamDataHeader";
    ATH_CHECK(deleteEntry<DataHeader>(key));


    // Created data header element with BS provenance information
    std::unique_ptr<DataHeaderElement> dataHeaderElement = makeBSProvenance();
    // Create data header itself
    std::unique_ptr<DataHeader> dataHeader = std::make_unique<DataHeader>();
    // Declare header primary
    dataHeader->setStatus(DataHeader::Input);
    //add the data header elenebt self reference to the object vector
    dataHeader->insert(*std::move(dataHeaderElement));


    // Clean up EventInfo from the previous event
    key = m_eventInfoKey.value();
    ATH_CHECK(deleteEntry<xAOD::EventInfo>(key));
    // Now add ref to xAOD::EventInfo
    std::unique_ptr<IOpaqueAddress> iopx = std::make_unique<FaserByteStreamAddress>(
									       ClassID_traits<xAOD::EventInfo>::ID(), key, "");
    ATH_CHECK(m_storeGate->recordAddress(key, iopx.release()));
    const SG::DataProxy* ptmpx = m_storeGate->transientProxy(
							     ClassID_traits<xAOD::EventInfo>::ID(), key);
    if (ptmpx != nullptr) {
      DataHeaderElement element(ptmpx, 0, key);
      dataHeader->insert(element);
    }

    // Clean up auxiliary EventInfo from the previous event
    key = m_eventInfoKey.value() + "Aux.";
    ATH_CHECK(deleteEntry<xAOD::EventAuxInfo>(key));
    // Now add ref to xAOD::EventAuxInfo
    std::unique_ptr<IOpaqueAddress> iopaux = std::make_unique<FaserByteStreamAddress>(
										 ClassID_traits<xAOD::EventAuxInfo>::ID(), key, "");
    ATH_CHECK(m_storeGate->recordAddress(key, iopaux.release()));
    const SG::DataProxy* ptmpaux = m_storeGate->transientProxy(
							       ClassID_traits<xAOD::EventAuxInfo>::ID(), key);
    if (ptmpaux !=0) {
      DataHeaderElement element(ptmpaux, 0, key);
      dataHeader->insert(element);
    }

    // Record new data header.Boolean flags will allow it's deletionin case
    // of skipped events.
    ATH_CHECK(m_storeGate->record<DataHeader>(dataHeader.release(),
					      "ByteStreamDataHeader", true, false, true));
  }
  return StatusCode::SUCCESS;
}

/******************************************************************************/
void
FaserByteStreamInputSvc::EventCache::releaseEvent()
{
  // cleanup parts of previous event and re-init them
  if(rawEvent) {
    rawEvent.reset(nullptr);
    eventStatus = 0;
  }
}

/******************************************************************************/
FaserByteStreamInputSvc::EventCache::~EventCache()
{
  releaseEvent();
}



/******************************************************************************/
void
FaserByteStreamInputSvc::closeBlockIterator(bool clearMetadata)
{
  if (clearMetadata) {
    ATH_MSG_WARNING("Clearing input metadata store");
    // StatusCode status = m_inputMetadata->clearStore();
    // if (!status.isSuccess()) {
    //   ATH_MSG_WARNING("Unable to clear Input MetaData Proxies");
    // }
  }

  if (!readerReady()) {
    ATH_MSG_INFO("No more events in this run, high water mark for this file = "
		 << m_evtOffsets.size()-1);
  }

  m_reader.reset();
}


/******************************************************************************/
bool
FaserByteStreamInputSvc::setSequentialRead()
{
  // enable SequenceReading
  m_reader->enableSequenceReading();
  return true;
}

/******************************************************************************/
bool
FaserByteStreamInputSvc::ready() const
{
  return readerReady();
}


/******************************************************************************/
std::pair<long,std::string>
FaserByteStreamInputSvc::getBlockIterator(const std::string fileName)
{
  // open the file
  if(m_reader != 0) closeBlockIterator();

  m_reader = std::unique_ptr<FaserEventStorage::DataReader>(pickFaserDataReader(fileName));

  if(m_reader == nullptr) {
    ATH_MSG_ERROR("Failed to open file " << fileName);
    closeBlockIterator();
    return std::make_pair(-1,"END"); 
  }

  // Initilaize offset vector
  m_evtOffsets.resize(m_reader->eventsInFile(), -1);
  m_evtOffsets.clear();

  m_evtInFile = 0;

  // enable sequentialReading if multiple files
  if(m_sequential) {
    bool test = setSequentialRead();
    if (!test) return std::make_pair(-1,"SEQ");
  }

  ATH_MSG_INFO("Picked valid file: " << m_reader->fileName());
  // initialize offsets and counters
  m_evtOffsets.push_back(static_cast<long long>(m_reader->getPosition()));
  return std::make_pair(m_reader->eventsInFile(), m_reader->GUID());
}


//__________________________________________________________________________
bool FaserByteStreamInputSvc::readerReady() const
{
  bool eofFlag(false);
  if (m_reader!=0) eofFlag = m_reader->endOfFile();
  else {
    ATH_MSG_INFO("eformat reader object not initialized");
    return false;
  }
  bool moreEvent = m_reader->good();
  
  return (!eofFlag)&&moreEvent;
}
//__________________________________________________________________________
void FaserByteStreamInputSvc::setEvent(void* data, unsigned int eventStatus) {
  const EventContext context{ Gaudi::Hive::currentContext() };
  return setEvent( context, data, eventStatus );
}

void FaserByteStreamInputSvc::setEvent( const EventContext& context, void* data, unsigned int eventStatus )
{
  ATH_MSG_DEBUG("FaserByteStreamInputSvc::setEvent() called");

  EventCache* cache = m_eventsCache.get( context );
  cache->releaseEvent();

  EventFull* event = reinterpret_cast<EventFull*>(data);
  cache->rawEvent.reset(event);
  cache->eventStatus = eventStatus;

  // Set it for the data provider
  m_robProvider->setNextEvent(context, cache->rawEvent.get());
  m_robProvider->setEventStatus(context, cache->eventStatus);

  // Build a DH for use by other components
  StatusCode rec_sg = generateDataHeader();
  if (rec_sg != StatusCode::SUCCESS) {
    ATH_MSG_ERROR("Fail to record BS DataHeader in StoreGate. Skipping events?! " << rec_sg);
  }
}

//__________________________________________________________________________
inline const InterfaceID& FaserByteStreamInputSvc::interfaceID() {
  /// Declaration of the interface ID ( interface id, major version, minor version)
  static const InterfaceID IID_ByteStreamInputSvc("ByteStreamInputSvc", 1, 0);
  return(IID_ByteStreamInputSvc);
}
//__________________________________________________________________________
const EventFull* FaserByteStreamInputSvc::currentEvent() const {
  const EventContext context{ Gaudi::Hive::currentContext() };
  return m_eventsCache.get(context)->rawEvent.get();
}
//__________________________________________________________________________
unsigned int FaserByteStreamInputSvc::currentEventStatus() const {
  const EventContext context{ Gaudi::Hive::currentContext() };
  return m_eventsCache.get(context)->eventStatus;
}
//________________________________________________________________________________
StatusCode FaserByteStreamInputSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (FaserByteStreamInputSvc::interfaceID().versionMatch(riid)) {
      *ppvInterface = dynamic_cast<FaserByteStreamInputSvc*>(this);
   } else {
     // Interface is not directly available: try out a base class
     return(::AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}


std::unique_ptr<DataHeaderElement>
FaserByteStreamInputSvc::makeBSProvenance() const
{
  std::unique_ptr<Token> token = std::make_unique<Token>();
  token->setDb(m_fileGUID);
  token->setTechnology(0x00001000);
  token->setOid(Token::OID_t(0LL, m_evtFileOffset));

  // note: passing ownership of token to DataHeaderElement
  return std::make_unique<DataHeaderElement>(ClassID_traits<DataHeader>::ID(),
					     "StreamRAW", token.release());
}

