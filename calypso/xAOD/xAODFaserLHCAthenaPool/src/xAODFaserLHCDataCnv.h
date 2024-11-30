// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERLHCDATAATHENAPOOL_XAODFASERLHCDATACNV_H
#define XAODFASERLHCDATAATHENAPOOL_XAODFASERLHCDATACNV_H

#include "xAODFaserLHC/FaserLHCData.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"


typedef T_AthenaPoolxAODCnv<xAOD::FaserLHCData> xAODFaserLHCDataCnv;


#endif // XAODFASERLHCDATAATHENAPOOL_XAODFASERLHCDATACNV_H
