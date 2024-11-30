/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

// Local include(s):
#include "xAODFaserWaveform/versions/WaveformHitAuxContainer_v1.h"

namespace xAOD {

  WaveformHitAuxContainer_v1::WaveformHitAuxContainer_v1() 
    : AuxContainerBase() {

    AUX_VARIABLE(channel);
    AUX_VARIABLE(id32);
    AUX_VARIABLE(localtime);
    AUX_VARIABLE(peak);
    AUX_VARIABLE(width);
    AUX_VARIABLE(integral);
    AUX_VARIABLE(bcid_time);
    AUX_VARIABLE(raw_peak);
    AUX_VARIABLE(raw_integral);

    AUX_VARIABLE(baseline_mean);
    AUX_VARIABLE(baseline_rms);

    AUX_VARIABLE(hit_status);
    AUX_VARIABLE(mean);
    AUX_VARIABLE(alpha);
    AUX_VARIABLE(nval);
    AUX_VARIABLE(time_vector);
    AUX_VARIABLE(wave_vector);

  }

} // namespace xAOD

