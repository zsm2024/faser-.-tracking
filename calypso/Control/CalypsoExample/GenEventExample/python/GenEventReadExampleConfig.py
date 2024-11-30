# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import sys
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def GenEventReadExampleCfg(flags, name="GenEventReadExampleAlg", **kwargs):

    a = ComponentAccumulator()

    GenEventReadAlg = CompFactory.GenEventReadAlg
    a.addEventAlgo(GenEventReadAlg(name, **kwargs))

    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += ["HIST DATAFILE='genEventHist.root' OPT='RECREATE'"]
    a.addService(thistSvc)
    return a


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags

    Configurable.configurableRun3Behavior = True
    
# Flags for this job
    configFlags = initConfigFlags()
    configFlags.Input.isMC = True                                # Needed to bypass autoconfig
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-03"             # Always needed; must match FaserVersion
    configFlags.GeoModel.FaserVersion     = "FASERNU-04"           # Default FASER geometry
    # configFlags.Detector.EnableFaserSCT = True
    configFlags.Input.Files = ["my.cosmics.pool.root"]
    configFlags.lock()

# Configure components
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc = MainServicesCfg(configFlags)
    acc.merge(PoolReadCfg(configFlags))

# Set things up to create a conditions DB with neutral Tracker alignment transforms
    acc.merge(GenEventReadExampleCfg(configFlags))

# Configure verbosity    
    # configFlags.dump()
    # logging.getLogger('forcomps').setLevel(VERBOSE)
    # acc.foreach_component("*").OutputLevel = VERBOSE
    # acc.foreach_component("*ClassID*").OutputLevel = INFO
    # log.setLevel(VERBOSE)
    
# Execute and finish
    sys.exit(int(acc.run(maxEvents=-1).isFailure()))
