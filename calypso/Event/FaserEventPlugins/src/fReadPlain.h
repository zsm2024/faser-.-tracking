
#ifndef FREADPLAIN_H
#define FREADPLAIN_H

#include <fstream>
#include <string>

#include "FaserEventStorage/fRead.h"

class fReadPlain : public fRead
//class fReadPlain 
{
 public:
  fReadPlain(); 
  ~fReadPlain();

  bool isOpen();
  bool isEoF();
  bool fileExists(std::string fName) const;
  void openFile(std::string fName);
  void closeFile();
  void readData(char *buffer, unsigned int sizeBytes);
  int64_t getPosition();
  void setPosition(int64_t p);
  void setPositionFromEnd(int64_t p);
  fRead * newReader() const; 

 private:
  std::ifstream m_cFile; // current file
};

// fRead *fReadFactory();

#endif 

