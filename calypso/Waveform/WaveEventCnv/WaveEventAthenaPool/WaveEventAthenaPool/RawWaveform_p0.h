/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RAWWAVEFORM_P0_H
#define RAWWAVEFORM_P0_H

#include <vector>
#include <iostream>
#include <iomanip>

#include "Identifier/Identifier32.h"

class RawWaveform_p0 {
 public:
  RawWaveform_p0();

// List of Cnv classes that convert this into Rdo objects
  friend class RawWaveformCnv_p0;

 private:
  Identifier32::value_type m_ID;
  unsigned int m_channel;
  std::vector<unsigned int> m_adc_counts;

 public:
  void print() const {
    std::cout << "Persistent Waveform data:" << std::endl
	      << std::setw(30) << " channel:              "<<std::setfill(' ')<<std::setw(32)<<std::dec<<m_channel<<std::setfill(' ')<<std::endl
	      << std::setw(30) << " length:               "<<std::setfill(' ')<<std::setw(32)<<std::dec<<m_adc_counts.size()<<std::setfill(' ')<<std::endl;
  }
};

inline
RawWaveform_p0::RawWaveform_p0() : m_channel(0) { 
  m_adc_counts.clear(); 
}

// Stream operator for debugging
//std::ostream
//&operator<<(std::ostream &out, const RawWaveform_p0 &wfm) {
//  return wfm.print(out);
//}

#endif
