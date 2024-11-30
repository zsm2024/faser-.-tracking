#include <memory>
#include <sstream>
#include <iomanip>


#include "ers/ers.h"
#include "FaserEventStorage/EventStorageIssues.h"
#include "FaserEventStorage/fRead.h" 
#include "DataReaderController.h"
#include "EventStorage/RawFileName.h"
#include "EventStackLayer.h"
#include "ESLMultiFile.h"
//#include "ESLMergeFile.h"
#include "ESLOriginalFile.h"
#include "EventFormats/DAQFormats.hpp"

using namespace std;
using namespace FaserEventStorage;


DataReaderController::DataReaderController(fRead *fR, string filename)
{
  m_fR = fR; //the raw reader
  m_fileName = filename; //the initial filename
  
  m_sequenceReading = false; //seq reading off per default 
  m_finished = false;
  m_last_file_finished = false ;

  m_markForDeletion = 0;
  m_responsibleForMetaData = 0;

  //This is the entry Point, it always starts with a MultiFileReader
  m_filehandler = new FESLMultiFile(m_fR);

  ERS_DEBUG(1,"filehandler instantiated " << m_filehandler->identify() );

  m_filehandler->setFile(filename);

  ERS_DEBUG(3,"filehandler file set " << m_filehandler->identify() <<  " " << filename);

  // ET doesn't do anything, so OK
  m_filehandler->prepare();

  ERS_DEBUG(3,"filehandler " << m_filehandler->identify() << " prepared.");

  m_stack.push(m_filehandler);

  m_cFileNumber = 0; 
  m_sizeOfFileNumber = 0;

  initialize();

  ERS_DEBUG(3,"Initialization complete. Exiting Controller Constructor ");
}

DataReaderController::~DataReaderController()
{
  if (m_fR && m_fR->isOpen()) m_fR->closeFile();

  while (!m_stack.empty()) {
    EventStackLayer *tmp = m_stack.top(); 
    m_stack.pop();
    if (tmp != m_markForDeletion and tmp != m_filehandler)
      delete tmp;
  }

  if (m_markForDeletion != 0)
    {
      delete m_markForDeletion;
      m_markForDeletion = 0;
    }
  
  if(m_filehandler) delete m_filehandler;
  if(m_fR) delete m_fR;
}


void DataReaderController::initialize()
{

  //Now we load all the Instances onto the stack. filehandler will return an instance of type ESLMergeFile or ESLOriginalFile, this instance will report (method doneLoading) if it is finished. In case of the Originalfilereader, this will be true, in case of the MergeFileReader, this will be false, because the mergereader will want to load an originalfile inside. The process continues and the stack is filled. The resulting Stack should have a top element ready for Data to be read. 

  bool done=false;
  while (!done)
    {
      ERS_DEBUG(2, m_stack.top()->identify() << " openNext() called");
      EventStackLayer *ELSnew = m_stack.top()->openNext();
      ERS_DEBUG(2, ELSnew->identify() << " instantiated");

      
      //ELSnew->prepare();
      // prepare is already done in openNext.
      
      if (ELSnew->responsibleforMetaData())
	{
	  //if this lyer is responsible for metadata, it is linked to the internal pointer of Controller. Controller will the relay metadata requests to this layer. great.
	  m_responsibleForMetaData = ELSnew;
	}
      
      ELSnew->setCaller(m_stack.top());
      
      m_stack.push( ELSnew );
      ERS_DEBUG(2, ELSnew->identify() << " put on stack");

      done = ELSnew->doneLoading();
      ERS_DEBUG(2, ELSnew->identify() << " reported doneLoading: " << done);
    }

}

bool DataReaderController::good() const
{  
  return m_stack.size()? m_stack.top()->moreEvents():false;
}

bool DataReaderController::endOfFile() const
{
  //this is just to keep compatibility with calling code. they want to know i f a file just finished

  bool a = m_last_file_finished;
  //m_last_file_finished = false;
  return a;
}

bool DataReaderController::endOfFileSequence() const
{
  return false;
}  



//DRError DataReaderController::getData(unsigned int &eventSize, char **event, int64_t pos = -1 , bool memAlreadyAlloc = false, int64_t allocSizeInBytes = -1)
DRError DataReaderController::getData(DAQFormats::EventFull*& theEvent, int64_t pos)
{

  int64_t oldpos = 0;
  m_last_file_finished=false;


  if (m_markForDeletion != 0)
    {
      delete m_markForDeletion;
      m_markForDeletion = 0;
    }

  //if the position is defined, we need to remember the old position
  //and jump to the given position
  if ( pos != -1) 
    {
      oldpos = this->getPosition();
      this->setPosition(pos);
    }


  DRError res =  m_stack.top()->getData(theEvent, -1);



  //if position was defined, we need to jump back to the old position BEFORE checking for more events. Else if we are reading by offset, the program will crash.
  if ( pos != -1) 
    {
      this->setPosition(oldpos);
    }



  if ( !(m_stack.top()->moreEvents()) )
    {
      ERS_DEBUG(2, " No more Events on top of stack ");
      //this is the "main loop". It handles the switching from one originalfile to the next, from one mergefile to the next etc.
      while ( !m_stack.empty() && !m_stack.top()->moreEvents() )
{
  ERS_DEBUG(2, m_stack.top()->identify() << " has more events?: answer was " << m_stack.top()->moreEvents() );

  EventStackLayer *old = m_stack.top();    
  bool success = old->finished();

  //the following line we only use to report to the caller that just a file was closed
  if ( m_responsibleForMetaData == old )
    m_last_file_finished = true;

    
  ERS_DEBUG(2,  old->identify() << " finished? " << success);

  m_stack.pop();
    
  ERS_DEBUG(2, " Popping " << old->identify() << " from stack");

  if (success && !m_stack.empty()) m_stack.top()->advance(); 

  if (!m_stack.empty() && m_stack.top()->handlesContinuation())
    {
      bool end = (old->endOfFileSequence() || !m_sequenceReading);
      if (!end)
{
  //if the continuation is given, we ask for the next file
  unsigned int nextSN = old->nextInSequence(m_cFileNumber);
  ERS_DEBUG(2, " Next in Sequence is " << nextSN);
  string next = this->nextInContinuation(nextSN);

  m_stack.top()->setContinuationFile(next);

}
     
    } //if continuation is not handled, nothing happens
  if (old != m_responsibleForMetaData && old != m_filehandler)
            {
      //we preserve the m_metaDataResponsible Object. 
              delete old;
            }
          else if (old == m_responsibleForMetaData)
            {
	      //the user probably still wants to read metadata, so we keep in mind that in the next get_data, this must be deleted
              m_markForDeletion = old;
            }
  //while-loop will just continue to pop
 } //end of while loop 
      
      if (m_stack.empty())
	{
	  //if the stack is empty, we are finished
	  ERS_DEBUG(1, " Run finished, no more events ");
	  m_finished = true;
	}
      else
	{
	  ERS_DEBUG(2, " Continuing ");
	  //this will put the next redable item(s) on the stack
	  initialize();
	}
      
    }//end of moreEvents

  //now everything should be in order, so let's read data
  

  return res;

}

DRError DataReaderController::getData(DAQFormats::EventFull*& theEvent) {
  return getData(theEvent, -1);
}
/*
DRError DataReaderController::getDataPreAlloced(unsigned int &eventSize, char **event, int64_t allocSizeInBytes)
{
  return getData(eventSize, event, -1, true, allocSizeInBytes);
}

DRError DataReaderController::getDataPreAllocedWitPos(unsigned int &eventSize, char **event,  int64_t allocSizeInBytes, int64_t pos ) 
{
  return getData(eventSize, event, pos, true, allocSizeInBytes);
}
*/


void DataReaderController::setPosition(int64_t position)
{
  //if a setPosition takes place, only the instance on the stack that can handle position can do it. We are poppoing the stack until we find a responsible Instance.
  
  EventStackLayer *old = 0;

  while ( !m_stack.top()->handlesOffset() )
    {

      old = m_stack.top();
      m_stack.pop();
      //delete old;
      // WARNING: possible memory leak, if more than one Layer is popped. This should not be possible in current implementation. 
      if (m_stack.empty()) 
	{
	  FaserEventStorage::ES_InternalError ci(ERS_HERE, "EventStack is empty. This should not happen.");
	  throw ci;

	}
    }
  //now we should be at the instance where the responisble instance is at top. 

  //Please note: This only works if OriginalFile Layer is the last after Mergefile layer. There is however no reason yet to think this will change. famous last words :-~

  EventStackLayer *off = m_stack.top()->loadAtOffset(position, old);

  if (! (m_stack.top() == off) )
    {
      m_stack.push( off );
    }
  
    
  if (old && old != off) //we need to delete the old one, if the new one is not the same as the old one
    {
      //ERS_DEBUG(1, " deleting old " << old);
      delete old;
    }
  //after this procedure, the correct originalFilehandler should be on top of the stack and ready for reading

  m_finished = false;//we assume the user knows what he's doing and doesnt jump to an end of a file
  
  
}

int64_t DataReaderController::getPosition() const
{
  return m_fR->getPosition();
}

std::string DataReaderController::fileName() const
{
  return m_responsibleForMetaData->fileName();
}

unsigned int DataReaderController::fileStartDate() const
{
  return m_responsibleForMetaData->fileStartDate();
}

unsigned int DataReaderController::fileStartTime() const
{
  return m_responsibleForMetaData->fileStartTime();
}


unsigned int DataReaderController::currentFileNumber() const 
{ 
  return m_cFileNumber; 
}


DRError DataReaderController::enableSequenceReading()
{
  ERS_DEBUG(2,"DataReaderBase::enableSequenceReading() is called. "
	    <<"Try parsing file name\n"<<m_fileName);

  daq::RawFileName thename(m_fileName);
  
  if (!thename.hasValidTrailer()) 
    {
      ERS_DEBUG(1," Test of file name parsing has failed. "
		<<"File sequence reading not enabled.");
      return DRNOSEQ;
    } 
  else 
    {
      m_sizeOfFileNumber = daq::RAWFILENAME_FILE_NUMBER_LENGTH;
      m_cFileNumber = thename.fileSequenceNumber();
      m_fileNameCore = thename.fileNameCore();
      
      ERS_DEBUG(1," fileNameCore: " << m_fileNameCore);
      
      m_sequenceReading=true;
      
      // check if the actual name of this file corresponds to 
      // the fileName reported inside the file
      std::string test = fileName();  // name from meta-data inside the file
      if(test != m_fileName)
	{
	  ERS_DEBUG(1," The name of this file is different from the fileName "
		    << " reported inside the file itself. "
		    << "File sequence reading not enabled.");
	  return DRNOSEQ;
	}
      
      return DROK;
    }

  /**** OLD code here - will be trhown away in due time

  std::string tail= daq::RAWFILENAME_EXTENSION_FINISHED;

  std:: string::size_type tailPos=m_fileName.rfind(tail);

  if(tailPos==std::string::npos) {
  ERS_DEBUG(3,"File name does not contain "<<tail);
  return DRNOSEQ;
  }
  if(tailPos+tail.size() != m_fileName.size()) {
  ERS_DEBUG(3,"File name does not end with "<<tail);
  return DRNOSEQ; 
  }

  std::string nameCore=m_fileName.substr(0,tailPos);
  ERS_DEBUG(3,"File name without the tail "<<nameCore);

  std::string digits("0123456789");
  std::string::size_type numPos=nameCore.find_last_not_of(digits);
  ++numPos;
  if(numPos==string::npos) return DRNOSEQ;

  std::string num=nameCore.substr(numPos,nameCore.size());
  ERS_DEBUG(3,"Expecting file number in this string <"<<num<<">.");

  m_sizeOfFileNumber=num.size();
  ERS_DEBUG(3,"Size of the number "<<m_sizeOfFileNumber);
  if(m_sizeOfFileNumber==0) return DRNOSEQ;
 
  m_cFileNumber=atoi(num.c_str());
  ERS_DEBUG(3,"File number "<<m_cFileNumber);

  m_fileNameCore=nameCore.substr(0,numPos);
  ERS_DEBUG(3,"File name core "<<nameCore);

  ERS_DEBUG(2,"Finished parsing, now test the result.");

  m_sequenceReading=true; 
  
  std::string test = fileName();

  if(test != m_fileName) {
  ERS_DEBUG(1,"Test of file name parsing has failed. "
  <<"File sequence reading not enabled.");
  return DRNOSEQ;
  } 
 
  // --m_cFileNumber; // will be incremented at 1st open
  return DROK;
  
  End of OLD code
  ****/
}


std::string DataReaderController::nextInContinuation(unsigned int expected)
{
  int numFailed=20;
  m_cFileNumber = expected;
  
  while(numFailed){
    std::string next = this->sequenceFileName(true);
      
    if (m_fR->fileExists(next)){
      ERS_DEBUG(2, "fRead reported " << next << " exists");
      return next;
    }else{
      ERS_DEBUG(2, "fRead reported " << next << " does not exist");
      m_cFileNumber++;
      numFailed--;
    }
  }
  
  //Problem
  FaserEventStorage::ES_SquenceNextFileMissing ci(ERS_HERE, "EndOfSequenceFlag was not found and 18 files with continuous sequnce number were not found. Aborting");
  throw ci;

  return "";
}


unsigned int DataReaderController::runNumber() const 
{ 
  return m_responsibleForMetaData->runNumber();
}
unsigned int DataReaderController::maxEvents() const 
{
  return m_responsibleForMetaData->maxEvents();
}
unsigned int DataReaderController::recEnable() const 
{
  return m_responsibleForMetaData->recEnable();
}
unsigned int DataReaderController::triggerType() const 
{
  return m_responsibleForMetaData->triggerType();
}
std::bitset<128> DataReaderController::detectorMask() const 
{
  return m_responsibleForMetaData->detectorMask();
}
unsigned int DataReaderController::beamType() const 
{
  return m_responsibleForMetaData->beamType();
}
unsigned int DataReaderController::beamEnergy() const  
{
  return m_responsibleForMetaData->beamEnergy();
}


CompressionType DataReaderController::compression() const{
  return m_responsibleForMetaData->compression();
}

std::vector<std::string> DataReaderController::freeMetaDataStrings() const
{
  ERS_DEBUG(1, "Upper level is " << m_stack.top()->getCaller()->identify());
  
  std::vector<std::string> result;
  //  if (m_stack.top()->getCaller()->identify()=="ESLMerge")
  //  { //if a mergefiles is read, always return empty metadata
  //   return result;
  //  }
  //  else
  //  {
  result = m_responsibleForMetaData->freeMetaDataStrings();
  //  }


  ERS_DEBUG(1, "RESP " << m_responsibleForMetaData->identify() << " ;et");

  
  std::vector<std::string> cleaned_result;
  
  std::vector<std::string> reserved;
  reserved.push_back("Stream=");
  reserved.push_back("Project=");
  reserved.push_back("LumiBlock=");
  reserved.push_back("GUID=");
  reserved.push_back("Compression=");
  
  
  for (unsigned int i = 0 ; i < result.size(); i++)
    {
      //check for Stream
      
      int is_reserved = 0;
      for (unsigned int res = 0 ; res < reserved.size(); res++)
	{  
	  std::string ptn = reserved.at(res);
	  std::string::size_type pos=result.at(i).find(ptn);
	  if (pos==std::string::npos)
	    {
	      //ERS_DEBUG(1, "String " << result.at(i) << "Not reserved");
	    }
	  else
	    {
	      ERS_DEBUG(1, "String " << result.at(i) << "reserved");
	      is_reserved=1;
	    }
	}
      if (!is_reserved)
	cleaned_result.push_back(result.at(i));
    
    }

  return cleaned_result;

  
}

std::string DataReaderController::GUID() const
{
  return m_responsibleForMetaData->GUID();
}



unsigned int DataReaderController::fileEndDate()
{
  return m_responsibleForMetaData->fileEndDate();
}
///< Date when writing has stopped.
unsigned int DataReaderController::fileEndTime() 
{
  return m_responsibleForMetaData->fileEndTime();
}
///< Time when writing has stopped.
unsigned int DataReaderController::eventsInFile() 
{
  return m_responsibleForMetaData->eventsInFile();
}
///< Number of events in this file.
unsigned int DataReaderController::dataMB_InFile() 
{
  return m_responsibleForMetaData->dataMB_InFile();
}
///< Number of megabytes in this file.
unsigned int DataReaderController::eventsInFileSequence() 
{
  return m_responsibleForMetaData->eventsInFileSequence();
}
///< Number of events in this file sequence written so far.


unsigned int DataReaderController::fileSizeLimitInDataBlocks() const
{
  return m_responsibleForMetaData->fileSizeLimitInDataBlocks();
}

unsigned int DataReaderController::fileSizeLimitInMB() const
{
  return m_responsibleForMetaData->fileSizeLimitInMB();
}

std::string  DataReaderController::appName() const 
{
  /*if (m_responsibleForMetaData->identify()=="ESLMerge") 
    {
    return m_filehandler->getAppName();
    }
    else
  */
  return m_responsibleForMetaData->appName();
}

std::string  DataReaderController::fileNameCore() const 
{
  /*if (m_responsibleForMetaData->identify()=="ESLMerge")
    {
    return m_filehandler->getFileNameCore();
    }
    else*/
  
  return m_responsibleForMetaData->fileNameCore();
}

unsigned int DataReaderController::dataMB_InFileSequence() 
{
  ERS_DEBUG(1,"DataReaderController::dataMB_InFileSequence called but not implemented.");
  return 0;
  //return m_filehandler->getSum();
}




///< Number of MB in this file sequence written so far. 
//unsigned int DataReaderController::fileStatusWord() 
//{
//  return m_responsibleForMetaData->fileStatusWord();
//}
///< Indicates an end of sequence. 


std::string DataReaderController::sequenceFileName(bool isReady) const
{
  ERS_DEBUG(2,"DataReaderBase::sequenceFileName(...) is called.");
    
  daq::RawFileName name(m_fileNameCore, 
			m_cFileNumber, 
			isReady?daq::RAWFILENAME_EXTENSION_FINISHED:daq::RAWFILENAME_EXTENSION_UNFINISHED);
  ERS_DEBUG(2," fileNameCore: " << m_fileNameCore);

  ERS_DEBUG(2,"sequenceFileName returns " << name.fileName());
  return name.fileName();
    
  /**** OLD code here - will be trhown away in due time
	std::ostringstream n;
 
	n << m_fileNameCore;
	n << std::setw(m_sizeOfFileNumber) << std::setfill('0');
	n << m_cFileNumber;

	if(isReady) n << daq::RAWFILENAME_EXTENSION_FINISHED;
	else n << daq::RAWFILENAME_EXTENSION_UNFINISHED;
    
	std::string name = n.str();
    
	ERS_DEBUG(2,"sequenceFileName returns " << name);
	return name;

	End of OLD code
  ***/
}

uint32_t DataReaderController::lumiblockNumber()
{
  return m_responsibleForMetaData->lumiblockNumber();
}
std::string DataReaderController::stream()
{
  return m_responsibleForMetaData->stream();
}
std::string DataReaderController::projectTag()
{
  return m_responsibleForMetaData->projectTag();
}



unsigned int DataReaderController::latestDataNumber() const 
{
  return 1;
}


//void mythrowE(std::string errormessage)
//{
//  FaserEventStorage::ReadingIssue ci(ERS_HERE, errormessage.c_str());
//  throw ci;
//}


