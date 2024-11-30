/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_CLUSTER_ON_TRACK_CNV_p2_H
#define FASERSCT_CLUSTER_ON_TRACK_CNV_p2_H

#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerEventTPCnv/TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack_p2.h"

#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "AthLinks/ElementLink.h" 
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"

#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrkEventCnvTools/IEventCnvSuperTool.h"

class MsgStream;

class FaserSCT_ClusterOnTrackCnv_p2 : public T_AthenaPoolTPPolyCnvBase< Trk::MeasurementBase, Tracker::FaserSCT_ClusterOnTrack, Tracker::FaserSCT_ClusterOnTrack_p2 >{
public:
 FaserSCT_ClusterOnTrackCnv_p2(): m_eventCnvTool("Trk::EventCnvSuperTool/EventCnvSuperTool"), 
                             m_localParCnv(0), m_errorMxCnv(0) {}

    void persToTrans( const Tracker::FaserSCT_ClusterOnTrack_p2 *persObj, Tracker::FaserSCT_ClusterOnTrack *transObj, MsgStream &log );
    void transToPers( const Tracker::FaserSCT_ClusterOnTrack *transObj, Tracker::FaserSCT_ClusterOnTrack_p2 *persObj, MsgStream &log );

protected:
    ToolHandle<Trk::IEventCnvSuperTool>                       m_eventCnvTool;
    ElementLinkCnv_p1< ElementLinkToTrackerCFaserSCT_ClusterContainer > m_elCnv;     
    LocalParametersCnv_p1*                                    m_localParCnv;
    ErrorMatrixCnv_p1*                                        m_errorMxCnv;
};

#endif 
