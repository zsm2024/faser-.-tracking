#!/usr/bin/env python

import sys
from AthenaCommon.Logging import log, logging
from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from FaserByteStreamCnvSvc.FaserByteStreamCnvSvcConfig import FaserByteStreamCnvSvcCfg
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from TrackerSegmentFit.TrackerSegmentFitConfig import SegmentFitAlgCfg
from TrackerSpacePointFormation.TrackerSpacePointFormationConfig import TrackerSpacePointFinderCfg
from FaserActsKalmanFilter.FaserActsKalmanFilterConfig import FaserActsKalmanFilterCfg


log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

# Configure
configFlags = initConfigFlags()
configFlags.Input.Files = ['/home/tboeckh/tmp/Faser-Physics-006470-00093.raw_middleStation.SPs']
configFlags.Output.ESDFileName = "MiddleStation-KalmanFilter.ESD.pool.root"
configFlags.Output.AODFileName = "MiddleStation-KalmanFilter.AOD.pool.root"
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"
configFlags.IOVDb.DatabaseInstance = "OFLP200"
configFlags.Input.ProjectName = "data21"
configFlags.Input.isMC = False
configFlags.GeoModel.FaserVersion = "FASER-02"
configFlags.Common.isOnline = False
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.Detector.GeometryFaserSCT = True
configFlags.lock()

# Core components
acc = MainServicesCfg(configFlags)
acc.merge(FaserByteStreamCnvSvcCfg(configFlags))
acc.merge(FaserSCT_ClusterizationCfg(configFlags, name="LevelClustering", DataObjectName="SCT_LEVELMODE_RDOs", ClusterToolTimingPattern="X1X"))
acc.merge(SegmentFitAlgCfg(configFlags, name=f"LevelFit", MaxClusters=44))
# acc.merge(FaserSCT_ClusterizationCfg(configFlags))
# acc.merge(SegmentFitAlgCfg(configFlags))
# acc.merge(TrackerSpacePointFinderCfg(configFlags))
acc.merge(FaserActsKalmanFilterCfg(configFlags))
acc.getEventAlgo("FaserActsKalmanFilterAlg").OutputLevel = DEBUG

replicaSvc = acc.getService("DBReplicaSvc")
replicaSvc.COOLSQLiteVetoPattern = ""
replicaSvc.UseCOOLSQLite = True
replicaSvc.UseCOOLFrontier = False
replicaSvc.UseGeomSQLite = True


# logging.getLogger('forcomps').setLevel(VERBOSE)
# acc.foreach_component("*").OutputLevel = VERBOSE
# acc.foreach_component("*ClassID*").OutputLevel = INFO
# acc.getService("StoreGateSvc").Dump = True
# acc.getService("ConditionStore").Dump = True
# acc.printConfig(withDetails=True)
# configFlags.dump()

# Execute and finish
sc = acc.run(maxEvents=-1)
sys.exit(not sc.isSuccess())
