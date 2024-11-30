//Dear emacs, this is -*- c++ -*-

#ifndef FASEREVENTSTORAGE_ESCOMPRESSION_H
#define FASEREVENTSTORAGE_ESCOMPRESSION_H

#include <string>
#include "ers/ers.h"

namespace FaserEventStorage {
      
  enum CompressionType { NONE, RESERVED, UNKNOWN, ZLIB};

  static const std::string compressiontag("Compression");
  
  CompressionType string_to_type(const std::string& type);
  std::string type_to_string(const CompressionType& type);

}

#endif
