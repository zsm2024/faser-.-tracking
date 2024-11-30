#!/usr/bin/env python
""""
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import sys
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from TrackerData.TrackerTruthDataConfig import TrackerTruthDataCfg

log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

configFlags = initConfigFlags()
configFlags.Input.Files = ["my.HITS.pool.root"]
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"
configFlags.IOVDb.DatabaseInstance = "OFLP200"
configFlags.Input.ProjectName = "data22"
configFlags.Input.isMC = True
configFlags.GeoModel.FaserVersion = "FASER-01"
configFlags.Common.isOnline = False
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.Detector.GeometryFaserSCT = True
configFlags.lock()

# Core components
acc = MainServicesCfg(configFlags)
acc.merge(PoolReadCfg(configFlags))

acc.merge(TrackerTruthDataCfg(configFlags))
acc.getEventAlgo("Tracker::TrackerTruthDataAlg").OutputLevel = DEBUG

# Execute and finish
sc = acc.run(maxEvents=-1)
sys.exit(not sc.isSuccess())
