/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSIHITCNV_P2_H
#define FASERSIHITCNV_P2_H

/*
Transient/Persistent converter for FaserSiHit class
*/

#include "TrackerSimEvent/FaserSiHit.h"
#include "FaserSiHit_p2.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class FaserSiHitCnv_p2  : public T_AthenaPoolTPCnvBase<FaserSiHit, FaserSiHit_p2>
{
public:

  FaserSiHitCnv_p2() {}

  virtual void          persToTrans(const FaserSiHit_p2* persObj, FaserSiHit*
transObj, MsgStream &log);
  virtual void          transToPers(const FaserSiHit* transObj, FaserSiHit_p2*
persObj, MsgStream &log);
};


#endif
