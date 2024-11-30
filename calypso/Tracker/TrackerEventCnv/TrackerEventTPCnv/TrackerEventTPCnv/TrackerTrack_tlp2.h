/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKER_TRACK_tlp2_H
#define TRACKER_TRACK_tlp2_H



//-----------------------------------------------------------------------------
// TrackerRIO_OnTrack
//-----------------------------------------------------------------------------
// #include "TrackerEventTPCnv/InDetRIO_OnTrack/TRT_DriftCircleOnTrack_p2.h"
// #include "TrackerEventTPCnv/InDetRIO_OnTrack/PixelClusterOnTrack_p2.h"
#include "TrackerEventTPCnv/TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack_p2.h"


// Tracker cROTS

// #include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack_p1.h"
// #include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrack_p1.h"
// #include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrack_p1.h"

namespace Tracker
{
   class Track_tlp2
   {
  public:
     Track_tlp2() {}

    //  std::vector< InDet::PixelClusterOnTrack_p2 >               m_pixelClustersOT;
     std::vector< Tracker::FaserSCT_ClusterOnTrack_p2 >            m_sctClustersOT;
    //  std::vector< InDet::TRT_DriftCircleOnTrack_p2 >            m_trtDriftCirclesOT;
    //  std::vector< InDet::CompetingPixelClustersOnTrack_p1>      m_pixelCROTs;
    //  std::vector< InDet::CompetingSCT_ClustersOnTrack_p1>       m_sctCROTs;
    //  std::vector< InDet::CompetingTRT_DriftCirclesOnTrack_p1>   m_trtCROTs;
   };
}

#endif