// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORM_VERSIONS_WAVEFORMCLOCK_V1_H
#define XAODFASERWAVEFORM_VERSIONS_WAVEFORMCLOCK_V1_H

// System include(s):
extern "C" {
#   include "stdint.h"
}

// Core EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

  // Cllss describing pulses in the waveform digitizer
  class WaveformClock_v1 : public SG::AuxElement {

  public:
    /// Defaullt constructor
    WaveformClock_v1();

    /// @name Access WaveformClock elements
    /// @{

    /// Clock Frequency (in MHz)
    double frequency() const;
    void set_frequency(double value);

    /// Clock Phase (in Radians)
    double phase() const;
    void set_phase(double value);

    /// DC Clock offset
    double dc_offset() const;
    void set_dc_offset(double value);

    /// Amplitude of primary freq. component
    double amplitude() const;
    void set_amplitude(double value);

    /// Distance of time (in ns) from previous rising clock edge
    float time_from_clock(float time) const;

    /// @}

  }; // class WaveformClock_v1

  std::ostream& operator<<(std::ostream& s, const xAOD::WaveformClock_v1& clk);

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::WaveformClock_v1, SG::AuxElement );

#endif // XAODFASERWAVEFORM_VERSIONS_WAVEFORMCLOCK_V1_H
