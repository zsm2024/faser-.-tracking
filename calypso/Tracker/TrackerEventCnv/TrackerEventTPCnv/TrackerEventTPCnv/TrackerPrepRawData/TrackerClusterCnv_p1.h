/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERCLUSTER_CNV_P1_H
#define TRACKERCLUSTER_CNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   TrackerClusterCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "TrackerPrepRawData/TrackerCluster.h"
#include "TrackerCluster_p1.h"

#include "FaserSiWidthCnv_p1.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"


class MsgStream;

class TrackerClusterCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Tracker::TrackerCluster, Tracker::TrackerCluster_p1>
{
public:
  TrackerClusterCnv_p1(): m_rawDataCnv(0), m_swCnv(0) {}

  void persToTrans( const Tracker::TrackerCluster_p1 *, Tracker::TrackerCluster *, MsgStream & );
  void transToPers( const Tracker::TrackerCluster *, Tracker::TrackerCluster_p1 *, MsgStream & );
        
protected:
//  ITPConverterFor<Trk::PrepRawData>	*m_rawDataCnv;
//  ITPConverterFor<Tracker::FaserSiWidth> 	*m_swCnv;
    PrepRawDataCnv_p1 *m_rawDataCnv;
    FaserSiWidthCnv_p1  *m_swCnv;
   

};

#endif // TRACKERCLUSTER_CNV_P1_H
