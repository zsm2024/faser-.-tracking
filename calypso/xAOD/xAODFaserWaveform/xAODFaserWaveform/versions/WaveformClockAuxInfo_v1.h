// Dear emacs, this is -*- c++ -*- 

/* 
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORM_VERSIONS_WAVEFORMCLOCKAUXINFO_V1_H
#define XAODFASERWAVEFORM_VERSIONS_WAVEFORMCLOCKAUXINFO_V1_H

// System include(s):
extern "C" {
#   include "stdint.h"
}

// xAOD include(s):
#include "xAODCore/AuxInfoBase.h"

namespace xAOD {

  /// Class holding the data handled by WaveformClock_v1

  class WaveformClockAuxInfo_v1 : public AuxInfoBase {

  public:
    /// Default constructor
    WaveformClockAuxInfo_v1();

  private:
    /// @name Basic variables
    ///@ {
    double frequency;
    double phase;
    double dc_offset;
    double amplitude;
    ///@}

  }; // class WaveformClockAuxInfo_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::WaveformClockAuxInfo_v1, xAOD::AuxInfoBase );

#endif // XAODFASERWAVEFORM_VERSIONS_WAVEFORMCLOCKAUXINFO_V1_H
