#ifndef COMPRESS_DATA_LZ4
#define COMPRESS_DATA_LZ4
#include <vector>
#include <iostream>
#include <lz4hc.h>
class lz4Engine
{
public:
bool static lz4DecompressOptimized(const uint8_t* compressedFragmentsPointer,size_t sizeCompressed, std::vector<uint8_t>& outputFragments)
    {
     
    const char* inputBuffer = reinterpret_cast<const char*>(compressedFragmentsPointer);
    size_t inputSize = sizeCompressed;
    size_t estimatedDecompressedSize = sizeCompressed * 3;
    int decompressedSize = 0;

    // Loop until the decompressed size is not larger than the estimated size
    int MAX_RETRY = 3;
    int retryCount = 0;
    while (static_cast<size_t>(decompressedSize) <= estimatedDecompressedSize && retryCount < MAX_RETRY) {
        outputFragments.resize(estimatedDecompressedSize);
        decompressedSize = LZ4_decompress_safe(inputBuffer,
                                               reinterpret_cast<char*>(outputFragments.data()),
                                               inputSize,
                                               estimatedDecompressedSize);
        if (decompressedSize >= 0) {
            // Decompression successful
            outputFragments.resize(static_cast<size_t>(decompressedSize));
            return true;
        }
        estimatedDecompressedSize *= 2;
        retryCount++;
    }
    return false;
}
};
#endif