#!/usr/bin/env python
"""Run a test on Atlas Geometry configuration using a EVNT file as input

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == "__main__":
    import os
    import sys
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import VERBOSE, INFO
    from AthenaCommon.Configurable import Configurable
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    from AthenaConfiguration.Enums import ProductionStep
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    # Set up logging and new style config
    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
    log.setLevel(VERBOSE)
    Configurable.configurableRun3Behavior = True

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    # Provide MC input
    configFlags = initConfigFlags()
    configFlags.Common.ProductionStep = ProductionStep.Simulation
    configFlags.Input.Files = defaultTestFiles.EVNT
    configFlags.GeoModel.FaserVersion = "FASER-01"
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"             # Always needed; must match FaserVersion
    #configFlags.GeoModel.Align.Dynamic    = False
    configFlags.lock()

    # Construct ComponentAccumulator
    acc = MainServicesCfg(configFlags)
    acc.merge(PoolReadCfg(configFlags))
    acc.merge(FaserGeometryCfg(configFlags))
    # acc.getService("StoreGateSvc").Dump=True
    # acc.getService("ConditionStore").Dump=True

    # from GaudiCoreSvc.GaudiCoreSvcConf import MessageSvc
    # msgSvc = MessageSvc()
    # acc.addService(msgSvc)

    from AthenaCommon.Logging import logging
    logging.getLogger('forcomps').setLevel(VERBOSE)
    acc.foreach_component("*").OutputLevel = VERBOSE

    acc.foreach_component("*ClassID*").OutputLevel = INFO
    
    acc.printConfig(withDetails=True)
    f=open('FaserGeoModelCfg_EVNT.pkl','wb')
    acc.store(f)
    f.close()
    configFlags.dump()
    # Execute and finish
    sys.exit(int(acc.run(maxEvents=3).isFailure()))
