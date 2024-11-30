// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORM_WAVEFORMCLOCK_H
#define XAODFASERWAVEFORM_WAVEFORMCLOCK_H

// Local include(s):
#include "xAODFaserWaveform/versions/WaveformClock_v1.h"

namespace xAOD {
  /// Declare the latest version of the class
  typedef WaveformClock_v1 WaveformClock;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::WaveformClock, 58376762, 1 )

#endif // XAODFASERWAVEFORM_WAVEFORMCLOCK_H
