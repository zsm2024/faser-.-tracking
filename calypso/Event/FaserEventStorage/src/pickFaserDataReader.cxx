#include "ers/ers.h"
#include "FaserEventStorage/fRead.h" 
#include "FaserEventStorage/loadfRead.h"
#include "FaserEventStorage/pickFaserDataReader.h"
#include "FaserEventStorage/EventStorageIssues.h"
#include "DataReaderController.h"

DataReader * pickFaserDataReader(std::string fileName) {
  
  ERS_DEBUG(1,"pickFaserDataReader(file) with file name "<<fileName);

  std::vector<std::string> fReadLibs;

  // Control plugins libraries via file name prefixes.
  // Take the prefixes away from the name, in most cases anyway.

  if(fileName.find("root:")==0) {
    // Leave the prefix in the file name in this case.
    fReadLibs.push_back("fReadXRootD");
  } else if(fileName.find("disk:")==0) {
    fileName.erase(0,std::string("disk:").size()); 
    fReadLibs.push_back("fReadPlain"); 
  } else if(fileName.find("https:")==0) {
    // Leave the prefix in the file name in this case.
    fReadLibs.push_back("fReadDavix");
  } else if(fileName.find("davs:")==0) {
    // Leave the prefix in the file name in this case.
    fReadLibs.push_back("fReadDavix");
  } else { // by defaul all will be tried
    fReadLibs.push_back("fReadPlain");
    fReadLibs.push_back("fReadXRootD");
    fReadLibs.push_back("fReadDavix");
  }

  ERS_DEBUG(2,"After parsing the file name is "<<fileName);
  ERS_DEBUG(2,"Number of fRead plugins to try is "<<fReadLibs.size());

  std::vector<std::string>::iterator fReadLib=fReadLibs.begin();
  fRead *pfR = NULL;

  std::vector<std::string> libsNotLoaded;
  std::vector<std::string> libsNoSuccess;
  libsNotLoaded.clear(); libsNoSuccess.clear();

  for(fReadLib=fReadLibs.begin(); fReadLib!=fReadLibs.end(); ++fReadLib) {
    ERS_DEBUG(2,"Try to with lib "<<(*fReadLib));

    pfR = loadfRead(*fReadLib);
    if (!pfR) {
      ERS_DEBUG(2,"No fRead");
      libsNotLoaded.push_back(*fReadLib);
      continue;
    }
    ERS_DEBUG(2,"fRead* OK. Try to open the data file "<<fileName);
    
    pfR->openFile(fileName);

    ERS_DEBUG(2,"Is the file open? "<<pfR->isOpen());
    if(!pfR->isOpen()) {
      ERS_DEBUG(2,"Can not open file "<<fileName<<" with library "<<(*fReadLib));
      libsNoSuccess.push_back(*fReadLib);
      delete pfR;
      pfR=NULL;
      continue;    
    } else {
      break;
    }
  }

  if(pfR==NULL || !pfR->isOpen()) {
    if(libsNoSuccess.size()>0) {
      std::string libList="";
      std::vector<std::string>::iterator it;
      for(it = libsNoSuccess.begin(); it != libsNoSuccess.end(); ++it) {
	libList += *it + " ";
      } 
      //ERS_ERROR("Can not open file "<<fileName
      //<<"\nThe following libraries were tried: "<<libList);
      std::stringstream mystream;
      mystream << "can not open file "<<fileName
               <<"\nThe following libraries were tried: "<<libList;
      std::string err = mystream.str();
      ReadingIssue ci(ERS_HERE, err.c_str());
      ers::error(ci);
    } 
    if(libsNotLoaded.size()>0) {
      std::string libList="";
      std::vector<std::string>::iterator it;
      for(it = libsNotLoaded.begin(); it != libsNotLoaded.end(); ++it) {
libList += *it + " ";
      } 
      //ERS_WARNING("Can not load libraries: "<<libList);
      std::string err = "can not load libraries: " + libList;
      ReadingIssue ci(ERS_HERE, err.c_str());
      ers::warning(ci);
    } 
    
    return NULL;
  }

  // We have a working fRead and the file is accessible.

  ERS_DEBUG(1,"The file is accessible. Go through file format now.");

  pfR->closeFile();

  DataReader * pDR = NULL;
  try{
    pDR = new DataReaderController(pfR,fileName);
  }catch(FaserEventStorage::ES_WrongFileFormat &ex){
    ReadingIssue ci(ERS_HERE,"file format not valid for EventStorage.", ex); 
    ers::warning(ci);
    return NULL;
  }

  ERS_DEBUG(1,"File format is OK.");

  return pDR;
}

