/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   FaserSCT_ClusterCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/FaserSCT_ClusterCnv_p1.h"

void FaserSCT_ClusterCnv_p1::
persToTrans( const Tracker::FaserSCT_Cluster_p1 *persObj, Tracker::FaserSCT_Cluster *transObj,MsgStream &log ) 
{
//    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "SCT_ClusterCnv_p1::persToTrans" << endmsg;
   fillTransFromPStore( &m_siClusterCnv, persObj->m_siCluster, transObj, log );
}

void FaserSCT_ClusterCnv_p1::
transToPers( const Tracker::FaserSCT_Cluster *transObj, Tracker::FaserSCT_Cluster_p1 *persObj, MsgStream &log )
{
//    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "SCT_ClusterCnv_p1::transToPers" << endmsg;
   persObj->m_siCluster = baseToPersistent( &m_siClusterCnv, transObj, log );
}


