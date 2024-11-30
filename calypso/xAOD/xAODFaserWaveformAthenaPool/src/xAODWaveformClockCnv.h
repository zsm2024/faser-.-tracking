// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORMATHENAPOOL_XAODWAVEFORMCLOCKCNV_H
#define XAODFASERWAVEFORMATHENAPOOL_XAODWAVEFORMCLOCKCNV_H

#include "xAODFaserWaveform/WaveformClock.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"

typedef T_AthenaPoolxAODCnv<xAOD::WaveformClock> xAODWaveformClockCnv;

#endif // XAODFASERWAVEFORMATHENAPOOL_XAODWAVEFORMCLOCKCNV_H
