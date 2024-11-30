/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
  -*-c++-*-
*/

/**
   @class      DumpFrags
   @brief      Dump header and payload fragments
*/

#ifndef BYTESTREAMCNVSVC_DUMPFRAGS
#define BYTESTREAMCNVSVC_DUMPFRAGS

#include <iostream> 
#include "EventFormats/DAQFormats.hpp"
#include "EventFormats/TLBDataFragment.hpp"
#include "EventFormats/TLBMonitoringFragment.hpp"
#include "EventFormats/TrackerDataFragment.hpp"
#include "EventFormats/DigitizerDataFragment.hpp"

using namespace DAQFormats;

class DumpFrags {

  public:
    DumpFrags() {} 

  /// dump fragments from FullEventFragment
static void dump(const EventFull* re)
  {

    std::cout << " ---EventFull run number = " 
	      << re->run_number() << " event number = " << re->event_counter()
	      << " Size = " << re->size() << std::endl;
    
    std::cout<<" Event id  "<<re->event_id()<<std::endl;
    std::cout<<" Event tag  "<<static_cast<uint32_t>(re->event_tag())<< " [";
    switch (re->event_tag()) {
      case PhysicsTag:
        std::cout << "Physics]" << std::endl;
        break;
      case CalibrationTag:
        std::cout << "Calibration]" << std::endl;
        break;
      case MonitoringTag:
        std::cout << "Monitoring]" << std::endl;
        break;
      case TLBMonitoringTag:
        std::cout << "TLBMonitoring]" << std::endl;
        break;
      case CorruptedTag:
        std::cout << "Corrupted]" << std::endl;
        break;
      case IncompleteTag:
        std::cout << "Incomplete]" << std::endl;
        break;
      case DuplicateTag:
        std::cout << "Duplicate]" << std::endl;
        break;
      default:
        std::cout << "Unknown]" << std::endl;
    }

    std::cout<<" Status   "<<static_cast<uint32_t>(re->status())<<std::endl;
    std::cout<<" Bunch crossing id   "<<re->bc_id()<<std::endl;
    std::cout<<" Timestamp in microseconds  "<<re->timestamp()<<std::endl;
    std::cout<<" Trigger bits   " << std::hex << re->trigger_bits()<<std::dec << std::endl;

    for(const auto &id : re->getFragmentIDs()) {
      const EventFragment* frag=re->find_fragment(id);
      std::cout << *frag << std::endl;
      
      switch (frag->source_id()&0xFFFF0000) {
        case TriggerSourceID:
          if (re->event_tag() == PhysicsTag ) {
            std::cout<<"TLB data fragment:"<<std::endl;
            TLBDataFormat::TLBDataFragment tlb_data_frag = TLBDataFormat::TLBDataFragment(frag->payload<const uint32_t*>(), frag->payload_size());
            std::cout<<tlb_data_frag<<std::endl;
          }
          else if (re->event_tag() == TLBMonitoringTag ) {
            TLBMonFormat::TLBMonitoringFragment tlb_mon_frag = TLBMonFormat::TLBMonitoringFragment(frag->payload<const uint32_t*>(), frag->payload_size());
            std::cout<<"TLB monitoring fragment:"<<std::endl;
            std::cout<<tlb_mon_frag<<std::endl;
          }
          break;
        case TrackerSourceID: 
          if (re->event_tag() == PhysicsTag) {
            std::cout << "Tracker data fragment: " << (frag->source_id() & 0x0000FFFF) << std::endl;
            // TrackerDataFragment::set_debug_on();
            TrackerDataFragment tracker_data_frag {frag->payload<const uint32_t*>(), frag->payload_size()};
            std::cout << tracker_data_frag << std::endl;
          }
          break;
        case PMTSourceID:
          if (re->event_tag() == PhysicsTag ) {
            std::cout<<"Digitizer data fragment:"<<std::endl;
            DigitizerDataFragment digitizer_data_frag = DigitizerDataFragment(frag->payload<const uint32_t*>(), frag->payload_size());
            std::cout<<digitizer_data_frag<<std::endl;
          }
          break;
        default:
          const uint32_t* payload=frag->payload<const uint32_t *>();
          unsigned int ii=0;
          for(;ii<frag->payload_size()/4;ii++) {
                  if (ii%8==0) std::cout<<" ";
                  std::cout<<" 0x"<<std::setw(8)<<std::hex<<std::setfill('0')<<payload[ii];
                  if (ii%8==7) std::cout<<std::endl;
          }
          if (ii%8!=0) std::cout<<std::endl;
          std::cout<<std::dec<<std::setfill(' ');
          break;
      }
    }
  }  
};
  
#endif 

