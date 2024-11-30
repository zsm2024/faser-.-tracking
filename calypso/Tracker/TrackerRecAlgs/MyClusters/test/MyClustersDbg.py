#!/usr/bin/env python

import sys
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from MyClusters.MyClustersConfig import MyClustersCfg


log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

# Configure
configFlags = initConfigFlags()
configFlags.Input.Files = ['my.RDO.pool.root']
configFlags.Output.ESDFileName = "MyClusters.ESD.root"
configFlags.Output.AODFileName = "MyClsuters.AOD.pool.root"
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.lock()

acc = MainServicesCfg(configFlags)
acc.merge(PoolReadCfg(configFlags))
acc.merge(FaserSCT_ClusterizationCfg(configFlags))
acc.merge(MyClustersCfg(configFlags))
acc.getEventAlgo("MyClustersAlg").OutputLevel = DEBUG

sc = acc.run(maxEvents=10)
sys.exit(not sc.isSuccess())
