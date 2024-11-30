#!/usr/bin/env python

import sys
from AthenaCommon.Logging import log, logging
from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
# from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from TrackerSpacePointFormation.TrackerSpacePointFormationConfig import TrackerSpacePointFinderCfg
from TrackerSegmentFit.TrackerSegmentFitConfig import SegmentFitAlgCfg
from FaserActsKalmanFilter.CombinatorialKalmanFilterConfig import CombinatorialKalmanFilterCfg

log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

configFlags = initConfigFlags()
configFlags.Input.Files = ['my.RDO.pool.root']
configFlags.Output.ESDFileName = "CKF.ESD.pool.root"
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"
configFlags.GeoModel.FaserVersion = "FASER-01"
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
# configFlags.TrackingGeometry.MaterialSource = "Input"
configFlags.lock()

acc = MainServicesCfg(configFlags)
acc.merge(PoolReadCfg(configFlags))
# acc.merge(PoolWriteCfg(configFlags))

acc.merge(FaserSCT_ClusterizationCfg(configFlags))
acc.merge(TrackerSpacePointFinderCfg(configFlags))
acc.merge(SegmentFitAlgCfg(configFlags, SharedHitFraction=0.51, MinClustersPerFit=5, TanThetaCut=0.25))
acc.merge(CombinatorialKalmanFilterCfg(configFlags, noDiagnostics=True))
acc.getEventAlgo("CombinatorialKalmanFilterAlg").OutputLevel = VERBOSE

# logging.getLogger('forcomps').setLevel(INFO)
# acc.foreach_component("*").OutputLevel = INFO
# acc.foreach_component("*ClassID*").OutputLevel = INFO
# acc.getService("StoreGateSvc").Dump = True
# acc.getService("ConditionStore").Dump = True
# acc.printConfig(withDetails=True)
# configFlags.dump()

sc = acc.run(maxEvents=-1)
sys.exit(not sc.isSuccess())
