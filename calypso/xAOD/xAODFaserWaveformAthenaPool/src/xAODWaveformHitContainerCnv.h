// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORMATHENAPOOL_XAODWAVEFORMHITCONTAINERCNV_H
#define XAODFASERWAVEFORMATHENAPOOL_XAODWAVEFORMHITCONTAINERCNV_H

#include "xAODFaserWaveform/WaveformHitContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"

typedef T_AthenaPoolxAODCnv<xAOD::WaveformHitContainer> xAODWaveformHitContainerCnv;

#endif // XAODFASERWAVEFORMATHENAPOOL_XAODWAVEFORMHITCONTAINERCNV_H
