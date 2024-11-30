#pragma once
#include "DAQFormats.hpp"
#define ZSTD_LEVEL 5
#define LZ4_DEFAULT_LEVEL_MODULE 5
class DataCompression
{
public:
DataCompression()
{}
virtual ~DataCompression()
{}
virtual bool compress(std::unique_ptr<DAQFormats::EventFull>& inputEvent) = 0;
virtual void setCompressionLevel(std::string level) = 0;
};
class ZSTDCompress : public DataCompression
{
  public:
  ZSTD_CCtx* ctx;
  int compressionLevel;
  ZSTDCompress():DataCompression()
  {
    ctx =  ZSTD_createCCtx();
    compressionLevel = ZSTD_LEVEL;
  };
  bool compress(std::unique_ptr<DAQFormats::EventFull>& inputEvent) 
  {
    std::vector<uint8_t>* eventFragments = inputEvent->raw_fragments();
    std::vector<uint8_t> outputevent;
    const size_t maxOutputSize = ZSTD_compressBound(eventFragments->size());
    outputevent.resize(maxOutputSize);
    size_t compressedSize = ZSTD_compressCCtx(ctx, outputevent.data(), maxOutputSize, eventFragments->data(), eventFragments->size(), compressionLevel);
    if (ZSTD_isError(compressedSize)) {

        ERROR("Error: zstd compression failed: " << ZSTD_getErrorName(compressedSize));
        return false;
    }
    outputevent.resize(compressedSize);
    
    inputEvent->loadCompressedData(outputevent,DAQFormats::ZstdCode);

    std::vector<uint8_t>().swap(outputevent);
    delete eventFragments;

    return true;
  };
  void setCompressionLevel(std::string level)
  {
    this->compressionLevel = std::stoi(level);
    return;
  };
  ~ZSTDCompress()
  {ZSTD_freeCCtx(ctx);};
};
class ZlibCompress: public DataCompression
{
  public:
    z_stream stream;
    int compressionLevel = Z_DEFAULT_COMPRESSION;
    int bufferSize; 
  ZlibCompress():DataCompression()
  {
    compressionLevel = Z_DEFAULT_COMPRESSION;
  };
  bool compress(std::unique_ptr<DAQFormats::EventFull>& inputEvent)
  {
    std::vector<uint8_t>* eventFragments = inputEvent->raw_fragments();
    size_t compressedSize = 0;
    std::vector<uint8_t> outputevent;
    stream.next_in = eventFragments->data();
    stream.avail_in = eventFragments->size();
    if (deflateInit(&stream, this->compressionLevel) != Z_OK)
        return false;
    std::vector<uint8_t> compressedBuffer(static_cast<size_t>(this->bufferSize));
    do {
        stream.next_out = compressedBuffer.data();
        stream.avail_out = compressedBuffer.size();
        int result = deflate(&stream, Z_FINISH);

        if (result == Z_STREAM_ERROR) {
            deflateEnd(&stream);
            return false;
        }
        compressedSize = compressedBuffer.size() - stream.avail_out;

        // Append the compressed data to the result vector
        outputevent.insert(outputevent.end(), compressedBuffer.begin(), compressedBuffer.begin() + static_cast<long int>(compressedSize));
    } while (stream.avail_out == 0);
    inputEvent->loadCompressedData(outputevent,DAQFormats::ZlibCode);
    std::vector<uint8_t>().swap(outputevent);
    delete eventFragments;
    return true;
  };
  void setCompressionLevel(std::string level)
  {
    this->compressionLevel = std::stoi(level);
    return;
  };
  ~ZlibCompress()
  {deflateEnd(&stream);};
};
class LZ4Compress: public DataCompression
{
  public:
  int compressionLevel;
  LZ4Compress():DataCompression()
  {
    compressionLevel = LZ4_DEFAULT_LEVEL_MODULE;
  };
  bool compress(std::unique_ptr<DAQFormats::EventFull>& inputEvent) // only keep one FIXME
  {
    std::vector<uint8_t>* eventFragments = inputEvent->raw_fragments();
    LZ4_streamHC_t* lz4Stream = LZ4_createStreamHC();
    int compressedSize = 0;
    std::vector<uint8_t> outputevent;
    size_t maxCompressedSize = static_cast<size_t>(LZ4_compressBound(eventFragments->size()));
    outputevent.resize(maxCompressedSize);
    compressedSize = LZ4_compress_HC(reinterpret_cast<const char*>(eventFragments->data()),
                                         reinterpret_cast<char*>(outputevent.data()),
                                         eventFragments->size(),
                                         maxCompressedSize,
                                         this->compressionLevel);
    if (compressedSize <= 0) {
        return false;  // Compression failed
    }
    LZ4_freeStreamHC(lz4Stream);
    outputevent.resize(static_cast<size_t>(compressedSize));
    inputEvent->loadCompressedData(outputevent,DAQFormats::Lz4Code);
    
    std::vector<uint8_t>().swap(outputevent);
    delete eventFragments;
    return true;
  };
  void setCompressionLevel(std::string level)
  {
    this->compressionLevel = std::stoi(level);
    return;
  };
  ~LZ4Compress()
  {};
};