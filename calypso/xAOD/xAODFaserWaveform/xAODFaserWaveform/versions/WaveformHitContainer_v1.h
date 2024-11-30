// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORM_VERSIONS_WAVEFORMHITCONTAINER_V1_H
#define XAODFASERWAVEFORM_VERSIONS_WAVEFORMHITCONTAINER_V1_H

// System include(s):
extern "C" {
#   include "stdint.h"
}

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local includes:
#include "xAODFaserWaveform/versions/WaveformHit_v1.h"

namespace xAOD {
  // Define the container as a simple DataVector
  typedef DataVector<WaveformHit_v1> WaveformHitContainer_v1;
}

#endif // XAODFASERWAVEFORM_VERSIONS_WAVEFORMHITCONTAINER_V1_H
