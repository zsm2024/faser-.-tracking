/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKER_TRACKCNV_tlp2_H
#define TRACKER_TRACKCNV_tlp2_H


#include "AthenaPoolCnvSvc/AthenaPoolTopLevelExtTPConverter.h"

#include "TrackerTrack_tlp2.h"

// #include "TrackerEventTPCnv/InDetRIO_OnTrack/PixelClusterOnTrackCnv_p2.h"
#include "TrackerEventTPCnv/TrackerRIO_OnTrack/FaserSCT_ClusterOnTrackCnv_p2.h"
// #include "InDetEventTPCnv/InDetRIO_OnTrack/SiClusterOnTrackCnv_p2.h"
// #include "InDetEventTPCnv/InDetRIO_OnTrack/TRT_DriftCircleOnTrackCnv_p2.h"

// #include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrackCnv_p1.h"
// #include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrackCnv_p1.h"
// #include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrackCnv_p1.h"

#include <iostream>

class TrackerTrackCnv_tlp2
    : public AthenaPoolTopLevelExtTPConverter< Tracker::Track_tlp2 >
{
public:

    TrackerTrackCnv_tlp2();
    virtual ~TrackerTrackCnv_tlp2() {}

    virtual unsigned short    getConverterID() { return 1; }

    virtual void          setPStorage( Tracker::Track_tlp2 *storage );

protected:
    // PixelClusterOnTrackCnv_p2                 m_pixelClustersOTCnv;
    FaserSCT_ClusterOnTrackCnv_p2                  m_sctClustersOTCnv;
    // TRT_DriftCircleOnTrackCnv_p2              m_trtDriftCirclesOTCnv;
    // SiClusterOnTrackCnv_p2                    m_siClustersOTCnv;

    // CompetingPixelClustersOnTrackCnv_p1       m_pixelCrotCnv;
    // CompetingSCT_ClustersOnTrackCnv_p1        m_sctCrotCnv;
    // CompetingTRT_DriftCirclesOnTrackCnv_p1    m_trtCrotCnv;


};


#endif


