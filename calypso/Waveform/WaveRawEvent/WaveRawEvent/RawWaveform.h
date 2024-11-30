/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RawWaveform.h
//   Header file for class RawWaveform
///////////////////////////////////////////////////////////////////
// Class for the raw waveform data for all scintillator detectors
// This is a direct conversion of the DigitizerDataFragment data
///////////////////////////////////////////////////////////////////

#ifndef WAVERAWEVENT_RAWWAVEFORM_H
#define WAVERAWEVENT_RAWWAVEFORM_H

#include <bitset>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "AthenaKernel/CLASS_DEF.h"
#include "Identifier/Identifier.h"
#include "Identifier/Identifier32.h"
#include "Identifier/Identifiable.h"

class DigitizerDataFragment;

class RawWaveform : public Identifiable {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Default constructor
  RawWaveform();

  // Destructor:
  virtual ~RawWaveform(); 
  
  //move assignment defaulted
  RawWaveform & operator = (RawWaveform &&) = default;
  //assignment defaulted
  RawWaveform & operator = (const RawWaveform &) = default;
  //copy c'tor defaulted
  RawWaveform(const RawWaveform &) = default;

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // General data from Digitizer
  unsigned int board_id() const;
  bool         board_fail_flag() const;
  unsigned int pattern_trig_options() const;
  unsigned int channel_mask() const;
  unsigned int event_counter() const;
  unsigned int trigger_time_tag() const;
  unsigned int n_samples() const;

  // Waveform data
  unsigned int channel() const;
  const std::vector<unsigned int>& adc_counts() const;
  size_t size() const {return m_adc_counts.size();}

  // Return channel identifier
  Identifier identify() const;
  Identifier32 identify32() const;

  // Full-scale range (in V) of 14-bit ADC reading
  // mV per bit is given by range() / 16.384
  float range() const;
  float mv_per_bit() const {return m_range / 16.384;}

  // some print-out:
  void print() const;

  bool operator < (const RawWaveform& rhs) const
  {return m_ID < rhs.m_ID;}

  // Set functions
  void setIdentifier (Identifier id);  // Identifier class
  void setHeader (const DigitizerDataFragment* frag);
  void setWaveform (unsigned int channel, const std::vector<uint16_t> waveform);

  // Set functions for P->T conversion
  void setBoardId(unsigned int id) {m_board_id = id;}
  void setBoardFailFlag(bool flag) {m_board_fail_flag = flag;}
  void setPatternTrigOptions(unsigned int pattern) {m_pattern_trig_options = pattern;}
  void setChannelMask(unsigned int mask) {m_channel_mask = mask;}
  void setEventCounter(unsigned int counter) {m_event_counter = counter;}
  void setTriggerTime(unsigned int tag) {m_trigger_time_tag = tag;}
  void setChannel(unsigned int chan) {m_channel = chan;}
  void setSamples(unsigned int samp) {m_samples = samp;}
  void setCounts(const std::vector<unsigned int>& counts) {m_adc_counts = counts;}

  void setRange(float range) {m_range = range;}

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  uint32_t m_board_id;
  bool     m_board_fail_flag;
  uint16_t m_pattern_trig_options;
  uint16_t m_channel_mask;
  uint32_t m_event_counter;
  uint32_t m_trigger_time_tag;
      
  unsigned int m_samples;
  unsigned int m_channel;
  std::vector<unsigned int> m_adc_counts;

  Identifier32 m_ID;

  float m_range;  
};


///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline unsigned int
RawWaveform::channel() const { return m_channel; }

inline unsigned int 
RawWaveform::board_id() const { return m_board_id; }

inline bool 
RawWaveform::board_fail_flag() const { return m_board_fail_flag; }

inline unsigned int 
RawWaveform::pattern_trig_options() const { return m_pattern_trig_options; }

inline unsigned int 
RawWaveform::channel_mask() const { return m_channel_mask; }

inline unsigned int 
RawWaveform::event_counter() const { return m_event_counter; }

inline unsigned int 
RawWaveform::trigger_time_tag() const { return m_trigger_time_tag; }

inline unsigned int 
RawWaveform::n_samples() const { return m_samples; }

inline const std::vector<unsigned int>&
RawWaveform::adc_counts() const { return m_adc_counts; }

inline Identifier 
RawWaveform::identify() const { return Identifier(m_ID); }

inline Identifier32
RawWaveform::identify32() const { return m_ID; }

inline float
RawWaveform::range() const { return m_range; }

std::ostream 
&operator<<(std::ostream &out, const RawWaveform &wfm);

CLASS_DEF( RawWaveform, 17563068, 1 )

#endif // WAVERAWEVENT_RAWWAVEFORM_H
