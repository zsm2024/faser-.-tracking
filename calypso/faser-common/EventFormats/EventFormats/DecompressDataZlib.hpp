#ifndef COMPRESS_DATA_ZLIB
#define COMPRESS_DATA_ZLIB
#include <vector>
#include <iostream>
#include <zlib.h>
class ZlibEngine
{
public:
bool static zlibDecompressOptimized(const uint8_t* compressedFragmentsPointer,size_t sizeCompressed, std::vector<uint8_t>& outputFragments)
{
      // DEBUG("Detected Zlib Compression in Event");
      outputFragments.clear();
      uLongf decompressedSize = sizeCompressed * 3;
      outputFragments.resize(decompressedSize);
      int result = uncompress(outputFragments.data(), &decompressedSize, compressedFragmentsPointer, sizeCompressed);

      if (result != Z_OK) {
          return false;
      }
      outputFragments.resize(decompressedSize);
      return true;
}
};
#endif