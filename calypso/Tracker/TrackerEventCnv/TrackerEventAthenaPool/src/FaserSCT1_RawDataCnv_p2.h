/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT1_RAWDATACNV_P2_H
#define FASERSCT1_RAWDATACNV_P2_H

/*
Transient/Persistent converter for SCT1_RawData class
Author: Davide Costanzo
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrackerRawData/FaserSCT1_RawData.h"
#include "TrackerEventAthenaPool/TrackerRawData_p2.h"

class MsgStream;


class FaserSCT1_RawDataCnv_p2  : public T_AthenaPoolTPCnvBase<FaserSCT1_RawData, TrackerRawData_p2>
{
public:

  FaserSCT1_RawDataCnv_p2() {}
  virtual void          persToTrans(const TrackerRawData_p2* persObj, FaserSCT1_RawData* transObj, MsgStream& log);
  virtual void          transToPers(const FaserSCT1_RawData* transObj, TrackerRawData_p2* persObj, MsgStream& log);
};


#endif

