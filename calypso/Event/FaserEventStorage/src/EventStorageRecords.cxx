//
// functions to convert records to strings
// for debugging 
// 
#include <sstream>

#include "FaserEventStorage/EventStorageRecords.h"

std::string FaserEventStorage::string_record(void *ri, const void *pi) {

  std::ostringstream s;
  
  uint32_t *record = (uint32_t *)ri;
  uint32_t *pattern = (uint32_t *)pi;
  int size=pattern[1];

  for(int i=0; i<size; i++) {
    if(pattern[i] != 0) {
      s << std::hex << pattern[i] << std::dec << " ";
    } else {
      s << record[i] << " ";
    }
  }
  
  std::string rs = s.str();   
  return rs;    
}

std::string FaserEventStorage::string_record(FaserEventStorage::file_name_strings nst) {
  
  std::ostringstream s;

  s << std::hex << FaserEventStorage::file_name_strings_marker << std::dec;

  s << " " << nst.appName.size();
  s << " " << nst.appName;

  char ns = nst.appName.size() % 4;
  if(ns) s.write("____",4-ns);

  s << " " << nst.fileNameCore.size();
  s << " " << nst.fileNameCore;

  ns = nst.fileNameCore.size() % 4;
  if(ns) s.write("____",4-ns);

  std::string rs = s.str();   
  return rs;    

}

void FaserEventStorage::reset_record(void *ri, const void *pi) {

  uint32_t *record = (uint32_t *)ri;
  uint32_t *pattern = (uint32_t *)pi;
  int size=pattern[1];

  for(int i=0; i<size; i++) record[i] = pattern[i];
}
