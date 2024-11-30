/*
  Copyright (C) 2019-2020 CERN for the benefit of the FASER collaboration
*/

///////////////////////////////////////////////////////////////////
// DAQFormats.hpp, (c) FASER Detector software
///////////////////////////////////////////////////////////////////
#pragma once

#include <stdint.h>
#include <stdexcept>
#include <chrono>
#include <vector>
#include <iomanip>
#include <map>
#include <fstream>
#include "Exceptions/Exceptions.hpp"
#include "DecompressDataZstd.hpp"
#include "DecompressDataZlib.hpp"
#include "DecompressDataLz4.hpp"
#include "Logging.hpp"
using namespace std::chrono_literals;
using namespace std::chrono;
CREATE_EXCEPTION_TYPE(EFormatException,DAQFormats)
CREATE_EXCEPTION_TYPE(CompressionDataException,DAQFormats)

namespace DAQFormats {
  typedef std::vector<uint8_t> byteVector;
  
  //FIXME: add Doxygen

  enum EventTags {
    PhysicsTag = 0x00,
    CalibrationTag = 0x01,
    MonitoringTag = 0x02,
    TLBMonitoringTag = 0x03,
    MaxRegularTag = 0x03,  //  to be updated if new tags are added!
    CorruptedTag = 0x10,
    IncompleteTag = 0x11,
    DuplicateTag = 0x12,
    MaxAnyTag = 0x13       //  to be updated if new error tags are added! Note that is last tag value+1 !
  };

  enum SourceIDs {
    TriggerSourceID = 0x020000,
    TrackerSourceID = 0x030000,
    PMTSourceID     = 0x040000,
    BOBRSourceID    = 0x050000
  };
  
  const uint16_t EventHeaderVersion = 0x0001;

  enum EventStatus { // to be reviewed as not all are relevant for FASER
    UnclassifiedError = 1,
    BCIDMismatch = 1<<1,
    TagMismatch = 1<<2,
    Timeout = 1<<3,
    Overflow = 1<<4,
    CorruptedFragment = 1<<5,
    DummyFragment = 1<<6,
    MissingFragment = 1<<7,  
    EmptyFragment = 1<<8,
    DuplicateFragment = 1<<9,
    ErrorFragment = 1<<10,    // used by event builder to wrap incoming non-deciphable data
    Compressed = 1 << 11 // Indicates a Compressed event
  };

  enum CompressionCode {
    NotCompressed=0x00,
    ZstdCode = 0x01,
    ZlibCode = 0x02,
    Lz4Code = 0x03,
    ZstdDictCode = 0x0A,
    Lz4DictCode = 0x0B,
    BrotliCode = 0x04
  };

  /** \brief This class define DAQ fragment header encapsulating raw data
   *  from the experiment. Encoding/decoding and access functions are provided
   */
 
  class EventFragment {
  public:
    friend inline std::ostream& operator<<(std::ostream &out, const  DAQFormats::EventFragment &frag);

    EventFragment() = delete;

    /** \brief Constructor given a detector payload 
     */
    EventFragment(uint8_t fragment_tag, uint32_t source_id,
		  uint64_t event_id, uint16_t bc_id, const void * payload, size_t payloadsize ) {
      microseconds timestamp;
      timestamp = duration_cast<microseconds>(system_clock::now().time_since_epoch());

      header.marker         = FragmentMarker;
      header.fragment_tag   = fragment_tag;
      header.trigger_bits   = 0;
      header.version_number = FragmentVersionLatest;
      header.header_size    = sizeof(struct EventFragmentHeader);
      header.payload_size   = payloadsize;
      header.source_id      = source_id;
      header.event_id       = event_id;
      header.bc_id          = bc_id;
      header.status         = 0;
      header.timestamp      = static_cast<uint64_t>(timestamp.count());
      fragment=byteVector(reinterpret_cast<const uint8_t *>(payload),
			  reinterpret_cast<const uint8_t *>(payload)+payloadsize);
    }
  
    /** \brief Constructor given an already encoded fragment
     */
    EventFragment(const uint8_t *data, size_t size, bool allowExcessData=false) {
      if (size<8) THROW(EFormatException,"Too little data for fragment header");
      const struct EventFragmentHeader* newHeader=reinterpret_cast<const struct EventFragmentHeader*>(data);
      if (newHeader->marker!=FragmentMarker) THROW(EFormatException,"No fragment header");
      if (newHeader->version_number!=FragmentVersionLatest) {
	//FIXMEL should do conversion here
	THROW(EFormatException,"Unsupported fragment version");
      }
      if (size<newHeader->header_size) THROW(EFormatException,"Too little data for fragment header");
      if (size<newHeader->header_size+newHeader->payload_size) THROW(EFormatException,"Too little data for fragment");
      if ((size!=newHeader->header_size+newHeader->payload_size)&&!allowExcessData) THROW(EFormatException,"fragment size does not match header information");
      size=newHeader->header_size+newHeader->payload_size;
      fragment=byteVector(data+newHeader->header_size,data+newHeader->header_size+newHeader->payload_size);
      header=*(reinterpret_cast<const struct EventFragmentHeader *>(data));
    }

    /// \brief Returns the payload as pointer of desired type
    template <typename T = void *> T payload() const {
      static_assert(std::is_pointer<T>(), "Type parameter must be a pointer type");
      return reinterpret_cast<T>(fragment.data());
    }
  
    /// Return fragment as vector of bytes
    const byteVector * raw() const {
      const uint8_t *rawHeader=reinterpret_cast<const uint8_t *>(&header);
      byteVector* data=new byteVector(rawHeader,rawHeader+sizeof(header));
      data->insert(data->end(),fragment.begin(),fragment.end());
      return data;
    }

    /// Append fragment to existing vector of bytes (for building events)
    void rawAppend(byteVector *data) const {
      const uint8_t *rawHeader=reinterpret_cast<const uint8_t *>(&header);
      data->insert(data->end(),rawHeader,rawHeader+sizeof(header));
      data->insert(data->end(),fragment.begin(),fragment.end());
    }
  
    /// Set status bits
    void set_status(uint16_t status) {
      header.status = status;
    } 

    /// Set trigger bits
    void set_trigger_bits(uint16_t trigger_bits) {
      header.trigger_bits = trigger_bits;
    } 

    /// Set fragment tag - should normally not be used
    void set_fragment_tag(uint8_t fragment_tag) {
      header.fragment_tag = fragment_tag;
    } 

    //getters here
    uint64_t event_id() const { return header.event_id; }
    uint8_t  fragment_tag() const { return header.fragment_tag; }
    uint32_t source_id() const { return header.source_id; }
    uint16_t bc_id() const { return header.bc_id; }
    uint16_t status() const { return header.status; }
    uint16_t trigger_bits() const { return header.trigger_bits; }
    uint32_t size() const { return header.header_size+header.payload_size; }
    uint16_t header_size() const { return header.header_size; }
    uint32_t payload_size() const { return header.payload_size; }
    uint64_t timestamp() const { return header.timestamp; }
    
  private:
    const uint16_t FragmentVersionLatest = 0x0001;
    /*
    Current Version
    00 01
    Compressed Fragment
    01 01
    */
    const uint8_t FragmentMarker = 0xAA; // indicates good raw data events
    struct EventFragmentHeader {
      uint8_t marker;
      uint8_t fragment_tag;
      uint16_t trigger_bits;
      uint16_t version_number;
      uint16_t header_size;
      uint32_t payload_size;
      uint32_t source_id; 
      uint64_t event_id;
      uint16_t bc_id;
      uint16_t status; 
      uint64_t timestamp;
    }  __attribute__((__packed__)) header;
    byteVector fragment;
  };

    /** \brief This class define DAQ event header encapsulating one or more
     *	event fragments. Encoding/decoding and access functions are provided
     */

  class EventFull {
  public:

    /** \brief Constructor with fragments to be added later
     *  Default parameeters allow this to be used as a default constructor as well
     */
    
    friend inline std::ostream& operator<<(std::ostream &out, const  DAQFormats::EventFull &ev);
    EventFull(uint8_t event_tag=IncompleteTag, unsigned int run_number=0, uint64_t event_number=0) {
      microseconds timestamp;
      timestamp = duration_cast<microseconds>(system_clock::now().time_since_epoch());

      header.marker         = EventMarker;
      header.event_tag      = event_tag;
      header.trigger_bits   = 0;
      header.version_number = EventVersionLatest;
      header.compression_code = NotCompressed;
      header.header_size    = sizeof(struct EventHeader);
      header.payload_size   = 0;
      header.fragment_count = 0;
      header.run_number     = run_number;
      header.event_id       = 0;
      header.event_counter  = event_number;
      header.bc_id          = 0xFFFF;
      header.status         = 0;
      header.timestamp      = static_cast<uint64_t>(timestamp.count());
    }

    /// \brief Constructor given an existing event in stream of bytes 
    EventFull(const uint8_t *data,size_t eventsize) {

      // Read EventHeader out of byte stream
      loadHeader(data, eventsize);

      // Skip forward by amount of data actually read
      uint32_t dataLeft = eventsize - header.header_size;
      data+=header.header_size;
      loadPayload(data, dataLeft);
    }

    /// \brief Constructor reading an existing event from a file stream
    // FIXME: no format migration support or for partially corrupted events
    EventFull(std::ifstream &in) {
      in.read(reinterpret_cast<char *>(&header),sizeof(header));
      if (in.fail()) THROW(EFormatException,"Too small to be event");
      if (header.marker!=EventMarker) THROW(EFormatException,"Wrong event header");
      if (header.version_number  != EventVersionLatest )
      {
	    //should do conversion here
	    THROW(EFormatException,"Unsupported event format version");
      }
      if (header.payload_size>1000000) THROW(EFormatException,"Payload size too large (>1000000)");
      uint8_t* data=new uint8_t[header.payload_size];
      uint8_t* full=data;
      in.read(reinterpret_cast<char *>(data),header.payload_size);
      if (in.fail()) THROW(EFormatException,"Event size does not match header information");

      uint32_t dataLeft=header.payload_size;
      loadPayload(data, dataLeft);
      delete [] full;
    }



    /// OR's new error flags into existing ones
    void processCompressedData(uint32_t dataLeft)
    {
      updatePayloadSize(dataLeft);
      clearCompression();
      clearCompressionAlgo();
    }
    void updateStatus(uint16_t status) {
      header.status|=status;
    }
    void updatePayloadSize(uint32_t size){
      header.payload_size=size;
    }
    void setCompression()
    {
      header.status|=Compressed;
    }
    void clearCompression()
    {
      header.status&=~Compressed;
    }
    bool isCompressed()
    {
      if(header.status & Compressed)
        return true;
      else
        return false;
    }
    void setCompressionAlgo(uint8_t algoCode)
    {
      header.compression_code =algoCode;  
    }

    void clearCompressionAlgo()
    {
      header.compression_code = 0x00;
    }
    
    /// Return full event as vector of bytes
    byteVector* raw() {
      const uint8_t *rawHeader=reinterpret_cast<const uint8_t *>(&header);
      byteVector* full=new byteVector(rawHeader,rawHeader+sizeof(header));
      /* if its a compressed event populate the byteVector with the contents of the 
      compressed payload
      */
      if(this->isCompressed())
      {
        // DEBUG("raw () invoked on compressed event");
        full->insert(full->end(),this->compressedData.begin(),this->compressedData.end());
        return full;
      }
      for(const auto& frag: fragments) {
	        frag.second->rawAppend(full); // Note the key of the hashMap is discarded
      }
      return full;
    }

    byteVector* raw_fragments() {
      byteVector* fragmentraw=new byteVector();
      for(const auto& frag: fragments) {
	        frag.second->rawAppend(fragmentraw);
      }
      return fragmentraw;
    }

    ~EventFull() {
      if(this->isCompressed())
      {
        // DEBUG("Since Fragments are cleared during compression Skipping that part");
        this->compressedData.clear();
      }
      else{
      for(const auto& frag: this->fragments) { // FIXME: should use smarter pointers for memory management
	    delete frag.second;
      }
      }
      
    }
    /** \brief Appends fragment to list of fragments in event
     *
     *  Ownership is taken of fragment, i.e. don't delete it later
     */
  static bool decompressPayload(uint8_t header_compressionCode,const uint8_t* compressedFragments,size_t compressedSize, std::vector<uint8_t>& outputFragments)
  {
    bool status;
  switch (header_compressionCode) {
  case ZstdCode:
    status = ZstdEngine::zstdDecompressOptimized(compressedFragments,compressedSize, outputFragments);
    return status;
    break;
  case ZstdDictCode:
  case Lz4DictCode:
    ERROR("Dictionary Compression is an experimental feature.\nDecompression of Events with Dictionary based compression is not supported");
    return false;
    break;
  case ZlibCode:
    status = ZlibEngine::zlibDecompressOptimized(compressedFragments,compressedSize, outputFragments);
    return status;
    break;
  case Lz4Code:
    status = lz4Engine::lz4DecompressOptimized(compressedFragments,compressedSize, outputFragments);
    return status;
    break;
  default:
    ERROR("Invalid Compression code detected");
    return false;
    break;
}
  return false; // redundant just in case something breaks
  }

    int16_t addFragment(const EventFragment* fragment) { // FIXME: should use smarter pointers for memory management
      int16_t status=0;
      if (fragments.find(fragment->source_id())!=fragments.end()) 
	    THROW(EFormatException,"Duplicate fragment addition!");
      fragments[fragment->source_id()]=fragment;
      if (!header.fragment_count) {
          header.bc_id=fragment->bc_id();
          header.event_id = fragment->event_id();
      }
      header.fragment_count++;
      header.trigger_bits |= fragment->trigger_bits(); //? should only come from trigger fragment - BP: add check?
      header.payload_size+=fragment->size();
      if (header.bc_id!=fragment->bc_id()) {
	status |= EventStatus::BCIDMismatch;
      }
      if (fragment->source_id()==SourceIDs::TriggerSourceID)
	header.bc_id=fragment->bc_id();  //TLB should be primary source BCID

      //BP: could check for event ID mismatch, but should not happen...

      updateStatus(fragment->status()|status);
      return status;
    }

    // \brief Load Compressed data into data member
    //  This method loads compressed data and sets the header.status to a flag indicating compression
    //  This updates the payload size to the new compressed payload size
    void loadCompressedData(std::vector<uint8_t>& compressedDataToLoad, uint8_t algoCode)
    {
      for(const auto& frag: fragments)
	    delete frag.second;
      this->compressedData.insert(this->compressedData.end(),compressedDataToLoad.begin(),compressedDataToLoad.end());
      this->updatePayloadSize(compressedData.size());
      this->setCompression();
      setCompressionAlgo(algoCode);
      
    }

    // \brief Load header from stream of bytes
    // Return actual size of header
    uint16_t loadHeader(const uint8_t *data, size_t datasize) {
      if (datasize<sizeof(struct EventHeader)) THROW(EFormatException,"Too small to be event");
      header=*reinterpret_cast<const struct EventHeader *>(data);
      if (header.marker!=EventMarker) THROW(EFormatException,"Wrong event header");
      if (header.version_number != EventVersionLatest){
	    THROW(EFormatException,"Unsupported event format version");
      }

      return header_size(); 
    }

    // \brief Load payload from stream of bytes
    void loadPayload(const uint8_t *data, size_t datasize) {
    std::vector<uint8_t> decompressed_data_vector;
    if (header.status & Compressed) // Compressed Event Detected
      { 
        // DEBUG("A Compressed Event is being read");
        
        // Detect Compression Algorithm And Decompress
        if (decompressPayload(header.compression_code,data,static_cast<size_t>(datasize),decompressed_data_vector ))
        {
          //uint8_t* decompressed_data = &decompressed_data_vector[0];
          data = &decompressed_data_vector[0];
          datasize = decompressed_data_vector.size();
          processCompressedData(datasize);
        }
        else
        {
            THROW(CompressionDataException,"DECOMPRESSION FAILED SKIPPING EVENT READ");
        }
      }
    if (datasize != header.payload_size) {
    THROW(EFormatException, "Payload size does not match header information");
    }
    for(int fragNum=0;fragNum<header.fragment_count;fragNum++) {
        EventFragment *fragment=new EventFragment(data,datasize,true);
        data+=fragment->size();
        datasize-=fragment->size();
        fragments[fragment->source_id()]=fragment;
      }
    }

    // getters here
    uint8_t event_tag() const { return header.event_tag; }
    uint16_t status() const { return header.status; }
    uint64_t event_id() const { return header.event_id; }
    uint64_t event_counter() const { return header.event_counter; }
    uint16_t bc_id() const { return header.bc_id; }
    uint32_t size() const { return header.header_size+header.payload_size; }
    uint16_t header_size() const { return header.header_size; }
    uint32_t payload_size() const { return header.payload_size; }
    uint64_t timestamp() const { return header.timestamp; }
    uint64_t run_number() const { return header.run_number; }
    uint16_t trigger_bits() const { return header.trigger_bits; }
    uint16_t fragment_count() const { return header.fragment_count; }
    uint8_t event_version() const { return header.version_number; }
    uint8_t event_compressionCode() const { return header.compression_code; }
    const uint8_t* event_CompressedDataStart() const {return compressedData.data();}
    size_t eventCompressedSize() const {return compressedData.size();}


    /// Get list of fragment source ids
    std::vector<uint32_t> getFragmentIDs() const
    {
      std::vector<uint32_t> ids;
      for (const auto &frag : fragments)
      {
        ids.push_back(frag.first);
      }
      return ids;
    }

    /// Find fragment with specific source id
    const EventFragment *find_fragment(uint32_t source_id) const
    {
      if (fragments.find(source_id) == fragments.end())
          return nullptr;
      return fragments.find(source_id)->second;
    }

  private:
    const uint8_t EventVersionLatest = 0x01;
    const uint8_t EventMarker = 0xBB;
    std::vector<uint8_t> compressedData;
    struct EventHeader {
      uint8_t marker;
      uint8_t event_tag;
      uint16_t trigger_bits;
      uint8_t version_number; 
      uint8_t compression_code; // Could be used to record algorithm for compression
      uint16_t header_size;
      uint32_t payload_size;
      uint8_t  fragment_count;
      unsigned int run_number : 24;
      uint64_t event_id;  
      uint64_t event_counter;
      uint16_t bc_id;
      uint16_t status; 
      uint64_t timestamp;
    }  __attribute__((__packed__)) header;
    std::map<uint32_t,const EventFragment*> fragments;
  };

  inline std::ostream &operator<<(std::ostream &out, const  DAQFormats::EventFragment &frag) {
    out<<" Fragment: tag="<<static_cast<int>(frag.fragment_tag())
      <<" source=0x"<<std::hex<<std::setfill('0')<<std::setw(4)<<std::hex<<frag.source_id()
      <<" bc="<<std::dec<<std::setfill(' ')<<std::setw(4)<<frag.bc_id()
      <<" status=0x"<<std::hex<<std::setw(4)<<std::setfill('0')<<frag.status()
      <<" payload="<<std::dec<<std::setfill(' ')<<std::setw(5)<<frag.payload_size()
      <<" bytes";
      return out;
  }

  inline std::ostream &operator<<(std::ostream &out, const  DAQFormats::EventFull &ev) {
      out<<"Event: "<<std::setw(8)<<ev.event_counter()<<" (0x"<<std::hex<<std::setfill('0') <<std::setw(8)<<std::right<<ev.event_id()<<") "
        <<std::setfill(' ')
        <<" run="<<std::dec<<ev.run_number()
        <<" tag="<<std::dec<<static_cast<int>(ev.event_tag())
        <<" bc="<<std::dec<<std::setw(4)<<ev.bc_id()
        <<" trig=0x"<<std::hex<<std::setfill('0')<<std::setw(4)<<std::right<<ev.trigger_bits()
        <<" status=0x"<<std::hex<<std::setw(4)<<static_cast<int>(ev.status())
        <<std::setfill(' ')
        <<" time="<<std::dec<<ev.timestamp()  //FIXME: should be readable
        <<" #fragments="<<ev.fragment_count()
        <<" payload="<<std::dec<<std::setw(6)<<ev.payload_size()
        <<" bytes";

      return out;
  }
}

#define customdatatypeList (DataFragment<EventFull>)(DataFragment<EventFragment>)
