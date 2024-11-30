# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#!/usr/bin/env python
import sys
from AthenaConfiguration.ComponentFactory import CompFactory

def RDOReadExampleCfg(flags, name="RDOReadExampleAlg", **kwargs):

    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
    a = FaserGeometryCfg(flags)

    RDOReadAlg = CompFactory.RDOReadAlg
    a.addEventAlgo(RDOReadAlg(name, **kwargs))

    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += ["HIST DATAFILE='rdoReadHist.root' OPT='RECREATE'"]
    a.addService(thistSvc)
    return a


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags

    Configurable.configurableRun3Behavior = True
    
# Flags for this job
    configFlags = initConfigFlags()
    configFlags.Input.isMC = True                                # Needed to bypass autoconfig
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"             # Always needed; must match FaserVersion
    configFlags.GeoModel.FaserVersion     = "FASER-01"           # Default FASER geometry
    configFlags.Detector.GeometryFaserSCT = True
    configFlags.Input.Files = ["my.RDO.pool.root"]
    configFlags.lock()

# Configure components
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc = MainServicesCfg(configFlags)
    acc.merge(PoolReadCfg(configFlags))

# Set things up to create a conditions DB with neutral Tracker alignment transforms
    acc.merge(RDOReadExampleCfg(configFlags))

# Configure verbosity    
    # configFlags.dump()
    # logging.getLogger('forcomps').setLevel(VERBOSE)
    # acc.foreach_component("*").OutputLevel = VERBOSE
    # acc.foreach_component("*ClassID*").OutputLevel = INFO
    # log.setLevel(VERBOSE)
    
# Execute and finish
    sys.exit(int(acc.run(maxEvents=-1).isFailure()))
