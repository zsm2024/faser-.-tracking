#ifndef FASEREVENTSTACKLAYER_H
#define FASEREVENTSTACKLAYER_H

#include "ers/ers.h"
#include <fstream>
#include <vector>
#include <stack>
#include <bitset>
#include "FaserEventStorage/DRError.h"
#include "FaserEventStorage/fRead.h"
#include "FaserEventStorage/DRError.h"
#include "FaserEventStorage/EventStorageRecords.h"
#include "EventStorage/RawFileName.h"
#include "FaserEventStorage/ESCompression.h"

#include "EventStorageInternalRecords.h"

namespace DAQFormats {
class EventFull;
}

using namespace FaserEventStorage;


class EventStackLayer //=ESL henceforward
{
 protected:
 EventStackLayer(): 
  m_fR(NULL),
    m_caller(NULL)
      {}

 public:
  EventStackLayer( fRead* fR ) ;
  
  virtual ~EventStackLayer() {};
  
  virtual std::string identify() {return m_me;}
  
  //virtual DRError getData(unsigned int &eventSize, char **event, int64_t pos, bool memAlreadyAlloc, int64_t allocSizeInBytes);
  virtual DRError getData(DAQFormats::EventFull*& theEvent, int64_t pos);
  
  //the interface to instantiate a son-ESL instance.
  //e.g. MergeFileReader Instantiates an Originalfilereader
  virtual EventStackLayer* openNext();
  
  
  //the instance that can handle offsets also needs to be able to "load" a new original file.
  virtual EventStackLayer* loadAtOffset(int64_t position, EventStackLayer *old);
  
  
  //the interface to prepare a recently instantiated ESL
  //this should parse the header (if any), so that either data can be read, or openNext() can be called
  virtual void prepare() = 0 ;
  
  //tells the caller if the instance is ready for data to be read. If true is given, the caller can start with getData. If false is given, openNext() should be called, so the current instance can create an Object to be put on the stack.
  virtual bool doneLoading() = 0 ;
  
  
  //tells the caller if more events can be read from this instance. 
  // - true should be returned if this is an Originalfile and reading is not finished. 
  // - True should be returned by MergeFileHandler and MultiFileHand have more Originalfiles or MergeFiles to load
  // - False should be returned if a Originalfile reader reached the end of the originalfile, or the Mergefilereader has no more Originalfiles or the MultiFilereader is out of files.
  virtual bool moreEvents() = 0 ;
  
  
  //this tells the caller if the instance handles offsets inside files. This is important for the get/set Position stuff. If a setPosition is called, the stack will be emptied until an instance is found which can handle offsets. Returns true for mergefilereader and false for multifile. If Multifilereader loads originalfiles directly, Originalfile handles offsets.
  virtual bool handlesOffset();
  
  //tells the caller if continuation is handled by this instance on the stack
  virtual bool handlesContinuation();
  
  //if the intance can handle continuation, it needs to provide a ressource, e.g. a filename string to the loader. E.g. Multifilehandler is finished with a file, and it asks the old file which is the next
  //virtual std::string nextInContinuation() ;
  
  //tells if the sequence is at an end
  virtual bool endOfFileSequence() ;
  
  //sets the next "file" to be opened (by Mutifilereader)
  virtual void setContinuationFile(std::string next);
  
  virtual void setFile( std::string filename);
  
  virtual std::string fileName();
  
  //A ESl can be asked id it is rsponsible for metadata responsibiliry
  virtual bool responsibleforMetaData();
  
  virtual void setResponsibleForMetaData();
  
  virtual std::string GUID() const;
  
  virtual bool finished();
  virtual void advance();
  
  void handleContinuation() ;
  virtual unsigned int nextInSequence(unsigned int current) ;
  
  
  void handleOffset() ;
  
  //Run parameter get records
  virtual unsigned int runNumber() const;
  virtual unsigned int maxEvents() const;
  virtual unsigned int recEnable() const ;
  virtual unsigned int triggerType() const;
  virtual std::bitset<128> detectorMask() const ;
  virtual unsigned int beamType() const ;
  virtual unsigned int beamEnergy() const;
  virtual std::vector<std::string> freeMetaDataStrings() const;
  virtual CompressionType compression() const;
  
  virtual unsigned int fileStartDate() const;
  virtual unsigned int fileStartTime() const;

  virtual unsigned int fileEndDate();             ///< Date when writing has stopped.
  virtual unsigned int fileEndTime();             ///< Time when writing has stopped.
  virtual unsigned int eventsInFile();           ///< Number of events in this file.
  virtual unsigned int dataMB_InFile();           ///< Number of megabytes in this file.
  virtual unsigned int eventsInFileSequence();    ///< Number of events in this file sequence written so far.

  virtual unsigned int fileSizeLimitInDataBlocks() const ; 
  virtual unsigned int fileSizeLimitInMB() const ;
  virtual std::string appName() const ; 
  virtual std::string fileNameCore() const ;  

  virtual unsigned int dataMB_InFileSequence();   ///< Number of MB in this file sequence written so far. 
  //virtual unsigned int fileStatusWord();          ///< Indicates an end of sequence. 
  
  virtual uint32_t lumiblockNumber();
  virtual std::string stream();
  virtual std::string projectTag();
  
  virtual void setCaller(EventStackLayer* caller);
  virtual EventStackLayer* getCaller();

  
 protected:
  //This is the raw file access reader
  //it is assigned in the constructor
  fRead* m_fR;
  
  EventStackLayer* m_caller;
  
  bool m_finished;
  bool m_error;
  
  std::string m_me;
  bool m_handleCont;
  bool m_handleOffs;
  
  bool m_responsibleMetaData;
  std::string m_currentFile;
  
  //unfile record is used by the inheriting classes
  //to read data from the file
  int unfile_record(void *ri, const void *pi);
  
};



#endif 

