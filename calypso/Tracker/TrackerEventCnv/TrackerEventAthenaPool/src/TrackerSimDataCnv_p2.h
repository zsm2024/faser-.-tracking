/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERSIMDATACNV_P2_H
#define TRACKERSIMDATACNV_P2_H

/*
  Transient/Persistent converter for TrackerSimData class
  Author: Davide Costanzo
*/

#include "TrackerSimData/TrackerSimData.h"
#include "TrackerEventAthenaPool/TrackerSimData_p2.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

class MsgStream;
class IProxyDict;

class TrackerSimDataCnv_p2  : public T_AthenaPoolTPCnvBase<TrackerSimData, TrackerSimData_p2>
{
public:

  TrackerSimDataCnv_p2();
  virtual void          persToTrans(const TrackerSimData_p2* persObj, TrackerSimData* transObj, MsgStream &log);
  virtual void          transToPers(const TrackerSimData* transObj, TrackerSimData_p2* persObj, MsgStream &log);
  void setCurrentStore (IProxyDict* store);

private:
  IProxyDict* m_sg;
  HepMcParticleLinkCnv_p2 HepMcPLCnv;
};


#endif

