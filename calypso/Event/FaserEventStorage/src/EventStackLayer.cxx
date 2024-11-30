#include "ers/ers.h"
#include <fstream>
#include <vector>
#include <stack>
#include "EventStackLayer.h"
#include "DataReaderController.h"
#include "FaserEventStorage/EventStorageIssues.h"

#include "EventFormats/DAQFormats.hpp"

EventStackLayer::EventStackLayer( fRead* fR )
{
  //by default, this just sets the fileReader
  ERS_DEBUG(2,"EventStackLayer gets fRead Object ");
  m_fR = fR;
  m_handleCont=false;
  m_handleOffs=false;
  m_responsibleMetaData=false;
  m_finished = false;
  m_error = false;
  m_caller = NULL;
}

bool EventStackLayer::finished()
{
  return m_finished;
}

void EventStackLayer::advance()
{

}


DRError EventStackLayer::getData(DAQFormats::EventFull*& theEvent, int64_t pos)
{
  if (theEvent){};
  std::ostringstream os;
  os << "EventStackLayerinterface::getData() was called, "
     << " but should not have been called for this type of file!" 
     << " position: " << pos ;  
  
  FaserEventStorage::ES_InternalError ci(ERS_HERE, os.str().c_str());
  throw ci;

  return DRNOOK;
}

EventStackLayer* EventStackLayer::openNext() 
{ 
  return 0; 
}


EventStackLayer*
EventStackLayer::loadAtOffset(int64_t position, EventStackLayer *old) 
{ 
  (void)old;
  if (position < 0) ERS_DEBUG(2," called with negative position!");
  return 0; 
}

bool EventStackLayer::handlesOffset() 
{ 
  return m_handleOffs; 
}

bool EventStackLayer::handlesContinuation() 
{ 
  return m_handleCont; 
} 

// std::string EventStackLayer::nextInContinuation() 
//{ 
//return "";
//} 


bool EventStackLayer::endOfFileSequence() 
{ 
  return true; 
}

void EventStackLayer::setContinuationFile(std::string next)
{
  if ( next == "blabla" ) ERS_DEBUG(2, " this is a blabla test");
}

void EventStackLayer::setFile( std::string filename)
{
  ERS_DEBUG(3, " SETTING filename " << filename);
  m_currentFile = filename;
}

std::string EventStackLayer::fileName() 
{ 
  return m_currentFile;
} 

bool EventStackLayer::responsibleforMetaData()
{
  return m_responsibleMetaData;
}

void EventStackLayer::setResponsibleForMetaData()
{
  m_responsibleMetaData=true;
}

std::string EventStackLayer::GUID() const
{
  return "";
}

void EventStackLayer::handleContinuation() 
{
  m_handleCont=true;
}


unsigned int EventStackLayer::nextInSequence(unsigned int current) 
{
  std::ostringstream os;
  os << "EventStackLayerinterface::getData() was called, "
     << " but should not have been called for this type of file!" 
     << " Current index: " << current;

  FaserEventStorage::ES_InternalError ci(ERS_HERE, os.str().c_str());
  throw ci;

  return 0;
}



void EventStackLayer::handleOffset() 
{
  m_handleOffs=true;
}

unsigned int EventStackLayer::runNumber() const { FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci;return 0;}
unsigned int EventStackLayer::maxEvents() const {FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return 0;}
unsigned int EventStackLayer::recEnable() const {FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci;return 0;}
unsigned int EventStackLayer::triggerType() const {FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return 0;}
std::bitset<128>     EventStackLayer::detectorMask() const {FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return 0;}
unsigned int EventStackLayer::beamType() const { FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return 0;}
unsigned int EventStackLayer::beamEnergy() const {FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return 0;}
std::vector<std::string> EventStackLayer::freeMetaDataStrings() const { 
  FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; 
  std::vector<std::string> empty_vec;
  return empty_vec;
}

FaserEventStorage::CompressionType EventStackLayer::compression() const{
  FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; 
  return FaserEventStorage::NONE;
}

unsigned int EventStackLayer::fileStartDate() const { FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return 0; }
unsigned int EventStackLayer::fileStartTime() const { FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return 0; }

unsigned int EventStackLayer::fileEndDate() {FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return 0;}      ///< Date when writing has stopped.
unsigned int EventStackLayer::fileEndTime() {FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return 0;}             ///< Time when writing has stopped.
unsigned int EventStackLayer::eventsInFile() {FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return 0;}           ///< Number of events in this file.
unsigned int EventStackLayer::dataMB_InFile() {FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return 0;}           ///< Number of megabytes in this file.
unsigned int EventStackLayer::eventsInFileSequence() {FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return 0;}    ///< Number of events in this file sequence written so far.
// unsigned int EventStackLayer::dataMB_InFileSequence() {mythrowE("ESL Interface called for metadata. Abort"); return 0;}   ///< Number of MB in this file sequence written so far. 
// unsigned int EventStackLayer::fileStatusWord() {throw 14; return 0;}          ///< Indicates an end of sequence. 


unsigned int EventStackLayer::fileSizeLimitInDataBlocks() const
{
  FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci;
  return 0;
}

unsigned int EventStackLayer::fileSizeLimitInMB() const
{
  FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci;
  return 0;
}

std::string  EventStackLayer::appName() const 
{
  FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci;
  return "";
}

std::string  EventStackLayer::fileNameCore() const 
{
  FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci;
  return "";
}

unsigned int EventStackLayer::dataMB_InFileSequence() 
{
  FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci;
  return 0;
}




uint32_t EventStackLayer::lumiblockNumber() 
{FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return 0; }

std::string EventStackLayer::stream() 
{FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return ""; }

std::string EventStackLayer::projectTag()
{FaserEventStorage::ES_InternalError ci(ERS_HERE, "ESL Interface called for metadata. This should not happen. Abort.");
  throw ci; return ""; }



int EventStackLayer::unfile_record(void *ri, const void *pi) 
{
  ERS_DEBUG(3,"UNFILE RECORD called ");
  uint32_t *record = (uint32_t *)ri;
  uint32_t *pattern = (uint32_t *)pi;
  int size=pattern[1];
  ERS_DEBUG(3,"reading record of size "<< size);
  int64_t pos = m_fR->getPosition();

  for(int i=0; i<size; i++) 
    {
      ERS_DEBUG(3,"READING ");
      if(pattern[i] != 0) 
{
  uint32_t tst=0;
  m_fR->readData((char *)(&tst),sizeof(uint32_t));
  ERS_DEBUG(3,"GOT "<< tst << " vs " <<  pattern[i] <<" expected");
  if(not (tst==pattern[i])) 
    {
      m_fR->setPosition(pos);
      return 0;
    }
} 
      else 
{
  m_fR->readData((char *)(record+i),sizeof(uint32_t));
  ERS_DEBUG(3,"GOT " << *(record+i));
}
    }
  return size;
}

void EventStackLayer::setCaller(EventStackLayer* caller)
{
m_caller = caller;
}

EventStackLayer* EventStackLayer::getCaller()
{
return m_caller;
}


