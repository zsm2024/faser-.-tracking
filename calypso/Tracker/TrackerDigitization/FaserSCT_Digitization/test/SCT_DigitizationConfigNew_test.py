#!/usr/bin/env python
"""Run tests on SCT_DigitizationConfigNew.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg
from FaserSCT_Digitization.FaserSCT_DigitizationConfigNew import FaserSCT_DigitizationHSCfg

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
# Configure
ConfigFlags.Input.Files = defaultTestFiles.HITS
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"             # Always needed; must match FaserVersion
#ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.lock()
# Construct our accumulator to run
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(FaserSCT_DigitizationHSCfg(ConfigFlags))
# Dump config
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
acc.run(maxEvents=3)

