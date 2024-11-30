/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODFaserWaveform/versions/WaveformHit_v1.h"

namespace xAOD {

  WaveformHit_v1::WaveformHit_v1() : SG::AuxElement() {
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, unsigned int, channel, set_channel )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, unsigned int, id32, set_id32 )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, float, localtime, set_localtime )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, float, peak, set_peak )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, float, width, set_width )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, float, integral, set_integral )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, float, bcid_time, set_bcid_time )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, float, trigger_time, set_trigger_time )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, float, raw_peak, set_raw_peak )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, float, raw_integral, set_raw_integral )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, float, baseline_mean, set_baseline_mean )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, float, baseline_rms, set_baseline_rms )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, unsigned int, hit_status, set_hit_status )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, float, mean, set_mean )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, float, alpha, set_alpha )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, float, nval, set_nval )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, std::vector<float>, time_vector, set_time_vector )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( WaveformHit_v1, std::vector<float>, wave_vector, set_wave_vector )

} // namespace xAOD

namespace xAOD {

  std::ostream& operator<<(std::ostream& s, const xAOD::WaveformHit_v1& hit) {
    s << "xAODWaveformHit: channel=" << hit.channel()
      << " local time=" << hit.localtime()
      << " peak=" << hit.peak()
      << " start time=" << hit.time_vector().front()
      << " end time=" << hit.time_vector().back()
      << std::endl;

    return s;
  }

} // namespace xAOD
