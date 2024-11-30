/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "WaveRawEvent/RawWaveform.h"
#include "EventFormats/DigitizerDataFragment.hpp"

// Default consdtructor 
//
RawWaveform::RawWaveform( ) :
  m_board_id(0),
  m_board_fail_flag(1),
  m_pattern_trig_options(0),
  m_channel_mask(0),
  m_event_counter(0),
  m_trigger_time_tag(0),
  m_samples(0),
  m_channel(0),
  m_adc_counts(),
  m_ID(0xffff),
  m_range(2.)
{

}

RawWaveform::~RawWaveform() {}

void
RawWaveform::setIdentifier(Identifier id) {
  m_ID = id.get_identifier32();
}

void
RawWaveform::setHeader(const DigitizerDataFragment* frag) {
  m_board_id = frag->board_id();
  m_pattern_trig_options = frag->pattern_trig_options();
  m_channel_mask = frag->channel_mask();
  m_event_counter = frag->event_counter();
  m_trigger_time_tag = frag->trigger_time_tag();
  m_samples = frag->n_samples();
}

void
RawWaveform::setWaveform(unsigned int channel, const std::vector<uint16_t> waveform) {
  m_channel = channel;

  // Make sure the vector is empty first
  m_adc_counts.clear();

  for(uint16_t count: waveform) 
    m_adc_counts.push_back(count);
}

std::ostream 
&operator<<(std::ostream &out, const RawWaveform &wfm) {
  out << "Waveform data channel:" << std::dec << wfm.channel() << std::endl;
  out << std::setw(30) << " board_id:             "<<std::setfill(' ')<<std::setw(32)<<std::dec<<wfm.board_id()<<std::setfill(' ')<<std::endl;
  out << std::setw(30) << " board_fail_flag:      "<<std::setfill(' ')<<std::setw(32)<<std::dec<<wfm.board_fail_flag()<<std::setfill(' ')<<std::endl;
  out << std::setw(30) << " board_trig_options:   "<<std::setfill(' ')<<std::setw(32)<<std::dec<<wfm.pattern_trig_options()<<std::setfill(' ')<<std::endl;
  out << std::setw(30) << " channel_mask:         "<<std::setfill(' ')<<std::setw(32)<<std::hex<<wfm.channel_mask()<<std::setfill(' ')<<std::endl;
  out << std::setw(30) << " event_counter:        "<<std::setfill(' ')<<std::setw(32)<<std::dec<<wfm.event_counter()<<std::setfill(' ')<<std::endl;
  out << std::setw(30) << " trigger_time_tag:     "<<std::setfill(' ')<<std::setw(32)<<std::dec<<wfm.trigger_time_tag()<<std::setfill(' ')<<std::endl;
  out << std::setw(30) << " n_samples:            "<<std::setfill(' ')<<std::setw(32)<<std::dec<<wfm.n_samples()<<std::setfill(' ')<<std::endl;
  out << std::setw(30) << " adc_counts.size():    "<<std::setfill(' ')<<std::setw(32)<<std::dec<<wfm.adc_counts().size()<<std::setfill(' ')<<std::endl;

  return out;
}
