#ifndef COMPRESS_DATA_ZSTD
#define COMPRESS_DATA_ZSTD
#include <zstd.h>
#include <vector>
#include <iostream>
class ZstdEngine
{
public:
static bool zstdDecompressOptimized(const uint8_t* compressedFragmentsPointer,size_t sizeCompressed, std::vector<uint8_t>& outputFragments)
{
      // DEBUG("Detected ZSTD Compression in Event");
      const size_t maxDecompressedSize = ZSTD_getFrameContentSize(compressedFragmentsPointer, sizeCompressed);
      if (maxDecompressedSize == ZSTD_CONTENTSIZE_ERROR || maxDecompressedSize == ZSTD_CONTENTSIZE_UNKNOWN) {
          std::cerr << "Error: Invalid compressed data" << std::endl;
          return false;
      }

    outputFragments.resize(maxDecompressedSize);

    const size_t decompressedSize = ZSTD_decompress(outputFragments.data(), maxDecompressedSize,
                                                    compressedFragmentsPointer, sizeCompressed);
    if (ZSTD_isError(decompressedSize)) {
        std::cerr << "Error: Failed to decompress data: " << ZSTD_getErrorName(decompressedSize) << std::endl;
        return false;
    }

    outputFragments.resize(decompressedSize);
    return true;
}
};
#endif
