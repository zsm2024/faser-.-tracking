#ifndef FASERDATAREADERCONTOLLER_H
#define FASERDATAREADERCONTOLLER_H

#include <fstream>
#include <vector>
#include <stack>
#include <stdint.h>

#include "FaserEventStorage/DataReader.h"
#include "FaserEventStorage/EventStorageIssues.h"

namespace DAQFormats {
  class EventFull;
}
class EventStackLayer;
class fRead;
class FESLMultiFile;

namespace FaserEventStorage
{
  
  class DataReaderController : public DataReader
  {
  public:
    DataReaderController(fRead *fR, std::string filename); 
    ~DataReaderController();
    
    void initialize();
    
    DRError enableSequenceReading();
    
    void setPosition(); //sets global position in file
    int64_t getPosition() const;//gets current position in the file
    void setPosition(int64_t pos);
    
    bool good() const;
    bool endOfFile() const;
    bool endOfFileSequence() const;  

    unsigned int currentFileNumber() const; 

    
    //    DRError getData(unsigned int &eventSize, char **event, int64_t pos , bool memAlreadyAlloc, int64_t allocSizeInBytes );
    DRError getData(DAQFormats::EventFull*& theEvent);
    DRError getData(DAQFormats::EventFull*& theEvent, int64_t pos);

    //    DRError getDataPreAlloced(unsigned int &eventSize, char **event, int64_t allocSizeInBytes);

    //DRError getDataPreAllocedWitPos(unsigned int &eventSize, char **event,  int64_t allocSizeInBytes, int64_t pos = -1);


    /*methods to delete
      virtual bool fileWasFound() const = 0; 
      
    */
    
    unsigned int runNumber() const ;
    unsigned int maxEvents() const ;
    unsigned int recEnable() const ;
    unsigned int triggerType() const ;
    std::bitset<128> detectorMask() const ;
    unsigned int beamType() const ;
    unsigned int beamEnergy() const ;
    std::string GUID() const;
    
    
    std::vector<std::string> freeMetaDataStrings() const;

    CompressionType compression() const;

    std::string fileName() const;
    
    unsigned int fileStartDate() const;
    unsigned int fileStartTime() const;
    
    unsigned int fileEndDate();            ///< Date when writing has stopped.
    unsigned int fileEndTime() ;             ///< Time when writing has stopped.
    unsigned int eventsInFile() ;          ///< Number of events in this file.
    unsigned int dataMB_InFile() ;          ///< Number of megabytes in this file.
    unsigned int eventsInFileSequence() ;   ///< Number of events in this file sequence written so far.
    unsigned int dataMB_InFileSequence() ; ///< Number of MB in this file sequence written so far. 
    //unsigned int fileStatusWord() ;         ///< Indicates an end of sequence.

    unsigned int fileSizeLimitInDataBlocks() const ; 
    unsigned int fileSizeLimitInMB() const ;
    std::string appName() const ; 
    std::string fileNameCore() const ;  
    
    uint32_t lumiblockNumber();
    std::string stream();
    std::string projectTag();

    unsigned int latestDataNumber() const;
    
  private:
    std::string nextInContinuation(unsigned int expected);
    std::string sequenceFileName(bool isReady) const;


  private:
    std::stack<EventStackLayer*> m_stack;
    fRead* m_fR;
    bool m_sequenceReading; 
    bool m_finished;
    FESLMultiFile* m_filehandler; //the file handler //now unused
    bool m_last_file_finished;
    EventStackLayer* m_responsibleForMetaData;
    EventStackLayer* m_markForDeletion;
    
    unsigned int m_sizeOfFileNumber;
    unsigned int m_cFileNumber;
    std::string m_fileNameCore;
    std::string m_fileName;
  };
  
  // fRead *fReadFactory();
}

void mythrowE(std::string errormessage);

#endif 

