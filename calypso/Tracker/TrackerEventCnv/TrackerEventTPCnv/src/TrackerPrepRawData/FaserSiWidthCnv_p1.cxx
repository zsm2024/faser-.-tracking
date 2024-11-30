/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   FaserSiWidthCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrackerPrepRawData/FaserSiWidth.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/FaserSiWidthCnv_p1.h"

void FaserSiWidthCnv_p1::persToTrans( const Tracker::FaserSiWidth_p1 *persObj,
				 Tracker::FaserSiWidth *transObj,  MsgStream &log )
{
//    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "in SiWidthCnv_p1::persToTrans" << endmsg;
  transObj->setColumn (persObj->x);
  transObj->setRow    (persObj->y);
  Amg::Vector2D phirzwidth;
  fillTransFromPStore( &m_localPosCnv, persObj->m_phirzWidth, &phirzwidth, log );
  transObj->setPhirzWidth (phirzwidth);
}


void FaserSiWidthCnv_p1::transToPers( const Tracker::FaserSiWidth *transObj,
				 Tracker::FaserSiWidth_p1 *persObj, MsgStream &log )
{
//     if (log.level() <= MSG::DEBUG) log<< MSG::DEBUG << "in SiWidthCnv_p1::transToPERS" << endmsg;
  persObj->x = (float) transObj->colRow()[Trk::locX];
  persObj->y = (float) transObj->colRow()[Trk::locY];
  persObj->m_phirzWidth = toPersistent( &m_localPosCnv, &transObj->widthPhiRZ(), log );
}
