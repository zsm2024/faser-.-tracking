// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERCALORIMETERATHENAPOOL_XAODCALORIMETERHITCONTAINERCNV_H
#define XAODFASERCALORIMETERATHENAPOOL_XAODCALORIMETERHITCONTAINERCNV_H

#include "xAODFaserCalorimeter/CalorimeterHitContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"

typedef T_AthenaPoolxAODCnv<xAOD::CalorimeterHitContainer> xAODCalorimeterHitContainerCnv;

#endif // XAODFASERCALORIMETERATHENAPOOL_XAODCALORIMETERHITCONTAINERCNV_H
