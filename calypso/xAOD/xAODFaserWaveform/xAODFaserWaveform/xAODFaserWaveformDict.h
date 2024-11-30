/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORM_XAODFASERWAVEFORMDICT_H
#define XAODFASERWAVEFORM_XAODFASERWAVEFORMDICT_H

// Local includes
#include "xAODFaserWaveform/WaveformHit.h"
#include "xAODFaserWaveform/WaveformHitContainer.h"
#include "xAODFaserWaveform/WaveformHitAuxContainer.h"

#include "xAODFaserWaveform/WaveformClock.h"
#include "xAODFaserWaveform/WaveformClockAuxInfo.h"

#include "xAODFaserWaveform/versions/WaveformHit_v1.h"
#include "xAODFaserWaveform/versions/WaveformHitContainer_v1.h"
#include "xAODFaserWaveform/versions/WaveformHitAuxContainer_v1.h"

#include "xAODFaserWaveform/versions/WaveformClock_v1.h"
#include "xAODFaserWaveform/versions/WaveformClockAuxInfo_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

namespace {
  struct GCCXML_DUMMY_INSTANTIATION_XAODFASERWAVEFORM {
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, WaveformHitContainer_v1 );
    XAOD_INSTANTIATE_NS_OBJECT_TYPES( xAOD, WaveformClock_v1 );
  };
}

#endif // XAODFASERWAVEFORM_XAODFASERWAVEFORMDICT_H
