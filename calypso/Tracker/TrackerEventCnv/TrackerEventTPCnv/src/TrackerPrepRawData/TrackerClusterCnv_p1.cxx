/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackerClusterCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrackerPrepRawData/TrackerCluster.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/TrackerClusterCnv_p1.h"


void TrackerClusterCnv_p1::
persToTrans( const Tracker::TrackerCluster_p1 *persObj, Tracker::TrackerCluster *transObj, MsgStream &log) 
{
//   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "In SiCluster::persToTrans" << endmsg;
  fillTransFromPStore( &m_rawDataCnv, persObj->m_rawData, transObj, log );
  fillTransFromPStore( &m_swCnv, persObj->m_width, &transObj->m_width, log );
}


void TrackerClusterCnv_p1::transToPers( const Tracker::TrackerCluster *transObj, Tracker::TrackerCluster_p1 *persObj, MsgStream &log )
{
//   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "In SiCluster::transToPers" << endmsg;
   persObj->m_rawData = baseToPersistent( &m_rawDataCnv, transObj, log );
   persObj->m_width = toPersistent( &m_swCnv, &transObj->width(), log );
}


