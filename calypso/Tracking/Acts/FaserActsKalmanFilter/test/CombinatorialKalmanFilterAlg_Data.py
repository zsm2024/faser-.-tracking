#!/usr/bin/env python

import sys
from AthenaCommon.Logging import log, logging
from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from FaserByteStreamCnvSvc.FaserByteStreamCnvSvcConfig import FaserByteStreamCnvSvcCfg
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from FaserActsKalmanFilter.CombinatorialKalmanFilterConfig import CombinatorialKalmanFilterCfg
from TrackerSegmentFit.TrackerSegmentFitConfig import SegmentFitAlgCfg

log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

configFlags = initConfigFlags()
configFlags.Input.Files = ['/home/tboeckh/Documents/data/raw/TI12/MiddleStationTrack.raw']
configFlags.Output.ESDFileName = "CKF.ESD.pool.root"
configFlags.addFlag("Output.xAODFileName", f"CKF.xAOD.root")
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"
configFlags.IOVDb.DatabaseInstance = "OFLP200"
configFlags.Input.ProjectName = "data21"
configFlags.Input.isMC = False
configFlags.GeoModel.FaserVersion = "FASER-01"
configFlags.Common.isOnline = False
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.Detector.GeometryFaserSCT = True
configFlags.lock()

acc = MainServicesCfg(configFlags)
acc.merge(PoolWriteCfg(configFlags))
acc.merge(FaserByteStreamCnvSvcCfg(configFlags))
acc.merge(FaserSCT_ClusterizationCfg(configFlags, DataObjectName="SCT_LEVELMODE_RDOs", ClusterToolTimingPattern="X1X"))
acc.merge(SegmentFitAlgCfg(configFlags))
acc.merge(CombinatorialKalmanFilterCfg(configFlags, SummaryWriter=False, StatesWriter=False, PerformanceWriter=False))
acc.getEventAlgo("CombinatorialKalmanFilterAlg").OutputLevel = VERBOSE

from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
itemList = ["xAOD::EventInfo#*",
            "xAOD::EventAuxInfo#*",
            "FaserSCT_RDO_Container#*",
            "Tracker::FaserSCT_ClusterContainer#*",
            "TrackCollection#*"]
acc.merge(OutputStreamCfg(configFlags, "xAOD", itemList))

print( "Writing out xAOD objects:" )
print( acc.getEventAlgo("OutputStreamxAOD").ItemList )

# logging.getLogger('forcomps').setLevel(VERBOSE)
# acc.foreach_component("*").OutputLevel = VERBOSE
# acc.foreach_component("*ClassID*").OutputLevel = INFO
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

sc = acc.run(maxEvents=20)
sys.exit(not sc.isSuccess())
