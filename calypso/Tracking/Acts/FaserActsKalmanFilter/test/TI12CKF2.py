#!/usr/bin/env python

import sys
from AthenaCommon.Logging import log, logging
from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
# from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from FaserByteStreamCnvSvc.FaserByteStreamCnvSvcConfig import FaserByteStreamCnvSvcCfg
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from TrackerSpacePointFormation.TrackerSpacePointFormationConfig import TrackerSpacePointFinderCfg
from TrackerSegmentFit.TrackerSegmentFitConfig import SegmentFitAlgCfg
from FaserActsKalmanFilter.GhostBustersConfig import GhostBustersCfg
from FaserActsKalmanFilter.TI12CKF2Config import TI12CKF2Cfg

import argparse

parser = argparse.ArgumentParser()
parser.add_argument("file", nargs="+", help="full path to input file")
parser.add_argument("--nevents", "-n", default=-1, type=int, help="Number of events to process")
args = parser.parse_args()

log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

configFlags = initConfigFlags()
configFlags.Input.Files = args.file
configFlags.Output.ESDFileName = "CKF.ESD.pool.root"
configFlags.addFlag("Output.xAODFileName", f"CKF.xAOD.root")
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"
configFlags.IOVDb.DatabaseInstance = "OFLP200"
configFlags.Input.ProjectName = "data22"
configFlags.Input.isMC = False
configFlags.GeoModel.FaserVersion = "FASERNU-03"
configFlags.Common.isOnline = False
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.Detector.GeometryFaserSCT = True
configFlags.TrackingGeometry.MaterialSource = "geometry-maps.json"
configFlags.lock()

acc = MainServicesCfg(configFlags)
acc.merge(FaserGeometryCfg(configFlags))
acc.merge(PoolWriteCfg(configFlags))
acc.merge(FaserByteStreamCnvSvcCfg(configFlags, OccupancyCut=0.015))
acc.merge(FaserSCT_ClusterizationCfg(configFlags, DataObjectName="SCT_EDGEMODE_RDOs", ClusterToolTimingPattern="01X"))
acc.merge(TrackerSpacePointFinderCfg(configFlags))
acc.merge(SegmentFitAlgCfg(configFlags, SharedHitFraction=0.61, MinClustersPerFit=5, TanThetaXZCut=0.083))
acc.merge(GhostBustersCfg(configFlags))
acc.merge(TI12CKF2Cfg(configFlags, noDiagnostics=True))
acc.getEventAlgo("CKF2").OutputLevel = DEBUG

# logging.getLogger('forcomps').setLevel(VERBOSE)
# acc.foreach_component("*").OutputLevel = VERBOSE
# acc.foreach_component("*ClassID*").OutputLevel = VERBOSE
# acc.getService("StoreGateSvc").Dump = True
# acc.getService("ConditionStore").Dump = True
# acc.printConfig(withDetails=True)
# configFlags.dump()

# Hack to avoid problem with our use of MC databases when isMC = False
replicaSvc = acc.getService("DBReplicaSvc")
replicaSvc.COOLSQLiteVetoPattern = ""
replicaSvc.UseCOOLSQLite = True
replicaSvc.UseCOOLFrontier = False
replicaSvc.UseGeomSQLite = True

sc = acc.run(maxEvents=args.nevents)
sys.exit(not sc.isSuccess())
