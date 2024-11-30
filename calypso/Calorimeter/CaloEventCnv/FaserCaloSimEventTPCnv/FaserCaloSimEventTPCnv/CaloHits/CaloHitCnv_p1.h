/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOHITCNV_P1_H
#define CALOHITCNV_P1_H

/*
Transient/Persistent converter for CaloHit class
Author: Davide Costanzo
*/

#include "FaserCaloSimEvent/CaloHit.h"
#include "CaloHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class CaloHitCnv_p1  : public T_AthenaPoolTPCnvBase<CaloHit, CaloHit_p1>
{
public:

  CaloHitCnv_p1() {}

  virtual void          persToTrans(const CaloHit_p1* persObj, CaloHit*
transObj, MsgStream &log);
  virtual void          transToPers(const CaloHit* transObj, CaloHit_p1*
persObj, MsgStream &log);
};


#endif
