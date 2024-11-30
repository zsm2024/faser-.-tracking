# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS and FASER collaborations

#!/usr/bin/env python
import sys
from AthenaCommon.Constants import VERBOSE, INFO
from AthenaConfiguration.ComponentFactory import CompFactory

def SimHitAlgCfg(flags, name="SimHitAlg", **kwargs):

    # Initialize GeoModel
    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
    a = FaserGeometryCfg(flags)

    # Configure the algorithm itself
    SimHitAlg = CompFactory.SimHitAlg
    a.addEventAlgo(SimHitAlg(name, **kwargs))

    # Set up histogramming
    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += ["HIST DATAFILE='simHitAlg.root' OPT='RECREATE'"]
    a.addService(thistSvc)

    return a

if __name__ == "__main__":
    from AthenaCommon.Logging import log#, logging
    from AthenaCommon.Configurable import Configurable
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags

    Configurable.configurableRun3Behavior = True
    
# Flags for this job
    configFlags = initConfigFlags()
    configFlags.Input.Files = ["my.HITS.pool.root"]              # input file(s)
    configFlags.Input.isMC = True                                # Needed to bypass autoconfig
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"             # Always needed; must match FaserVersion
    configFlags.GeoModel.FaserVersion     = "FASERNU-03"         # Default FASER geometry
    configFlags.Detector.GeometryEmulsion = True
    configFlags.Detector.GeometryTrench   = True
    configFlags.lock()

# Configure components
# Core framework
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(configFlags)

# Data input
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(configFlags))

# Algorithm
    acc.merge(SimHitAlgCfg(configFlags, McEventCollection = "TruthEvent",
                                        PrintNeutrinoHits = False,
                                        PrintTrackerHits = False,
                                        PrintScintillatorHits = False,
                                        PrintCalorimeterHits = False))

# Configure verbosity    
    msgSvc = acc.getService("MessageSvc")
    msgSvc.Format = "% F%30W%S%7W%R%T %0W%M"
    # configFlags.dump()
    # logging.getLogger('forcomps').setLevel(VERBOSE)
    #acc.foreach_component("*").OutputLevel = VERBOSE
    #acc.foreach_component("*ClassID*").OutputLevel = INFO
    #log.setLevel(VERBOSE)
    
# Execute and finish
    sys.exit(int(acc.run(maxEvents=-1).isFailure()))
