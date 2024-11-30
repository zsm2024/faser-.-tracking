/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_CLUSTER_CNV_P1_H
#define FASERSCT_CLUSTER_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   FaserSCT_ClusterCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "FaserSCT_Cluster_p1.h"

#include "TrackerClusterCnv_p1.h"


class MsgStream;

class FaserSCT_ClusterCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Tracker::FaserSCT_Cluster, Tracker::FaserSCT_Cluster_p1>
{
public:
  FaserSCT_ClusterCnv_p1() : m_siClusterCnv(0) {}

  void persToTrans( const Tracker::FaserSCT_Cluster_p1 *persObj, Tracker::FaserSCT_Cluster *transObj, MsgStream &log );
  void transToPers( const Tracker::FaserSCT_Cluster *transObj, Tracker::FaserSCT_Cluster_p1 *persObj,MsgStream &log );

private:
  TrackerClusterCnv_p1	*m_siClusterCnv;
};


#endif // FASERSCT_CLUSTER_CNV_P1_H
