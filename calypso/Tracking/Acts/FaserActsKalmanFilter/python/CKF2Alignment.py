#!/usr/bin/env python

import sys
from AthenaCommon.Logging import log, logging
from AthenaCommon.Constants import INFO
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from TrackerSpacePointFormation.TrackerSpacePointFormationConfig import TrackerSpacePointFinderCfg
from TrackerSegmentFit.TrackerSegmentFitConfig import SegmentFitAlgCfg
from FaserActsKalmanFilter.GhostBustersConfig import GhostBustersCfg
from TruthSeededTrackFinder.TruthSeededTrackFinderConfig import TruthSeededTrackFinderCfg
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg
from FaserActsGeometry.ActsGeometryConfig import ActsTrackingGeometryToolCfg
from FaserActsGeometry.ActsGeometryConfig import ActsExtrapolationToolCfg
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
from AthenaConfiguration.ComponentFactory import CompFactory

THistSvc=CompFactory.getComps("THistSvc")

CKF2Alignment, FaserActsExtrapolationTool, FaserActsTrackingGeometryTool=CompFactory.getComps("CKF2Alignment", "FaserActsExtrapolationTool","FaserActsTrackingGeometryTool")
from FaserActsGeometry.ActsGeometryConfig import ActsTrackingGeometrySvcCfg


def CKF2AlignmentCfg(flags, **kwargs):
    acc = FaserSCT_GeometryCfg(flags)
    acc.merge(MagneticFieldSvcCfg(flags))
    result, actsTrackingGeometryTool = ActsTrackingGeometryToolCfg(flags)
#    acts_tracking_geometry_svc = ActsTrackingGeometrySvcCfg(flags)
    acc.merge(result)
    track_seed_tool = CompFactory.CircleFitTrackSeedTool()
    #remove the IFT in the track finding or not
    track_seed_tool.removeIFT = False
    sigma_loc0 = 1.9e-2
    sigma_loc1 = 9e-1
    sigma_phi = 3.3e-2
    sigma_theta = 2.9e-4
    p = 1000
    sigma_p = 0.1 * p
    sigma_qop = sigma_p / (p * p)
    initial_variance_inflation = [100, 100, 100, 100, 1000]
    track_seed_tool.covLoc0 = initial_variance_inflation[0] * sigma_loc1 * sigma_loc1
    track_seed_tool.covLoc1 = initial_variance_inflation[1] * sigma_loc0 * sigma_loc0
    track_seed_tool.covPhi = initial_variance_inflation[2] * sigma_phi * sigma_phi
    track_seed_tool.covTheta = initial_variance_inflation[3] * sigma_theta * sigma_theta
    track_seed_tool.covQOverP = initial_variance_inflation[4] * sigma_qop * sigma_qop
    track_seed_tool.std_cluster = 0.0231
    track_seed_tool.TrackCollection = "Segments"
    # useless in the alignment, will clean them up
    #seed_writer_tool = CompFactory.RootSeedWriterTool()
    #seed_writer_tool.noDiagnostics = kwargs.pop("noDiagnostics", True)
    
    trajectory_states_writer_tool = CompFactory.RootTrajectoryStatesWriterTool()
    trajectory_states_writer_tool.noDiagnostics = kwargs.pop("noDiagnostics", True)
    trajectory_states_writer_tool1 = CompFactory.RootTrajectoryStatesWriterTool()
    trajectory_states_writer_tool1.noDiagnostics = kwargs.pop("noDiagnostics", True)
    trajectory_states_writer_tool1.FilePath = "/pool/track_states_ckf1_10root"
    
    trajectory_summary_writer_tool = CompFactory.RootTrajectorySummaryWriterTool()
    trajectory_summary_writer_tool.noDiagnostics = kwargs.pop("noDiagnostics", True)
    trajectory_summary_writer_tool1 = CompFactory.RootTrajectorySummaryWriterTool()
    trajectory_summary_writer_tool1.FilePath = "/pool/track_summary_ckf1_0.root"
    trajectory_summary_writer_tool1.noDiagnostics = kwargs.pop("noDiagnostics", True)
    actsExtrapolationTool = CompFactory.FaserActsExtrapolationTool("FaserActsExtrapolationTool")
    actsExtrapolationTool.MaxSteps = 1000
    actsExtrapolationTool.TrackingGeometryTool = actsTrackingGeometryTool
    acc.merge(result)
    
    trajectory_performance_writer_tool = CompFactory.PerformanceWriterTool("PerformanceWriterTool")
    trajectory_performance_writer_tool.ExtrapolationTool = actsExtrapolationTool
    trajectory_performance_writer_tool.noDiagnostics = kwargs.pop("noDiagnostics", True)
    ckf2fit = CompFactory.CKF2Alignment(**kwargs)
    #biased residual or unbiased
    ckf2fit.BiasedResidual = False
    ckf2fit.ExtrapolationTool = actsExtrapolationTool
    ckf2fit.TrackingGeometryTool=actsTrackingGeometryTool
    kalman_fitter1 = CompFactory.KalmanFitterTool(name="fitterTool1")
    kalman_fitter1.noDiagnostics = True
    kalman_fitter1.ActsLogging = "INFO"
    kalman_fitter1.SummaryWriter = False
    kalman_fitter1.StatesWriter = False
    kalman_fitter1.SeedCovarianceScale = 10
    kalman_fitter1.isMC = False
    kalman_fitter1.RootTrajectoryStatesWriterTool = trajectory_states_writer_tool1
    kalman_fitter1.RootTrajectorySummaryWriterTool = trajectory_summary_writer_tool1
    ckf2fit.KalmanFitterTool1 = kalman_fitter1
    ckf2fit.TrackSeed = track_seed_tool
    ckf2fit.ActsLogging = "INFO"
    ckf2fit.isMC = False
    ckf2fit.nMax = 10
    ckf2fit.chi2Max = 100000
    ckf2fit.maxSteps = 5000
    histSvc= CompFactory.THistSvc()
    # output ntuple
    histSvc.Output +=  [ "CKF2Alignment DATAFILE='kfalignment_mc.root' OPT='RECREATE'"]
    acc.addService(histSvc)
    acc.addEventAlgo(ckf2fit)
    return acc

if __name__ == "__main__":

#   log.setLevel(INFO)
   Configurable.configurableRun3Behavior = True
   
   # Configure
   configFlags = initConfigFlags()
   configFlags.Input.Files = [ '' ] #input RDO
   configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"
   configFlags.GeoModel.FaserVersion     = "FASERNU-03"           # FASER cosmic ray geometry (station 2 only)
   configFlags.TrackingGeometry.MaterialSource = "material-maps.json" # material map
   configFlags.Input.isMC = True
   #configFlags.GeoModel.Align.Dynamic = False
   configFlags.Input.ProjectName = "data21"                     # Needed to bypass autoconfig
   configFlags.Beam.NumberOfCollisions = 0.
   configFlags.addFlag("Input.InitialTimeStamp", 0)
   configFlags.Exec.MaxEvents = -1
   configFlags.Output.doWriteESD = False
   #configFlags.Concurrency.NumThreads = 1
#   configFlags.addFlag("Alignment.Output", "ckf_alignment.root")
   configFlags.fillFromArgs(sys.argv[1:])
   configFlags.dump()
   configFlags.lock()
   
   from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
   # Core components
   acc = MainServicesCfg(configFlags)
   acc.merge(FaserGeometryCfg(configFlags))
   acc.merge(PoolReadCfg(configFlags))
   acc.merge(PoolWriteCfg(configFlags))
   from FaserByteStreamCnvSvc.FaserByteStreamCnvSvcConfig import FaserByteStreamCnvSvcCfg
   # Inner Detector
   acc.merge(FaserSCT_ClusterizationCfg(configFlags))
   acc.merge(TrackerSpacePointFinderCfg(configFlags))
   acc.merge(SegmentFitAlgCfg(configFlags, SharedHitFraction=0.61, MinClustersPerFit=5, TanThetaXZCut=0.083))
   acc.merge(GhostBustersCfg(configFlags))
   acc.merge(CKF2AlignmentCfg(configFlags))
   logging.getLogger('forcomps').setLevel(INFO)
   acc.foreach_component("*").OutputLevel = INFO
   acc.foreach_component("*ClassID*").OutputLevel = INFO
   
   # Execute and finish
   sc = acc.run()
   sys.exit(not sc.isSuccess())
