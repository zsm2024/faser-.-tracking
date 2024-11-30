/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef RAWWAVEFORMCONTAINER_P0_H
#define RAWWAVEFORMCONTAINER_P0_H

// Persistent represenation of a RawWaveformContainer.

#include <vector>
#include "WaveEventAthenaPool/RawWaveform_p0.h"

class RawWaveformContainer_p0 {
 public:
  RawWaveformContainer_p0();
  friend class RawWaveformContainerCnv_p0;
 private:
  bool         m_board_fail_flag;
  unsigned int m_board_id;
  unsigned int m_pattern_trig_options;
  unsigned int m_channel_mask;
  unsigned int m_event_counter;
  unsigned int m_trigger_time_tag;
  unsigned int m_samples;
  std::vector<RawWaveform_p0> m_waveforms;
};

inline
RawWaveformContainer_p0::RawWaveformContainer_p0() : m_board_fail_flag(0),
			  m_board_id(0), 
			  m_pattern_trig_options(0),
			  m_channel_mask(0),
			  m_event_counter(0),
			  m_trigger_time_tag(0),
			  m_samples(0) {
  m_waveforms.clear();
}

#endif
