// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERTRIGGERDATAATHENAPOOL_XAODFASERTRIGGERDATACNV_H
#define XAODFASERTRIGGERDATAATHENAPOOL_XAODFASERTRIGGERDATACNV_H

#include "xAODFaserTrigger/FaserTriggerData.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"


typedef T_AthenaPoolxAODCnv<xAOD::FaserTriggerData> xAODFaserTriggerDataCnv;


#endif // XAODFASERTRIGGERDATAATHENAPOOL_XAODFASERTRIGGERDATACNV_H
