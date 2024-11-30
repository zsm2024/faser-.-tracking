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
from FaserActsGeometry.FaserActsWriteTrackingGeometryConfig import FaserActsWriteTrackingGeometryCfg

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

# Configure
configFlags = initConfigFlags()
configFlags.Input.Files = ["myevt4.HITS.pool.root"]
#configFlags.Output.RDOFileName = "myRDO_sp.pool.root"
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"             # Always needed; must match FaserVersion
configFlags.GeoModel.FaserVersion = "FASERNU-03"               # Always needed
#configFlags.GeoModel.FaserVersion = "FASERNU-03"               # Always needed
# Workaround for bug/missing flag; unimportant otherwise 
configFlags.addFlag("Input.InitialTimeStamp", 0)
# Workaround to avoid problematic ISF code
configFlags.GeoModel.Layout = "Development"
configFlags.Detector.GeometryFaserSCT = True
#configFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01" # Always needed to fool autoconfig; value ignored
#configFlags.GeoModel.Align.Dynamic = False
#configFlags.Concurrency.NumThreads = 1
#configFlags.Beam.NumberOfCollisions = 0.
configFlags.lock()

# Core components
acc = MainServicesCfg(configFlags)
acc.merge(PoolReadCfg(configFlags))
#acc.merge(PoolWriteCfg(configFlags))
from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
acc.merge(FaserGeometryCfg(configFlags))

# Inner Detector
acc.merge(FaserActsWriteTrackingGeometryCfg(configFlags))

# Output Stream customization
#oStream = acc.getEventAlgo("OutputStreamRDO")
#oStream.ItemList += ["EventInfo#*",
#                     "McEventCollection#TruthEvent",
#                     "McEventCollection#BeamTruthEvent"
#                    ]
                    
# Timing
#acc.merge(MergeRecoTimingObjCfg(configFlags))

# Dump config
logging.getLogger('forcomps').setLevel(VERBOSE)
acc.foreach_component("*").OutputLevel = VERBOSE
acc.foreach_component("*ClassID*").OutputLevel = INFO
#acc.getCondAlgo("FaserActsAlignmentCondAlg").OutputLevel = VERBOSE
#acc.getCondAlgo("FaserNominalAlignmentCondAlg").OutputLevel = VERBOSE
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
configFlags.dump()
# Execute and finish
sc = acc.run(maxEvents=1)
# Success should be 0
sys.exit(not sc.isSuccess())
