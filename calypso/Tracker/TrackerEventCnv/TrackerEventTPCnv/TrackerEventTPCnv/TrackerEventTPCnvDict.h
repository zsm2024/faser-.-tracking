/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKEREVENTTPCNV_TRACKEREVENTTPCNVDICT_H
#define TRACKEREVENTTPCNV_TRACKEREVENTTPCNVDICT_H

//-----------------------------------------------------------------------------
//
// file:   TrackerEventTPCnvDict_p1.h
//
//-----------------------------------------------------------------------------

// #include "InDetEventTPCnv/InDetTrack_tlp1.h"
#include "TrackerEventTPCnv/TrackerTrack_tlp2.h"
// #include "InDetEventTPCnv/PixelClusterContainer_tlp1.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/TrackerCluster_p1.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/TrackerPRD_Collection_p1.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/TrackerPRD_Container_p1.h"
// #include "InDetEventTPCnv/InDetPrepRawData/PixelCluster_p1.h"
// #include "InDetEventTPCnv/InDetPrepRawData/PixelCluster_p2.h"
// #include "InDetEventTPCnv/InDetPrepRawData/PixelCluster_p3.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/FaserSCT_Cluster_p1.h"
// #include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircle_p1.h"
// #include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircle_p2.h"
// #include "InDetEventTPCnv/TRT_DriftCircleContainer_p2.h"
// #include "InDetEventTPCnv/PixelClusterContainer_p2.h"
// #include "InDetEventTPCnv/PixelClusterContainer_p3.h"
// #include "InDetEventTPCnv/SCT_ClusterContainer_p2.h"
#include "TrackerEventTPCnv/FaserSCT_ClusterContainer_p3.h"
// #include "InDetEventTPCnv/PixelClusterContainer_tlp1.h"
#include "TrackerEventTPCnv/FaserSCT_ClusterContainer_tlp1.h"
// #include "InDetEventTPCnv/TRT_DriftCircleContainer_tlp1.h"
// #include "InDetEventTPCnv/InDetPrepRawData/PixelGangedClusterAmbiguities_p1.h"
// #include "InDetEventTPCnv/TRT_DriftCircleContainer_tlp1.h"
#include "TrackerEventTPCnv/TrackerRIO_OnTrack/SiClusterOnTrack_p1.h"
#include "TrackerEventTPCnv/TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack_p2.h"
// #include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack_p1.h"
// #include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrack_p1.h"
// #include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrack_p1.h"
// #include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidate_p1.h"
// #include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaContainer_p1.h"
// #include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidate_tlp1.h"
// #include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaContainer_tlp1.h"
#include "TrackerEventTPCnv/FaserSCT_SpacePoint_p0.h"
#include "TrackerEventTPCnv/FaserSCT_SpacePointCollection_p0.h"
#include "TrackerEventTPCnv/FaserSCT_SpacePointContainer_p0.h"

namespace TrackerEventTPCnvDict {
 struct tmp
{
    // std::vector< InDet::PixelCluster_p1>   		m_v1;
    std::vector< Tracker::FaserSCT_Cluster_p1>   		m_v2;
    std::vector< Tracker::FaserSCT_Cluster_p3>   		m_v2a;
    std::vector< Tracker::TrackerCluster_p1>   		m_v3;
    // std::vector< InDet::TRT_DriftCircle_p1 >   		m_v6;
    // std::vector< InDet::TRT_DriftCircle_p2 >   		m_v6a;
    std::vector< Tracker::TrackerPRD_Collection_p1 >	m_v7;
    std::vector< Tracker::TrackerPRD_Container_p1 >		m_v8;
    std::vector< Tracker::SiClusterOnTrack_p1 > m_v9;
    std::vector< Tracker::FaserSCT_ClusterOnTrack_p2 > m_10;
    // std::vector< InDet::CompetingPixelClustersOnTrack_p1> m_v9;
    // std::vector< InDet::CompetingSCT_ClustersOnTrack_p1> m_v10;
    // std::vector< InDet::CompetingTRT_DriftCirclesOnTrack_p1> m_v11;
    // std::vector< InDetLowBetaCandidate_p1> m_v12;
    std::vector<FaserSCT_SpacePoint_p0> m_v13;
    std::vector<FaserSCT_SpacePointCollection_p0> m_v14;
    std::vector<FaserSCT_SpacePointContainer_p0> m_v15;
 };
} //> namespace TrackerEventTPCnvDict

#endif // TRACKEREVENTTPCNV_TRACKEREVENTTPCNVDICT_H
