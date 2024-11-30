#ifndef ESLORIGINALFILE_H
#define ESLORIGINALFILE_H


#include "EventStackLayer.h"
#include "FaserEventStorage/fRead.h"

using namespace FaserEventStorage;

// Forward declaration
namespace DAQFormats {
  class EventFull;
}

class FESLOriginalFile : public EventStackLayer
{

 public:
  FESLOriginalFile( fRead * fR);
  ~FESLOriginalFile( );

  EventStackLayer* openNext();
  EventStackLayer* loadAtOffset(int64_t position, EventStackLayer *old);
  
  void prepare();
  bool doneLoading();

  //DRError getData(unsigned int &eventSize, char **event, int64_t pos, bool memAlreadyAlloc, int64_t allocSizeInBytes);
  DRError getData(DAQFormats::EventFull*& theEvent, int64_t pos);
  
  bool moreEvents() ;
  //bool handlesOffset() ;
  //bool handlesContinuation() ;
  //std::string nextInContinuation() ;
  //bool endOfFileSequence() ;


  //additonal
  internal_run_parameters_record getRunParameterrecord();

  std::string GUID() const;
  
  unsigned int runNumber() const ;
  unsigned int maxEvents() const ;
  unsigned int recEnable() const ;
  unsigned int triggerType() const ;
  std::bitset<128> detectorMask() const ;
  unsigned int beamType() const ;
  unsigned int beamEnergy() const ;
  std::vector<std::string> freeMetaDataStrings() const;
  CompressionType compression() const;

  unsigned int fileStartDate() const ;
  unsigned int fileStartTime() const; 
  
  unsigned int fileEndDate() ;
  unsigned int fileEndTime() ;
  unsigned int eventsInFile() ;
  unsigned int dataMB_InFile() ;
  unsigned int eventsInFileSequence() ;
  unsigned int dataMB_InFileSequence();
  unsigned int fileStatusWord();
  bool endOfFileSequence() ;
  unsigned int nextInSequence(unsigned int current) ;

  uint32_t lumiblockNumber();
  std::string stream();
  std::string projectTag();

  unsigned int fileSizeLimitInDataBlocks() const ; 
  unsigned int fileSizeLimitInMB() const ;
  std::string appName() const ; 
  std::string fileNameCore() const ;  
  


  
 private:
  void readOptionalFreeMetaDataStrings();
  void checkCompression();
  file_end_record currentFileFER();
  
  file_name_strings m_latest_fns;
  
  bool m_endOfFile;
  
  int64_t m_cfilesize;
  
  file_start_record m_latest_fsr;
  data_separator_record m_latest_dsr;
  
  file_end_record m_latest_fer;
  bool m_advance_fer_updated ;
  uint32_t m_lumiblockNumber;
  std::string m_stream;
  std::string m_projectTag;

  file_name_strings unfile_name_strings();
  
  FaserEventStorage::freeMetaDataStrings m_fmdStrings;
  internal_run_parameters_record m_internal_latest_rpr;

  std::string extractFromMetaData(std::string token);
  
  FaserEventStorage::CompressionType m_compression;
  //compression::DataBuffer * m_uncompressed;
};


#endif
