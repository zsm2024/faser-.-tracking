#!/usr/bin/env python
"""Test various ComponentAccumulator Digitization configuration modules

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
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
#from Digitization.DigitizationParametersConfig import writeDigitizationMetadata
from WaveRecAlgs.WaveRecAlgsConfig import WaveformReconstructionCfg
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from TrackerSegmentFit.TrackerSegmentFitConfig import SegmentFitAlgCfg
from TrackerSpacePointFormation.TrackerSpacePointFormationConfig import TrackerSpacePointFinderCfg
#from MCTruthSimAlgs.RecoTimingConfig import MergeRecoTimingObjCfg

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

# Configure
configFlags = initConfigFlags()
configFlags.Input.Files = ['my.RDO.pool.root']
configFlags.Output.ESDFileName = "segmentFit.ESD.pool.root"
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"
configFlags.IOVDb.DatabaseInstance = "OFLP200"
configFlags.Input.ProjectName = "data21"
configFlags.Input.isMC = True
configFlags.GeoModel.FaserVersion = "FASER-01"
configFlags.Common.isOnline = False
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.addFlag("Output.xAODFileName", "segmentFit.xAOD.pool.root")
# configFlags.Detector.GeometryFaserSCT = True
configFlags.lock()

# Core components
acc = MainServicesCfg(configFlags)
acc.merge(PoolReadCfg(configFlags))
acc.merge(PoolWriteCfg(configFlags))

acc.merge(FaserSCT_ClusterizationCfg(configFlags))
acc.merge(SegmentFitAlgCfg(configFlags))
#acc.getEventAlgo("Tracker::SegmentFitAlg").OutputLevel = DEBUG

from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
itemList = ["xAOD::EventInfo#*",
            "xAOD::EventAuxInfo#*",
            "xAOD::FaserTriggerData#*",
            "xAOD::FaserTriggerDataAux#*",
            "FaserSCT_RDO_Container#*",
            "Tracker::FaserSCT_ClusterContainer#*",
            "TrackCollection#*"
            ]
acc.merge(OutputStreamCfg(configFlags, "ESD", itemList))

# Dump config
# logging.getLogger('forcomps').setLevel(VERBOSE)
# acc.foreach_component("*").OutputLevel = VERBOSE
# acc.foreach_component("*ClassID*").OutputLevel = INFO
# acc.getCondAlgo("FaserSCT_AlignCondAlg").OutputLevel = VERBOSE
# acc.getCondAlgo("FaserSCT_DetectorElementCondAlg").OutputLevel = VERBOSE
# acc.getService("StoreGateSvc").Dump = True
# acc.getService("ConditionStore").Dump = True
# acc.printConfig(withDetails=True)
# configFlags.dump()

# Execute and finish
sc = acc.run(maxEvents=-1)
sys.exit(not sc.isSuccess())
