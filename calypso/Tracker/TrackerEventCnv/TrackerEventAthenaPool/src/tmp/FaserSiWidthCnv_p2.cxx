/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   FaserSiWidthCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#include "TrackerPrepRawData/FaserSiWidth.h"

#include "TrackerEventAthenaPool/FaserSiWidthCnv_p2.h"

void FaserSiWidthCnv_p2::persToTrans( const FaserSiWidth_p2 *persObj,
				 FaserSiWidth *transObj,  MsgStream &log )
{
//    if (log.level() <= MSG::VERBOSE) log << MSG::VERBOSE << "in SiWidthCnv_p2::persToTrans" << endmsg;
  transObj->setColumn (persObj->m_size >> 8);
  transObj->setRow    (persObj->m_size & 0xFF );
  Amg::Vector2D phirzwidth;
  m_localPosCnv.persToTrans(&persObj->m_phirzWidth, &phirzwidth, log);
  transObj->setPhirzWidth (phirzwidth);
}


void FaserSiWidthCnv_p2::transToPers( const FaserSiWidth *transObj,
				 FaserSiWidth_p2 *persObj, MsgStream &log )
{
//    if (log.level() <= MSG::VERBOSE)  log<< MSG::VERBOSE << "in SiWidthCnv_p2::transToPERS" << endmsg;
  persObj->m_size = ( (unsigned int) transObj->colRow()[0] << 8) | ( (unsigned int) transObj->colRow()[1] );
  //persObj->m_size = ( (unsigned int) transObj->colRow()[Trk::locX] << 8) | ( (unsigned int) transObj->colRow()[Trk::locY] );
  m_localPosCnv.transToPers(&transObj->widthPhiRZ(), &persObj->m_phirzWidth, log);
}
