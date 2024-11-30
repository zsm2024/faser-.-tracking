#ifndef FASERESLMULTIFILE_H
#define FASERESLMULTIFILE_H


#include "FaserEventStorage/fRead.h"
#include "EventStackLayer.h"
#include "EventStorage/RawFileName.h"
#include <vector>

using namespace FaserEventStorage;

class FESLMultiFile : public EventStackLayer
{
 protected:
  FESLMultiFile();

 public:
  FESLMultiFile(fRead* fR);

  
  EventStackLayer* openNext();
  
  void prepare();
  bool doneLoading();
  
  bool moreEvents() ;
  bool handlesContinuation()  ;

  void setContinuationFile(std::string next) ;

  void advance();

  unsigned int getSum();
  
  //distinct methods
  void setFile(std::string filename);
  void setFiles(std::vector<std::string> filenames);

  

 private:
  std::vector<std::string> m_filenames;
  unsigned int m_currentFileIndex;
  unsigned int m_sumsize;
  daq::RawFileName *m_rawfilename;

};


#endif 

