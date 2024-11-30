# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
from AthenaCommon.Logging import log, logging
from AthenaCommon.Constants import DEBUG, VERBOSE, INFO
from AthenaCommon.Configurable import Configurable
from CalypsoConfiguration.AllConfigFlags import initConfigFlags
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
# from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
# from Digitization.DigitizationParametersConfig import writeDigitizationMetadata
from TrackerPrepRawDataFormation.TrackerPrepRawDataFormationConfig import FaserSCT_ClusterizationCfg
from TrackerSpacePointFormation.TrackerSpacePointFormationConfig import TrackerSpacePointFinderCfg
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg

Tracker__TruthSeededTrackFinder, THistSvc=CompFactory.getComps("Tracker::FaserSpacePoints", "THistSvc")


def TruthSeededTrackFinderBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for TruthSeededTrackFinder"""
    acc = FaserSCT_GeometryCfg(flags)
    acc.addEventAlgo(Tracker__TruthSeededTrackFinder(**kwargs))
   # attach ToolHandles
    return acc

def TruthSeededTrackFinder_OutputCfg(flags):
    """Return ComponentAccumulator with Output for SCT. Not standalone."""
    acc = ComponentAccumulator()
    acc.merge(OutputStreamCfg(flags, "ESD"))
    ostream = acc.getEventAlgo("OutputStreamESD")
    ostream.TakeItemsFromInput = True
    return acc

def FaserSpacePointsCfg(flags, **kwargs):
    acc=TruthSeededTrackFinderBasicCfg(flags, **kwargs)
    histSvc= THistSvc()
    histSvc.Output += [ "TruthTrackSeeds DATAFILE='truthtrackseeds.root' OPT='RECREATE'" ]
    acc.addService(histSvc)
    acc.merge(TruthSeededTrackFinder_OutputCfg(flags))
    return acc

if __name__ == "__main__":
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = True

  # Configure
  configFlags = initConfigFlags()
  configFlags.Input.Files = ['my.RDO.pool.root']
  configFlags.Output.ESDFileName = "mySeeds.ESD.pool.root"
  configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"             # Always needed; must match FaserVersion
  configFlags.IOVDb.DatabaseInstance = "OFLP200"               # Use MC conditions for now
  configFlags.Input.ProjectName = "data20"                     # Needed to bypass autoconfig
  configFlags.Input.isMC = False                               # Needed to bypass autoconfig
  configFlags.GeoModel.FaserVersion     = "FASER-01"           # FASER cosmic ray geometry (station 2 only)
  configFlags.Common.isOnline = False
  #configFlags.GeoModel.Align.Dynamic = False
  configFlags.Beam.NumberOfCollisions = 0.

  configFlags.lock()

  # Core components
  acc = MainServicesCfg(configFlags)
  acc.merge(PoolReadCfg(configFlags))

  #acc.merge(writeDigitizationMetadata(configFlags))

  # Inner Detector
  acc.merge(FaserSCT_ClusterizationCfg(configFlags))
  acc.merge(TrackerSpacePointFinderCfg(configFlags))
  acc.merge(FaserSpacePointsCfg(configFlags))

  # Timing
  #acc.merge(MergeRecoTimingObjCfg(configFlags))

  # Dump config
  logging.getLogger('forcomps').setLevel(VERBOSE)
  acc.foreach_component("*").OutputLevel = VERBOSE
  acc.foreach_component("*ClassID*").OutputLevel = INFO
  # acc.getCondAlgo("FaserSCT_AlignCondAlg").OutputLevel = VERBOSE
  # acc.getCondAlgo("FaserSCT_DetectorElementCondAlg").OutputLevel = VERBOSE
  acc.getService("StoreGateSvc").Dump = True
  acc.getService("ConditionStore").Dump = True
  acc.printConfig(withDetails=True)
  configFlags.dump()

  # Execute and finish
  sc = acc.run(maxEvents=-1)

  # Success should be 0
  sys.exit(not sc.isSuccess())
