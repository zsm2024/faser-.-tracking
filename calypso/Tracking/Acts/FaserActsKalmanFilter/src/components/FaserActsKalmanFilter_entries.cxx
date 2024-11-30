/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "../FaserActsKalmanFilterAlg.h"
//#todo#include "../CombinatorialKalmanFilterAlg.h"
//#include "../MultiTrackFinderTool.h"
//#include "../TruthBasedInitialParameterTool.h"
//#include "../TruthTrackFinderTool.h"
//#include "../SPSeedBasedInitialParameterTool.h"
//#include "../SPSimpleInitialParameterTool.h"
//#include "../TrajectoryWriterTool.h"
//#include "../SimWriterTool.h"
//#include "../TruthSeededTrackFinderTool.h"
//#include "../ProtoTrackWriterTool.h"
#include "../RootTrajectoryStatesWriterTool.h"
#include "../RootTrajectorySummaryWriterTool.h"
//#include "../SegmentFitClusterTrackFinderTool.h"
//#include "../SegmentFitTrackFinderTool.h"
//#include "../ClusterTrackSeedTool.h"
#include "../ThreeStationTrackSeedTool.h"
//todo#include "../PerformanceWriterTool.h"
#include "../TrackSeedWriterTool.h"
#include "../ActsTrackSeedTool.h"
#include "../CKF2.h"
// #include "../CKF2Alignment.h"
#include "../KalmanFitterTool.h"
#include "../MyTrackSeedTool.h"
#include "../SeedingAlg.h"
#include "../CircleFitTrackSeedTool.h"
#include "../GhostBusters.h"
#include "../CreateTrkTrackTool.h"
#include "../TrackTruthMatchingTool.h"
#include "../FiducialParticleTool.h"
#include "../RootSeedWriteTool.h"

DECLARE_COMPONENT(FaserActsKalmanFilterAlg)
//#todoDECLARE_COMPONENT(CombinatorialKalmanFilterAlg)
//DECLARE_COMPONENT(TruthBasedInitialParameterTool)
//DECLARE_COMPONENT(SPSeedBasedInitialParameterTool)
//DECLARE_COMPONENT(SPSimpleInitialParameterTool)
//DECLARE_COMPONENT(TrajectoryWriterTool)
//DECLARE_COMPONENT(TruthTrackFinderTool)
//DECLARE_COMPONENT(SimWriterTool)
//DECLARE_COMPONENT(TruthSeededTrackFinderTool)
//DECLARE_COMPONENT(ProtoTrackWriterTool)
//DECLARE_COMPONENT(SegmentFitClusterTrackFinderTool)
//DECLARE_COMPONENT(SegmentFitTrackFinderTool)
DECLARE_COMPONENT(RootSeedWriterTool)
DECLARE_COMPONENT(RootTrajectoryStatesWriterTool)
DECLARE_COMPONENT(RootTrajectorySummaryWriterTool)
//DECLARE_COMPONENT(MultiTrackFinderTool)
//DECLARE_COMPONENT(ClusterTrackSeedTool)
DECLARE_COMPONENT(ThreeStationTrackSeedTool)
//#todoDECLARE_COMPONENT(PerformanceWriterTool)
DECLARE_COMPONENT(TrackSeedWriterTool)
DECLARE_COMPONENT(ActsTrackSeedTool)
DECLARE_COMPONENT(CKF2)
// DECLARE_COMPONENT(CKF2Alignment)
DECLARE_COMPONENT(KalmanFitterTool)
DECLARE_COMPONENT(MyTrackSeedTool)
DECLARE_COMPONENT(SeedingAlg)
DECLARE_COMPONENT(CircleFitTrackSeedTool)
DECLARE_COMPONENT(GhostBusters)
DECLARE_COMPONENT(CreateTrkTrackTool)
DECLARE_COMPONENT(TrackTruthMatchingTool)
DECLARE_COMPONENT(FiducialParticleTool)
