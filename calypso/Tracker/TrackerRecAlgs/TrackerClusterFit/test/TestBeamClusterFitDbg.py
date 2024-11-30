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
from TrackerClusterFit.TrackerClusterFitConfig import ClusterFitAlgCfg
from TrackerSpacePointFormation.TrackerSpacePointFormationConfig import TrackerSpacePointFinderCfg
#from MCTruthSimAlgs.RecoTimingConfig import MergeRecoTimingObjCfg

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

# Configure
configFlags = initConfigFlags()
configFlags.Input.Files = [
    'tb.raw',
    #'/eos/project-f/faser-commissioning/TI12Data/Run-001332/Faser-Physics-001332-00000.raw',
    #'/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00000.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00001.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00002.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00003.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00004.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00005.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00006.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00007.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00008.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00009.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00010.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00011.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00012.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00013.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00014.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00015.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00016.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00017.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00018.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00019.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00020.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00021.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00022.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00023.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00024.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00025.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00026.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00027.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00028.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00029.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00030.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00031.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00032.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00033.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00034.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00035.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00036.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00037.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00038.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00039.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00040.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00041.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00042.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00043.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00044.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00045.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00046.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00047.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00048.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00049.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00050.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00051.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00052.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00053.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00054.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00055.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00056.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00057.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00058.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00059.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00060.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00061.raw',
    # '/eos/project-f/faser-commissioning/winter2020CosmicsStand/Run-000608/Faser-Physics-000608-00062.raw'
]
#configFlags.Output.ESDFileName = "run608.ESD.pool.root"
#configFlags.Output.ESDFileName = "run001332.ESD.pool.root"
configFlags.Output.ESDFileName = "tb.ESD.pool.root"
configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-TB00"             # Always needed; must match FaserVersion
configFlags.IOVDb.DatabaseInstance = "OFLP200"               # Use MC conditions for now
configFlags.Input.ProjectName = "data21"                     # Needed to bypass autoconfig
configFlags.Input.isMC = False                               # Needed to bypass autoconfig
configFlags.GeoModel.FaserVersion     = "FASER-TB00"         # FASER geometry
configFlags.Common.isOnline = False
#configFlags.GeoModel.Align.Dynamic = False
configFlags.Beam.NumberOfCollisions = 0.
configFlags.Detector.GeometryFaserSCT = True

configFlags.lock()

# Core components
acc = MainServicesCfg(configFlags)
#acc.merge(PoolReadCfg(configFlags))
acc.merge(PoolWriteCfg(configFlags))

#acc.merge(writeDigitizationMetadata(configFlags))

from FaserByteStreamCnvSvc.FaserByteStreamCnvSvcConfig import FaserByteStreamCnvSvcCfg
acc.merge(FaserByteStreamCnvSvcCfg(configFlags))
acc.merge(WaveformReconstructionCfg(configFlags))
acc.merge(FaserSCT_ClusterizationCfg(configFlags, DataObjectName="SCT_EDGEMODE_RDOs"))
acc.merge(ClusterFitAlgCfg(configFlags))
acc.merge(TrackerSpacePointFinderCfg(configFlags))
#acc.getEventAlgo("Tracker::ClusterFitAlg").OutputLevel = DEBUG

from AthenaConfiguration.ComponentFactory import CompFactory
decoderTool = CompFactory.RawWaveformDecoderTool(name = "RawWaveformDecoderTool", 
                                                   CaloChannels = [0, 1, 2, 3, 4, 5], 
                                                   PreshowerChannels = [6, 7], 
                                                   TriggerChannels = [8, 9],
                                                   VetoChannels=[])
acc.addPublicTool(decoderTool)

# explicitly save RDO information
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
# Timing
#acc.merge(MergeRecoTimingObjCfg(configFlags))

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
sc = acc.run(maxEvents=1000)

# Success should be 0
sys.exit(not sc.isSuccess())
