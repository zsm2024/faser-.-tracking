/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCINTHITCNV_P1_H
#define SCINTHITCNV_P1_H

/*
Transient/Persistent converter for ScintHit class
Author: Davide Costanzo
*/

#include "ScintSimEvent/ScintHit.h"
#include "ScintHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class ScintHitCnv_p1  : public T_AthenaPoolTPCnvBase<ScintHit, ScintHit_p1>
{
public:

  ScintHitCnv_p1() {}

  virtual void          persToTrans(const ScintHit_p1* persObj, ScintHit*
transObj, MsgStream &log);
  virtual void          transToPers(const ScintHit* transObj, ScintHit_p1*
persObj, MsgStream &log);
};


#endif
