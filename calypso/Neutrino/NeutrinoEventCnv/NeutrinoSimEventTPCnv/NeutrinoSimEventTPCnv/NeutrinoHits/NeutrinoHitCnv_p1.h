/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NEUTRINOHITCNV_P1_H
#define NEUTRINOHITCNV_P1_H

/*
Transient/Persistent converter for NeutrinoHit class
Author: Davide Costanzo
*/

#include "NeutrinoSimEvent/NeutrinoHit.h"
#include "NeutrinoHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class NeutrinoHitCnv_p1  : public T_AthenaPoolTPCnvBase<NeutrinoHit, NeutrinoHit_p1>
{
public:

  NeutrinoHitCnv_p1() {}

  virtual void          persToTrans(const NeutrinoHit_p1* persObj, NeutrinoHit*
transObj, MsgStream &log);
  virtual void          transToPers(const NeutrinoHit* transObj, NeutrinoHit_p1*
persObj, MsgStream &log);
};


#endif
