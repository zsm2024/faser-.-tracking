/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_CLUSTER_CNV_P3_H
#define FASERSCT_CLUSTER_CNV_P3_H

//-----------------------------------------------------------------------------
//
// file:   FaserSCT_ClusterCnv_p3.h
//
//-----------------------------------------------------------------------------

#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "FaserSCT_Cluster_p3.h"

#include "FaserSiWidthCnv_p2.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrackerIdentifier/FaserSCT_ID.h"

class MsgStream;

class FaserSCT_ClusterCnv_p3
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Tracker::FaserSCT_Cluster, Tracker::FaserSCT_Cluster_p3>
{
public:
  FaserSCT_ClusterCnv_p3() {};
  FaserSCT_ClusterCnv_p3( const FaserSCT_ID * );

  Tracker::FaserSCT_Cluster
  createSCT_Cluster (const Tracker::FaserSCT_Cluster_p3* persObj,
                     Identifier clusId,
                     const TrackerDD::SiDetectorElement* detEl,
                     MsgStream& log);

  void persToTrans( const Tracker::FaserSCT_Cluster_p3 *, Tracker::FaserSCT_Cluster *, MsgStream & );
  void transToPers( const Tracker::FaserSCT_Cluster *, Tracker::FaserSCT_Cluster_p3 *, MsgStream & );
        
protected:
  const FaserSCT_ID*           m_sctId2;
  Tracker::FaserSiWidthCnv_p2  m_swCnv;
  LocalPositionCnv_p1	       m_localPosCnv;
  ErrorMatrixCnv_p1   	       m_errorMxCnv;
};

#endif // FASERSCT_CLUSTER_CNV_P3_H
