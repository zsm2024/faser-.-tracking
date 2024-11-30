/**
 \brief an interface for a call back class
 \file EventStorage/FileNameCallback.h

 a class to separate the generation of file names from the DataWriter

 \author Cyril Topfel
 \date December 2009
*/

#ifndef FASEREVENTSTORAGE_FILENAMECALLBACK_H
#define FASEREVENTSTORAGE_FILENAMECALLBACK_H

#include <string>

namespace FaserEventStorage {
  
class FileNameCallback
{
 public:
  virtual ~FileNameCallback() {};

  virtual void advance() = 0;
  virtual std::string getCurrentFileName(bool writing=false) = 0;
  virtual std::string getCoreName() = 0;
  /*  virtual std::string getFileNameByIndex(int index);*/
  virtual unsigned int getIndex() = 0;
  virtual void fileAlreadyExists() = 0;
  
  

};

}
#endif // EVENTSTORAGE_FILENAMECALLBACK_H




