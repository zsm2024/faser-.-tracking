// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORMATHENAPOOL_XAODWAVEFORMHITAUXCONTAINERCNV_H
#define XAODFASERWAVEFORMATHENAPOOL_XAODWAVEFORMHITAUXCONTAINERCNV_H

#include "xAODFaserWaveform/WaveformHitAuxContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::WaveformHitAuxContainer> xAODWaveformHitAuxContainerCnv;

#endif // XAODFASERWAVEFORMATHENAPOOL_XAODFASERWAVEFORMHITAUXCONTAINERCNV_H
