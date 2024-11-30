#include "FaserEventStorage/fRead.h"
#include "ESLMultiFile.h"
//#include "ESLMergeFile.h"
#include "ESLOriginalFile.h"
#include "DataReaderController.h"
//#include "MergedRawFile.h"
#include "EventStorage/RawFileName.h"

FESLMultiFile::FESLMultiFile(fRead* fR) : EventStackLayer(fR) {
  m_me = "FESLMulti"; 
  ERS_DEBUG(3, identify() << " Constructor called booya");
  m_currentFileIndex = 0 ;
  m_sumsize = 0;
  m_rawfilename = NULL;
}  

FESLMultiFile::FESLMultiFile(){ 
  m_rawfilename = NULL;
}  


//This methods takes the next string from the vector, opens, the file and tries to look at the starting marker. According to this, it decides if the file is an originalfile or a mergefile.

EventStackLayer* FESLMultiFile::openNext()
{
  
  EventStackLayer* nextESL = 0;
  
  //m_currentFileIndex++; //this is already done in moreEvents;

 
  if ( m_fR->isOpen() ) {
    ERS_DEBUG(3, " Closing open file ");
    m_fR->closeFile();
  }

  std::string next = m_filenames.at(m_currentFileIndex);

  ERS_DEBUG(3, this->identify() << " Trying to open next file: " << next);

  m_fR->openFile(next);
  
  ERS_DEBUG(3, this->identify() << " file opened " << next);
    
  m_fR->setCurrentEndOfFile(-1);

  ERS_DEBUG(3, this->identify() << " set current end of " << next);

  //m_rawfilename = new daq::RawFileName(next);

  // ET - no marker?
  //uint32_t marker = m_fR->readuint32_t();


  ERS_DEBUG(3, this->identify() << " reading start " << next);

  // ET - comment out everything related to reading file start marker
  //if ( marker == FILE_START_MARKER) //It's an Original File
  //  {
      ERS_DEBUG(1, this->identify() << " found original file");

      FESLOriginalFile* tmpofp = new FESLOriginalFile(m_fR);
      tmpofp -> setFile(next);
      tmpofp -> handleOffset();
      tmpofp -> setResponsibleForMetaData();
      
      nextESL = tmpofp;
  //  }
  // elseif ( marker == MergedRawFile::FileMarker ) //It's a Merged File
  //   {

  //     ERS_DEBUG(1, this->identify() << " found merge file");

  //     ESLMergeFile* tmpmf = new ESLMergeFile(m_fR);
  //     tmpmf -> setFile(next);
  //     tmpmf -> handleOffset();
  //     tmpmf -> setResponsibleForMetaData();
      
  //     nextESL = tmpmf;
  //   }
  // else
  //   {
  //     FaserEventStorage::ES_WrongFileFormat ci(ERS_HERE, "file is of no know format. Abort.");
  //     throw ci;
      
  //   }
  
  
  nextESL->prepare();

  // This looks for file-end sequence (which we don't have) 
  // but it is caught, so OK
  try
    {
      m_sumsize += nextESL->dataMB_InFile();
    }
  catch(...)
    {
      
    }

  return nextESL;
  
}

bool FESLMultiFile::handlesContinuation()
{
  return true;
}

void FESLMultiFile::prepare()
{
  return;
}

bool FESLMultiFile::doneLoading()
{
  //multifile cannot be the last level, since it has to load either the mergefile or the originalfile.
  return false;
}

void FESLMultiFile::advance()
{
  m_currentFileIndex++;
  ERS_DEBUG(1, this->identify() << " advancing to next file " << m_currentFileIndex);
}

bool FESLMultiFile::moreEvents() 
{
  ERS_DEBUG(3, this->identify() << " asked for more events ");


  if (m_filenames.size() == 0 ) return false;
  if (m_currentFileIndex > m_filenames.size()-1) 
    {
      m_finished = true;
      return false;
    }
  return true;
}

unsigned int FESLMultiFile::getSum()
{
  return m_sumsize;
  
}


void FESLMultiFile::setContinuationFile(std::string next) 
{
  ERS_DEBUG(2, " Pushing : " << next << " on " << identify());
  m_filenames.push_back(next);
  ERS_DEBUG(2, " size now : " << m_filenames.size() );
}

void FESLMultiFile::setFile(std::string filename)
{
  m_filenames.push_back(filename);
}

void FESLMultiFile::setFiles(std::vector<std::string> filenames)
{
  m_filenames = filenames;
}



