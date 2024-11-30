// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORM_WAVEFORMHITAUXCONTAINER_H
#define XAODFASERWAVEFORM_WAVEFORMHITAUXCONTAINER_H

// Local include(s):
#include "xAODFaserWaveform/versions/WaveformHitAuxContainer_v1.h"

namespace xAOD {
  /// Declare the latest version of the class
  typedef WaveformHitAuxContainer_v1 WaveformHitAuxContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::WaveformHitAuxContainer, 1262669778, 1 )

#endif // XAODFASERWAVEFORM_WAVEFORMHITAUXCONTAINER_H
