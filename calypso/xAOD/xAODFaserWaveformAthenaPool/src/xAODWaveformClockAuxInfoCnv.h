// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORMATHENAPOOL_XAODWAVEFORMCLOCKAUXINFOCNV_H
#define XAODFASERWAVEFORMATHENAPOOL_XAODWAVEFORMCLOCKAUXINFOCNV_H

#include "xAODFaserWaveform/WaveformClockAuxInfo.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::WaveformClockAuxInfo> xAODWaveformClockAuxInfoCnv;

#endif // XAODFASERWAVEFORMATHENAPOOL_XAODFASERWAVEFORMCLOCKAUXINFOCNV_H
