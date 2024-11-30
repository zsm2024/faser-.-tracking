// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORM_WAVEFORMCLOCKAUXINFO_H
#define XAODFASERWAVEFORM_WAVEFORMCLOCKAUXINFO_H

// Local include(s):
#include "xAODFaserWaveform/versions/WaveformClockAuxInfo_v1.h"

namespace xAOD {
  /// Declare the latest version of the class
  typedef WaveformClockAuxInfo_v1 WaveformClockAuxInfo;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::WaveformClockAuxInfo, 150155999, 1 )

#endif // XAODFASERWAVEFORM_WAVEFORMCLOCKAUXINFO_H
