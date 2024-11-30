// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORM_WAVEFORMHIT_H
#define XAODFASERWAVEFORM_WAVEFORMHIT_H

// Local include(s):
#include "xAODFaserWaveform/versions/WaveformHit_v1.h"

namespace xAOD {
  /// Declare the latest version of the class
  typedef WaveformHit_v1 WaveformHit;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::WaveformHit, 131600577, 1 )

#endif // XAODFASERWAVEFORM_WAVEFORMHIT_H
