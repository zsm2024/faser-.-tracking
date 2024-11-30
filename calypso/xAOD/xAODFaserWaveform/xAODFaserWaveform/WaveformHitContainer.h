// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORM_WAVEFORMHITCONTAINER_H
#define XAODFASERWAVEFORM_WAVEFORMHITCONTAINER_H

// Local include(s):
#include "xAODFaserWaveform/versions/WaveformHitContainer_v1.h"

namespace xAOD {
  /// Declare the latest version of the class
  typedef WaveformHitContainer_v1 WaveformHitContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::WaveformHitContainer, 1156844391, 1 )

#endif // XAODFASERWAVEFORM_WAVEFORMHITCONTAINER_H
