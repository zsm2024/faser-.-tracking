/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackerRawData/FaserSCT1_RawData.h"
#include "MsgUtil.h"

// Persistent class and converter header file
#include "TrackerEventAthenaPool/TrackerRawData_p2.h"
#include "FaserSCT1_RawDataCnv_p2.h"

// Other stuff
#include "Identifier/Identifier.h"



void
FaserSCT1_RawDataCnv_p2::persToTrans(const TrackerRawData_p2* persObj, FaserSCT1_RawData* transObj, MsgStream& /*log*/)
{
  // MSG_VERBOSE(log,"FaserSCT1_RawDataCnv_p2::persToTrans called ");
  *transObj = FaserSCT1_RawData(Identifier(persObj->m_rdoId),
                                persObj->m_word);
}

void
FaserSCT1_RawDataCnv_p2::transToPers(const FaserSCT1_RawData* transObj, TrackerRawData_p2* persObj, MsgStream& /*log*/)
{
  // MSG_VERBOSE(log,"FaserSCT1_RawDataCnv_p2::transToPers called ");
  persObj->m_rdoId = transObj->identify().get_compact();
  persObj->m_word = transObj->getWord();
}
