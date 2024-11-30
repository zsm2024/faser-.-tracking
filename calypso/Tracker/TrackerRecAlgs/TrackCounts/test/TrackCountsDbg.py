#!/usr/bin/env python

"""
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

import sys
from AthenaCommon.Logging import log, logging
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from FaserByteStreamCnvSvc.FaserByteStreamCnvSvcConfig import FaserByteStreamCnvSvcCfg
from WaveRecAlgs.WaveRecAlgsConfig import WaveformReconstructionCfg
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from TrackerClusterFit.TrackerClusterFitConfig import ClusterFitAlgCfg
from TrackCounts.TrackCountsConfig import TrackCountsAlgCfg

log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

configFlags = initConfigFlags()
configFlags.Input.Files = [
    '/eos/project/f/faser-commissioning/TI12Data/Run-001805/Faser-Physics-001805-00000.raw',
]

configFlags.Output.ESDFileName = "run001805_00.ESD.pool.root"
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"
configFlags.IOVDb.DatabaseInstance = "OFLP200"
configFlags.Input.ProjectName = "data21"
configFlags.Input.isMC = False
configFlags.Common.isOnline = False
configFlags.GeoModel.FaserVersion = "FASER-01"
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.Detector.GeometryFaserSCT = True

configFlags.lock()

acc = MainServicesCfg(configFlags)
acc.merge(PoolWriteCfg(configFlags))

acc.merge(FaserByteStreamCnvSvcCfg(configFlags))
acc.merge(WaveformReconstructionCfg(configFlags))
acc.merge(FaserSCT_ClusterizationCfg(configFlags, DataObjectName="SCT_EDGEMODE_RDOs"))
acc.merge(ClusterFitAlgCfg(configFlags))
acc.merge(TrackCountsAlgCfg(configFlags))

from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
itemList = [ "xAOD::EventInfo#*",
             "xAOD::EventAuxInfo#*",
             "FaserSCT_RDO_Container#*",
             "xAOD::FaserTriggerData#*",
             "xAOD::FaserTriggerDataAux#*",
             "RawWaveformContainer#*",
             "TrackCollection#*",
             "xAOD::WaveformHitContainer#*",
             "xAOD::WaveformHitAuxContainer#*",
             "xAOD::WaveformClock#*",
             "xAOD::WaveformClockAuxInfo#*",
             ]
acc.merge(OutputStreamCfg(configFlags, "ESD", itemList))
acc.getEventAlgo("OutputStreamESD").AcceptAlgs = ["Tracker::ClusterFitAlg"]

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

# Success should be 0
sys.exit(not sc.isSuccess())
