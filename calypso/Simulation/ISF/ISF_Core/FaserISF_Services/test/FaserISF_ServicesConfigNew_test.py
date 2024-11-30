#!/usr/bin/env python
"""Run tests on FaserISF_ServicesConfigNew.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == '__main__':
  from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
  import os

  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = 1


  #import config flags
  from AthenaConfiguration.AllConfigFlags import initConfigFlags
  configFlags = initConfigFlags()

  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  configFlags.Input.Files = defaultTestFiles.EVNT

  configFlags.Sim.WorldRRange = 15000
  configFlags.Sim.WorldZRange = 27000 #change defaults?
  # Finalize 
  configFlags.lock()

  from FaserISF_Services.FaserISF_ServicesConfigNew import FaserTruthServiceCfg, FaserGeoIDSvcCfg

  ## Initialize a new component accumulator
  cfg = MainServicesCfg(configFlags)

  #add the algorithm
  cfg.merge(FaserTruthServiceCfg(configFlags))
  cfg.merge(InputConverterCfg(configFlags))
  cfg.merge(FaserGeoIDSvcCfg(configFlags))

  # Dump config
  cfg.printConfig(withDetails=True, summariseProps = True)
  configFlags.dump()


  f=open("test.pkl","wb")
  cfg.store(f)
  f.close()
