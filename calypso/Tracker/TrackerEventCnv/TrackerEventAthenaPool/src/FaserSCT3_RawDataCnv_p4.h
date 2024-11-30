/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT3_RAWDATACNV_P4_H
#define FASERSCT3_RAWDATACNV_P4_H

/*
  Transient/Persistent converter for FaserSCT3_RawData class
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "Identifier/Identifier.h"
#include "TrackerRawData/FaserSCT3_RawData.h"
#include "TrackerEventAthenaPool/FaserSCT3_RawData_p4.h"

class MsgStream;
class FaserSCT_ID;

class FaserSCT3_RawDataCnv_p4 : public T_AthenaPoolTPCnvBase<FaserSCT3_RawData, FaserSCT3_RawData_p4>
{
 public:
  FaserSCT3_RawDataCnv_p4(const FaserSCT_ID* sctId) { m_sctId = sctId; }
  virtual void persToTrans(const FaserSCT3_RawData_p4* persObj, FaserSCT3_RawData* transObj, MsgStream& log);
  virtual void transToPers(const FaserSCT3_RawData* transObj, FaserSCT3_RawData_p4* persObj, MsgStream& log);
  void setWaferId(const Identifier waferId) { m_waferId = waferId; }
 private:
  const FaserSCT_ID* m_sctId;
  Identifier m_waferId;
};

#endif // FASERSCT3_RAWDATACNV_P4_H
