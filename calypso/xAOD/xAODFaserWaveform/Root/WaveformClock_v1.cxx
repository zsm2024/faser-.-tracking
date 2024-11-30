/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODFaserWaveform/versions/WaveformClock_v1.h"

namespace xAOD {

  WaveformClock_v1::WaveformClock_v1() : SG::AuxElement() {
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformClock_v1, double, frequency, set_frequency )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformClock_v1, double, phase, set_phase )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformClock_v1, double, dc_offset, set_dc_offset )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformClock_v1, double, amplitude, set_amplitude )

  float WaveformClock_v1::time_from_clock(float time) const {
    // Figure out which integer cycle we are on, must add 1/4 of a cycle
    // because FFT finds phase of cosine, which would be middle of 
    // clock HI region.  frequency is in MHz, so convert here to GHz
    int ncycle = int(time*frequency() / 1.E3 + phase() / (2*M_PI) + 0.25);
    float dt = time - (ncycle - phase() / (2*M_PI) - 0.25) * 1.E3 / frequency();
    return dt;
  }

} // namespace xAOD

namespace xAOD {

  std::ostream& operator<<(std::ostream& s, const xAOD::WaveformClock_v1& clk) {
    s << "xAODWaveformClock: frequency=" << clk.frequency()
      << " phase=" << clk.phase()
      << " amplitude=" << clk.amplitude()
      << " offset=" << clk.dc_offset()
      << std::endl;

    return s;
  }

} // namespace xAOD
