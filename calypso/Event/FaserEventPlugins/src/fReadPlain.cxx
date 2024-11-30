#include "ers/ers.h"
#include "fReadPlain.h"
#include "EventStorage/EventStorageIssues.h"


fReadPlain::fReadPlain() 
{
} 

fReadPlain::~fReadPlain() 
{
  this->closeFile();
}

bool fReadPlain::isOpen() 
{
  return m_cFile.is_open();
}

bool fReadPlain::isEoF() 
{
  if(this->isOpen()) {
    return m_cFile.eof();
  } else {
    return false;
  }
}

bool fReadPlain::fileExists(std::string fName) const
{
  std::ifstream testFile;
  testFile.open(fName.c_str(),std::ios::binary | std::ios::in);

  bool isThere = testFile.good();

  if(isThere) testFile.close();
  
  return isThere;
}

void fReadPlain::openFile(std::string fName) 
{
  if(this->isOpen()) this->closeFile();

  m_cFile.clear();
  m_cFile.open(fName.c_str(),std::ios::binary | std::ios::in);
}  

void fReadPlain::closeFile() 
{
  m_cFile.close();
}

void fReadPlain::readData(char *buffer, unsigned int sizeBytes) 
{
  if (sizeBytes==0) return;

  if(!m_cFile.is_open()) {
    std::stringstream mystream;
    mystream << "an attempt to read from a file that is not open. "
	     <<"fReadPlain::readData called to read "
	     <<sizeBytes<<" bytes.";
    std::string err = mystream.str();
    EventStorage::ReadingIssue ci(ERS_HERE, err.c_str());
    ers::warning(ci);
    return;
  }

  if(m_cFile.eof()) {
    std::stringstream mystream;
    mystream << "error reading data from disk. "
	     <<"fReadPlain called to read "
	     <<sizeBytes<<" bytes and finds EOF.";
    std::string err = mystream.str();
    EventStorage::ReadingIssue ci(ERS_HERE, err.c_str());
    ers::warning(ci);
    return;
  }

  if(m_cFile.bad()) {
    std::stringstream mystream;
    mystream << "error reading data from disk. "
	     <<"fReadPlain called to read " 
	     <<sizeBytes<<" bytes and finds that the bad flag is set.";
    std::string err = mystream.str();
    EventStorage::ReadingIssue ci(ERS_HERE, err.c_str());
    ers::warning(ci);
    return;
  } 

  m_cFile.read(buffer,sizeBytes);

  if(m_cFile.bad()) {
    std::stringstream mystream;
    mystream << "error reading data from disk. "
             <<"fReadPlain finds the file in bad state after trying to read "
             <<sizeBytes<<" bytes.";
    std::string err = mystream.str();
    EventStorage::ReadingIssue ci(ERS_HERE, err.c_str());
    ers::warning(ci);
  } else if(m_cFile.eof()) {
    std::stringstream mystream;
    mystream << "error reading data from disk. "
             <<"fReadPlain finds EOF after trying to read "
             <<sizeBytes<<" bytes.";
    std::string err = mystream.str();
    EventStorage::ReadingIssue ci(ERS_HERE, err.c_str());
    ers::warning(ci);
  }
}

int64_t fReadPlain::getPosition() 
{
  if(this->isOpen()) {
    std::streampos pos=m_cFile.tellg();
    return pos;
  }
  return -1;
}

void fReadPlain::setPosition(int64_t p) 
{  
  if(this->isOpen()) {
    std::streampos pos=p;
    m_cFile.seekg(pos);
  }
}
 
void fReadPlain::setPositionFromEnd(int64_t p)
{  
  if(this->isOpen()) {
    m_cFile.seekg(p,std::ios::end);
  }
}
 
fRead * fReadPlain::newReader() const
{
  fReadPlain * nfr = new fReadPlain();
  return (fRead *)nfr;
}

extern "C" {
  fRead *fReadFactory() 
  {
    fReadPlain * nfr = new fReadPlain();
    return (fRead *)nfr;
  }
}
