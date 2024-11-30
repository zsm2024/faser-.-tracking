/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

// Local include(s):
#include "xAODFaserWaveform/versions/WaveformClockAuxInfo_v1.h"

namespace xAOD {

  WaveformClockAuxInfo_v1::WaveformClockAuxInfo_v1()
    : AuxInfoBase(), 
      frequency(0), phase(0), dc_offset(0), amplitude(0) {
    AUX_VARIABLE( frequency );
    AUX_VARIABLE( phase );
    AUX_VARIABLE( dc_offset );
    AUX_VARIABLE( amplitude );
  }
} // namespace xAOD
