#!/usr/bin/env python
"""
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

import sys
from AthenaCommon.Logging import log, logging
from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from TrackerSegmentFit.TrackerSegmentFitConfig import SegmentFitAlgCfg
from TrackSeedPerformanceWriter.TrackSeedPerformanceWriterConfig import TrackSeedPerformanceWriterCfg

log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

configFlags = initConfigFlags()
configFlags.Input.Files = ['my.RDO.pool.root']
configFlags.Output.ESDFileName = "seeds.ESD.pool.root"
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"
configFlags.IOVDb.DatabaseInstance = "OFLP200"
configFlags.Input.ProjectName = "data21"
configFlags.Input.isMC = True
configFlags.GeoModel.FaserVersion = "FASER-01"
configFlags.Common.isOnline = False
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.lock()

acc = MainServicesCfg(configFlags)
acc.merge(PoolReadCfg(configFlags))
acc.merge(PoolWriteCfg(configFlags))
acc.merge(FaserSCT_ClusterizationCfg(configFlags))
acc.merge(SegmentFitAlgCfg(configFlags, MaxClusters=20, TanThetaCut=0.1))
acc.merge(TrackSeedPerformanceWriterCfg(configFlags))
#acc.getEventAlgo("Tracker::SegmentFitAlg").OutputLevel = DEBUG
acc.getEventAlgo("Tracker::TrackSeedPerformanceWriter").OutputLevel = DEBUG

sc = acc.run(maxEvents=1000)
sys.exit(not sc.isSuccess())
