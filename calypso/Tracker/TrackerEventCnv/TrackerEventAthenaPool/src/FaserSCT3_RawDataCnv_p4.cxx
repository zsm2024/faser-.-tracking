/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT3_RawDataCnv_p4.h"

#include "MsgUtil.h"

#include "TrackerEventAthenaPool/FaserSCT3_RawData_p4.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerRawData/FaserSCT3_RawData.h"

#include <vector>

void
FaserSCT3_RawDataCnv_p4::persToTrans(const FaserSCT3_RawData_p4* persObj, FaserSCT3_RawData* transObj, MsgStream& /*log*/)
{
  // MSG_DEBUG(log, "FaserSCT3_RawDataCnv_p4::persToTrans called");

  // Convert unsigned short to unsigned int
  // P: Bits 0-10 for group size, Bits 11-13 for time bin
  // T: Bits 0-10 for group size, Bits 22-24 for time bin
  unsigned int word = static_cast<unsigned int>(persObj->m_word);
  word = ((word & 0x7FF) | (((word >> 11) & 0x7) << 22));

  // Convert row and strip numbers to strip Identifeir using wafer Identifer
  Identifier stripId = m_sctId->strip_id(m_waferId,
                                         persObj->getRow(),
                                         persObj->getStrip());

  *transObj = FaserSCT3_RawData(stripId, word, std::vector<int>());
}

void
FaserSCT3_RawDataCnv_p4::transToPers(const FaserSCT3_RawData* transObj, FaserSCT3_RawData_p4* persObj, MsgStream& /*log*/)
{
  // MSG_DEBUG(log, "FaserSCT3_RawDataCnv_p4::transToPers called");

  // Set row and strip numbers
  persObj->setRowStrip(m_sctId->row(transObj->identify()), m_sctId->strip(transObj->identify()));

  // Convert unsigned int to unsigned short
  // T: Bits 0-10 for group size, Bits 22-24 for time bin
  // P: Bits 0-10 for group size, Bits 11-13 for time bin
  unsigned int word = transObj->getWord();
  word = ((word & 0x7FF) | (((word >> 22) & 0x7) << 11));
  persObj->m_word = static_cast<unsigned short>(word);
}
