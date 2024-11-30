#include "ESLOriginalFile.h"
#include "FaserEventStorage/EventStorageIssues.h"
#include "DataReaderController.h"
//#include "compression/compression.h"
//#include "compression/DataBuffer.h"
#include "EventFormats/DAQFormats.hpp"

#include <stdio.h>
#include <string.h>

FESLOriginalFile::FESLOriginalFile( fRead * m_fR) : EventStackLayer(m_fR)
{
  m_me = "FESLOriginal"; 

  ERS_DEBUG(3, identify() << " Constructor called ");
  
  uint64_t cpos = m_fR->getPosition();
  m_fR->setPositionFromEnd(0);
  m_cfilesize = m_fR->getPosition();
  m_fR->setPosition(cpos);

  ERS_DEBUG(3, identify() << " File size found: " << m_cfilesize);

  m_endOfFile = false;
  m_advance_fer_updated = false;

  m_compression = FaserEventStorage::NONE;
  //m_uncompressed = new compression::DataBuffer();

}
 
FESLOriginalFile::~FESLOriginalFile() 
{
  //delete m_uncompressed;
  ERS_DEBUG(3, identify() <<" destroyed ") ; 
}

 
EventStackLayer* FESLOriginalFile::openNext()
{
  return 0;
  //does nothing
}


//DRError FESLOriginalFile::getData(unsigned int &eventSize, char **event, int64_t pos, bool memAlreadyAlloc, int64_t allocSizeInBytes)
DRError FESLOriginalFile::getData(DAQFormats::EventFull*& theEvent, int64_t pos)
{
  ERS_DEBUG(2,"Entered FESLOriginalFile::getData().");
  
  if(pos>0) m_fR->setPosition(pos);
//uint64_t startofevent = m_fR->getPosition(); // we will need this in case of reading failure.

  // Don't allow stale data
  if (theEvent != NULL) delete theEvent;
  theEvent = new DAQFormats::EventFull();

  // Read data header
  size_t sizeofHeader = theEvent->header_size();
  char* buffer = new char[sizeofHeader];

  if (NULL == buffer) {
    ERS_DEBUG(1, " tried to allocate " << theEvent->header_size() << " bytes for header but failed!");
    FaserEventStorage::ES_AllocatingMemoryFailed ci(ERS_HERE, "FESLOriginalFile tried to allocate memory for header but failed. Abort.");
      throw ci;
  }

  // Check if we will read beyond the end of the file
  uint64_t cpos = m_fR->getPosition();
  uint64_t remaining = m_cfilesize - cpos;
  ERS_DEBUG(3, "current position before header " << cpos << " with " << remaining << " remaining") ;

  if (remaining < sizeofHeader) {
    ERS_DEBUG(1, "Requested" << sizeofHeader
	      << " bytes for header but only " << remaining << " remain in file!");

    FaserEventStorage::ES_OutOfFileBoundary ci(ERS_HERE, "Trying to read more data for header than remains in the file. This likely means your event is truncated. If you still want to read the data, catch this exception and proceed. The data block contains the rest of the data. ");
    throw ci;

    // In case this gets caught, but won't work well
    m_fR->readData(buffer, remaining);
    theEvent->loadHeader((uint8_t*)buffer, remaining);
    delete[] buffer;
    return DRNOOK;
  }

  m_fR->readData(buffer, sizeofHeader);
  theEvent->loadHeader((uint8_t*)buffer, sizeofHeader);

  delete[] buffer;

  ERS_DEBUG(2,"DATA HEADER: Expected event size " << theEvent->size());

  // Now we have the event length, create buffer to store this
  size_t sizeofPayload = theEvent->payload_size();
  buffer = new char[sizeofPayload];

  if (NULL == buffer) {
    ERS_DEBUG(1, "Tried to allocate " << sizeofPayload << " bytes for payload but failed!");    
    FaserEventStorage::ES_AllocatingMemoryFailed ci(ERS_HERE, "FESLOriginalFile tried to allocate memory for payload but failed. Abort.");
    throw ci;
  }

  // Check if we will read beyond the end of the file
  cpos = m_fR->getPosition();
  ERS_DEBUG(3, "current position before payload " << cpos << " with " << remaining << " remaining");
  remaining = m_cfilesize - cpos;

  if (remaining < sizeofPayload) {
    ERS_DEBUG(1, "Requested " << sizeofPayload 
	      << " bytes for payload but only " << remaining << " remain in file ");

    FaserEventStorage::ES_OutOfFileBoundary ci(ERS_HERE, "Trying to read more data than remains in the file. This could mean that either your event is truncated, or that the event size record of this event is corrupted. If you still want to read the data, catch this exception and proceed. The data block contains the rest of the data. ");
    throw ci;

    // In case this gets caught, although thee loadPayload will throw its own error
    m_fR->readData(buffer, remaining);
    theEvent->loadPayload((uint8_t*)buffer, remaining);
    delete[] buffer;

    return DRNOOK;

  } else {

    // OK, read event
    m_fR->readData(buffer, sizeofPayload);
    theEvent->loadPayload((uint8_t*)buffer, sizeofPayload);
  
  }
  ERS_DEBUG(2, "Event:\n" << *theEvent);
  
  ERS_DEBUG(3,"Finished reading the event.");

  // CHECK FOR END OF FILE REACHED
  // This is not an error
  if( /*m_fR->isEoF()*/  m_cfilesize<=m_fR->getPosition()) {
     //isEoF only tells you an end of file if you already hit the eof. it doesn't tell you if ou are exactly at the limit. therefore we check like that
    ReadingIssue ci(ERS_HERE, "End of file reached after event.");
    ers::warning(ci);
    m_endOfFile = true;
    m_finished = true;
    
  }

  return DROK;
}



EventStackLayer* FESLOriginalFile::loadAtOffset(int64_t position, EventStackLayer *old)
{
  if(old){}; //Avoid compilation warning

  if (m_cfilesize <=  position)
  {
 FaserEventStorage::ES_OutOfFileBoundary ci(ERS_HERE, "Trying to jump outside Filesize Boundary. Abort.");
  throw ci;
 
  }
  m_fR->setPosition(position);
  return this;
}

void FESLOriginalFile::prepare()
{
  // ET - Mainly reads metadata.
  // No metadata, so basically skip this

  /*
  ERS_DEBUG(2,"Read metadata from the beginning of the file.");
 
  file_start_record tmpfsr = file_start_pattern;
  tmpfsr.version = 0x0;
 
 
  if(unfile_record(&m_latest_fsr,&tmpfsr) == 0) {
      ERS_DEBUG(2,"File start record not found after fopen.");
      FaserEventStorage::ES_WrongFileFormat ci(ERS_HERE, "File start record not found after fopen.. FESLOriginalFile reported: File Format is not known. Abort");
      throw ci;
  }
  
  ERS_DEBUG(2,"File start record found after fopen. version "<< m_latest_fsr.version);
  
  m_latest_fns=unfile_name_strings();

  if(m_latest_fns.appName=="there was") {
    ERS_DEBUG(3,"File name strings not found in file.");     
    FaserEventStorage::ES_WrongFileFormat ci(ERS_HERE, "FESLOriginalFile reported: File name strings not found in file.");
    throw ci;
  } else {
    ERS_DEBUG(3,"File name strings found.");         
  }

  readOptionalFreeMetaDataStrings();
  
  checkCompression();

  if (m_latest_fsr.version == 0x2) {
    ERS_DEBUG(3,"Version 2 found. Switching to compability mode.");
    
    v2_internal_run_parameters_record v2_internal_rpr;
    if(unfile_record(&v2_internal_rpr,&v2_run_parameters_pattern) == 0) {
      ERS_DEBUG(3,"Run parameters record not found in file.");
      FaserEventStorage::ES_WrongFileFormat ci(ERS_HERE, "FESLOriginalFile reported: Old Run parameters record not found in file.");
      throw ci;
    } else {
      ERS_DEBUG(3,"Run parameters record found.");
      m_internal_latest_rpr.marker=v2_internal_rpr.marker;              
      m_internal_latest_rpr.record_size=v2_internal_rpr.record_size;
      m_internal_latest_rpr.run_number=v2_internal_rpr.run_number;          
      m_internal_latest_rpr.max_events=v2_internal_rpr.max_events;          
      m_internal_latest_rpr.rec_enable=v2_internal_rpr.rec_enable;          
      m_internal_latest_rpr.trigger_type=v2_internal_rpr.trigger_type;        
      m_internal_latest_rpr.detector_mask_1of4=v2_internal_rpr.detector_mask;
      m_internal_latest_rpr.detector_mask_2of4=0;
      m_internal_latest_rpr.detector_mask_3of4=0;
      m_internal_latest_rpr.detector_mask_4of4=0;
      m_internal_latest_rpr.beam_type=v2_internal_rpr.beam_type;           
      m_internal_latest_rpr.beam_energy=v2_internal_rpr.beam_energy;
    }

  } else if (m_latest_fsr.version == 0x5) {
    ERS_DEBUG(3,"Version 5 found. Switching to compability mode.");

    v5_internal_run_parameters_record v5_internal_rpr;
    if(unfile_record(&v5_internal_rpr,&v5_run_parameters_pattern) == 0) {
      ERS_DEBUG(3,"Run parameters record not found in file.");
      FaserEventStorage::ES_WrongFileFormat ci(ERS_HERE, "FESLOriginalFile reported: Old Run parameters record not found in file.");
      throw ci;
    } else {
      ERS_DEBUG(3,"Run parameters record found.");
      m_internal_latest_rpr.marker=v5_internal_rpr.marker;              
      m_internal_latest_rpr.record_size=v5_internal_rpr.record_size;
      m_internal_latest_rpr.run_number=v5_internal_rpr.run_number;          
      m_internal_latest_rpr.max_events=v5_internal_rpr.max_events;          
      m_internal_latest_rpr.rec_enable=v5_internal_rpr.rec_enable;          
      m_internal_latest_rpr.trigger_type=v5_internal_rpr.trigger_type;      
      m_internal_latest_rpr.detector_mask_1of4=v5_internal_rpr.detector_mask_1of2;
      m_internal_latest_rpr.detector_mask_2of4=v5_internal_rpr.detector_mask_2of2;
      m_internal_latest_rpr.detector_mask_3of4=0;
      m_internal_latest_rpr.detector_mask_4of4=0;
      m_internal_latest_rpr.beam_type=v5_internal_rpr.beam_type;           
      m_internal_latest_rpr.beam_energy=v5_internal_rpr.beam_energy;
    }

  } else if (m_latest_fsr.version == 0x6) {
    ERS_DEBUG(3,"New version found.");
    
    if(unfile_record(&m_internal_latest_rpr,&run_parameters_pattern) == 0) {
      ERS_DEBUG(1,"Run parameters record not found in file.");
      FaserEventStorage::ES_WrongFileFormat ci(ERS_HERE, "FESLOriginalFile reported: Run parameters record not found in file.");
      throw ci;
    } else {
      ERS_DEBUG(3,"Run parameters record found.");
    }
  } else {
    FaserEventStorage::ES_WrongFileFormat ci(ERS_HERE, "FESLOriginalFile reported: File Format Version is not known. Abort");
    throw ci;
  }

  ERS_DEBUG(2,"All metadata OK at the beginning of the file.");

  if(unfile_record(&m_latest_fer,&file_end_pattern) != 0) 
    {
      //ERS_WARNING("No event data. This file contains only metadata. "
      //  <<"End of file metadata found after the run parameters record.");
      ERS_DEBUG(3,"End Record right after metadata. This OriginalFile contains only metadata.");

      m_endOfFile = true;
      m_fer_read = true;
      
    }
  */
  ERS_DEBUG(1,"File " << fileName() << " ready for reading."); 
 
}

bool FESLOriginalFile::doneLoading()
{
  return true; //an original file is not further merging anything
}


bool FESLOriginalFile::moreEvents() 
{
  ERS_DEBUG(3, m_me <<" more events: "<< (!m_endOfFile && !m_error));
  return (!m_endOfFile && !m_error);
}


void FESLOriginalFile::readOptionalFreeMetaDataStrings() 
{
  ERS_DEBUG(3,"readOptionalFreeMetaDataStrings not implemented!");
 }

void FESLOriginalFile::checkCompression()
{
  // Always none
  m_compression = FaserEventStorage::NONE;
  /*
  std::string comp = extractFromMetaData(FaserEventStorage::compressiontag+"=");

  if(comp != ""){
    m_compression = string_to_type(comp);
    ERS_DEBUG(1,"Compressed file found. FMS: "<< comp << " Type: " 
      << m_compression);
  }
  */
}


unsigned int FESLOriginalFile::fileStartDate() const 
{
  return m_latest_fsr.date;
}

unsigned int FESLOriginalFile::fileStartTime() const 
{
  return m_latest_fsr.time; 
}


std::vector<std::string> FESLOriginalFile::freeMetaDataStrings() const
{
  return m_fmdStrings;
}

FaserEventStorage::CompressionType FESLOriginalFile::compression() const {
  return m_compression;
}

//Run parameter get records
unsigned int FESLOriginalFile::runNumber() const {
  return m_internal_latest_rpr.run_number;
}

     
unsigned int FESLOriginalFile::maxEvents() const {
  return  m_internal_latest_rpr.max_events;
}

     
unsigned int FESLOriginalFile::recEnable() const {
  return  m_internal_latest_rpr.rec_enable;
}

     
unsigned int FESLOriginalFile::triggerType() const {
  return m_internal_latest_rpr.trigger_type;
}

   
std::bitset<128> FESLOriginalFile::detectorMask() const {
    
  uint64_t tmp = m_internal_latest_rpr.detector_mask_4of4;
  tmp <<= 32;
  tmp |= m_internal_latest_rpr.detector_mask_3of4;
  
  std::bitset<128> mask(tmp);
  mask <<= 64;
  
  tmp = m_internal_latest_rpr.detector_mask_2of4;
  tmp <<= 32;
  tmp |= m_internal_latest_rpr.detector_mask_1of4;

  mask |= tmp;
  return mask;
}

unsigned int FESLOriginalFile::beamType() const {
  return m_internal_latest_rpr.beam_type;
}

unsigned int FESLOriginalFile::beamEnergy() const {
  return  m_internal_latest_rpr.beam_energy;
}


unsigned int FESLOriginalFile::fileEndDate() 
{

  if(!m_advance_fer_updated) {
    m_latest_fer = currentFileFER();
    m_advance_fer_updated = true;
  }

  return m_latest_fer.date;
}

    
unsigned int FESLOriginalFile::fileEndTime() 
{

  if(!m_advance_fer_updated) {
    m_latest_fer = currentFileFER();
    m_advance_fer_updated = true;
  }

  return m_latest_fer.time;
}

    
// This doesn't really work...
unsigned int FESLOriginalFile::eventsInFile() 
{

  if(!m_advance_fer_updated) {
    m_latest_fer = currentFileFER();
    m_advance_fer_updated = true;
  }

  //return m_latest_fer.events_in_file;
  return 1; // Avoid no events in file error
}

 
unsigned int FESLOriginalFile::dataMB_InFile() 
{

  if(!m_advance_fer_updated) {
    m_latest_fer = currentFileFER();
    m_advance_fer_updated = true;
  }

  return m_latest_fer.data_in_file;
}

   
unsigned int FESLOriginalFile::eventsInFileSequence() 
{

  if(!m_advance_fer_updated) {
    m_latest_fer = currentFileFER();
    m_advance_fer_updated = true;
  }

  return m_latest_fer.events_in_run; 
}

  
unsigned int FESLOriginalFile::dataMB_InFileSequence() 
{

  if(!m_advance_fer_updated) {
    m_latest_fer = currentFileFER();
    m_advance_fer_updated = true;
  }

  return m_latest_fer.data_in_run;
}


unsigned int FESLOriginalFile::fileSizeLimitInDataBlocks() const 
{
  return m_latest_fsr.sizeLimit_dataBlocks; 
}

unsigned int FESLOriginalFile::fileSizeLimitInMB() const 
{
  return m_latest_fsr.sizeLimit_MB; 
}

std::string FESLOriginalFile::appName() const 
{
  return m_latest_fns.appName;
}

std::string FESLOriginalFile::fileNameCore() const 
{
  return m_latest_fns.fileNameCore;
}




    
unsigned int FESLOriginalFile::fileStatusWord() 
{

  if(!m_advance_fer_updated) {
    m_latest_fer = currentFileFER();
    m_advance_fer_updated = true;
  }

  return m_latest_fer.status; 
}

bool FESLOriginalFile::endOfFileSequence()
{
  if (m_error) return true;
  ERS_DEBUG(3, identify() << " status word has value " << this->fileStatusWord());
  return this->fileStatusWord();

}

unsigned int FESLOriginalFile::nextInSequence(unsigned int current)
{
  return current+1; //stupid, but original file format does not support next/prev
}


file_end_record FESLOriginalFile::currentFileFER() {
  ERS_DEBUG(3,"FESLOriginalFile::currentFileFER() called.");

  file_end_record cfFER=file_end_pattern;
  m_advance_fer_updated = true;

  /*
  int64_t pos = m_fR->getPosition();

  //forward to the end and reverse a bit now and 
  int64_t back=sizeof(file_end_pattern);
  m_fR->setPositionFromEndSafe(-back);

  if (unfile_record(&cfFER,&file_end_pattern)!= 0)
    {
      m_advance_fer_updated = true;
    };
  
  // rewind back
  m_fR->setPosition(pos);

  if(cfFER.date==0)
    {
      ERS_DEBUG(3,"No end of file record found");
      FaserEventStorage::ES_NoEndOfFileRecord ci(ERS_HERE, "FESLOriginalFile reported: File end record not found in file. If you still want to read data from this file, either catch these excpetions or don't ask for information inside the EOF Record");
      throw ci;

    }
  */
  return cfFER;
}

internal_run_parameters_record FESLOriginalFile::getRunParameterrecord()
{
  return m_internal_latest_rpr;
}

std::string FESLOriginalFile::GUID() const 
{
  ERS_DEBUG(2,"DataReaderBase::GUID() called.");
  if(m_fmdStrings.size()==0) {
    ERS_DEBUG(2,"No metadata strings in this file. Return empty GUID string.");
    return std::string("");
  }
  std::string guid = m_fmdStrings[0];
  std::string ptn("GUID=");
  std::string::size_type pos=guid.find(ptn);
  if(pos==std::string::npos) {
    ERS_DEBUG(2,"No GUID in metadata strings of this file. Return empty GUID string.");
    return std::string("");
  }
  pos+=ptn.size();
  guid=guid.substr(pos,guid.size());
 
  ERS_DEBUG(2,"Returning "<<guid);
  return guid;
}

uint32_t FESLOriginalFile::lumiblockNumber()
{
  ERS_DEBUG(2,"DataReaderBase::lumiblockNumber() called.");
  std::string lumistring = extractFromMetaData("LumiBlock=");
  if (lumistring =="")
    {
      ERS_DEBUG(2,"No Lumiblock Info found. returning 0");
      return 0;
    }
  std::istringstream i(lumistring);
  i >> m_lumiblockNumber;

  return m_lumiblockNumber;
}

std::string FESLOriginalFile::stream()
{
  m_stream = extractFromMetaData("Stream=");
  return m_stream;
}

std::string FESLOriginalFile::projectTag()
{
  m_projectTag = extractFromMetaData("Project=");
  return m_projectTag;
}


file_name_strings FESLOriginalFile::unfile_name_strings() 
{

  int64_t pos = m_fR->getPosition();

  uint32_t tst;

  FaserEventStorage::file_name_strings nst;
  nst.appName="there was"; nst.fileNameCore="a problem";

  m_fR->readData((char *)(&tst),sizeof(uint32_t));
  if(tst != FaserEventStorage::file_name_strings_marker) {
    m_fR->setPosition(pos);
    return nst;
  }

  uint32_t size=0;
  m_fR->readData((char *)(&size),sizeof(uint32_t));

  char *csn=new char[size];
  m_fR->readData(csn,size);
  std::string name(csn,size);
  delete [] csn;

  char buf[4];
  char ns = size % 4;
  if(ns) m_fR->readData(buf,4-ns);

  m_fR->readData((char *)(&size),sizeof(uint32_t));
  
  char *cst=new char[size];
  m_fR->readData(cst,size);
  std::string tag(cst,size);
  delete [] cst;

  ns = size % 4;
  if(ns) m_fR->readData(buf,4-ns);

  nst.appName = name;
  nst.fileNameCore = tag;

  return nst;
}

std::string FESLOriginalFile::extractFromMetaData(std::string token)
{
  ERS_DEBUG(2,"DataReaderBase::extractFromMetaData() called: " << token);
  if(m_fmdStrings.size()==0) 
    {
      ERS_DEBUG(2,"No metadata strings in this file. Return empty string");
      return "";
    }
  for (unsigned int i=0; i< m_fmdStrings.size(); i++)
    {
      std::string cs = m_fmdStrings.at(i);
      std::string::size_type pos=cs.find(token);
      if(pos==std::string::npos) 
	{ /*not foundokay */}
      else
	{
	  pos+=token.size();
	  std::string result=cs.substr(pos,cs.size());
	  return result;
	}
    }
  ERS_DEBUG(2,"No "<< token <<" found in metadata strings of this file. Return emptystring.");
  return "";
}

