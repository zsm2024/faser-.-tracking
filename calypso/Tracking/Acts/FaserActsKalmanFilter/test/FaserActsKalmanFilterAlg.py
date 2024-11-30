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
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from TrackerSegmentFit.TrackerSegmentFitConfig import SegmentFitAlgCfg
from TrackerSpacePointFormation.TrackerSpacePointFormationConfig import TrackerSpacePointFinderCfg
from TruthSeededTrackFinder.TruthSeededTrackFinderConfig import TruthSeededTrackFinderCfg
from FaserActsKalmanFilter.FaserActsKalmanFilterConfig import FaserActsKalmanFilterCfg


log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

# Configure
configFlags = initConfigFlags()
configFlags.Input.Files = ['../my.RDO.pool.root']
configFlags.Output.ESDFileName = "FaserActsKalmanFilter.ESD.root"
configFlags.Output.AODFileName = "FaserActsKalmanFilter.AOD.pool.root"
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
#configFlags.Concurrency.NumThreads = 1
configFlags.lock()

# Core components
acc = MainServicesCfg(configFlags)
acc.merge(PoolReadCfg(configFlags))

# Inner Detector
acc.merge(FaserSCT_ClusterizationCfg(configFlags))
acc.merge(SegmentFitAlgCfg(configFlags))
# acc.merge(TrackerSpacePointFinderCfg(configFlags))
# acc.merge(TruthSeededTrackFinderCfg(configFlags))
acc.merge(FaserActsKalmanFilterCfg(configFlags))
acc.getEventAlgo("FaserActsKalmanFilterAlg").OutputLevel = DEBUG

# logging.getLogger('forcomps').setLevel(VERBOSE)
# acc.foreach_component("*").OutputLevel = VERBOSE
# acc.foreach_component("*ClassID*").OutputLevel = INFO
# acc.getService("StoreGateSvc").Dump = True
# acc.getService("ConditionStore").Dump = True
# acc.printConfig(withDetails=True)
# configFlags.dump()

# Execute and finish
sc = acc.run(maxEvents=1000)
sys.exit(not sc.isSuccess())
