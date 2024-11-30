// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERCALORIMETERATHENAPOOL_XAODCALORIMETERHITAUXCONTAINERCNV_H
#define XAODFASERCALORIMETERATHENAPOOL_XAODCALORIMETERHITAUXCONTAINERCNV_H

#include "xAODFaserCalorimeter/CalorimeterHitAuxContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::CalorimeterHitAuxContainer> xAODCalorimeterHitAuxContainerCnv;

#endif // XAODFASERCALORIMETERATHENAPOOL_XAODFASERCALORIMETERHITAUXCONTAINERCNV_H
