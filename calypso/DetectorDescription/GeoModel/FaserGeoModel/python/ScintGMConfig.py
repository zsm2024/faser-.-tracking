#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def ScintGeometryCfg (flags):
    acc = ComponentAccumulator()

    from VetoNuGeoModel.VetoNuGeoModelConfig import VetoNuGeometryCfg
    acc.merge(VetoNuGeometryCfg( flags ))

    from VetoGeoModel.VetoGeoModelConfig import VetoGeometryCfg
    acc.merge(VetoGeometryCfg( flags ))

    from TriggerGeoModel.TriggerGeoModelConfig import TriggerGeometryCfg
    acc.merge(TriggerGeometryCfg( flags ))
    
    from PreshowerGeoModel.PreshowerGeoModelConfig import PreshowerGeometryCfg
    acc.merge(PreshowerGeometryCfg( flags ))
    

    return acc


if __name__ == "__main__":
  # import os
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  from CalypsoConfiguration.AllConfigFlags import initConfigFlags
  from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
  from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
  # Set up logging and new style config
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = True

  configFlags = initConfigFlags()
  configFlags.GeoModel.FaserVersion = "Faser-01"

  # from AthenaConfiguration.TestDefaults import defaultTestFiles
  # Provide MC input
  # configFlags.Input.Files = defaultTestFiles.HITS
  configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"             # Always needed; must match FaserVersion
  configFlags.Detector.GeometryVeto   = True
  configFlags.Detector.GeometryTrigger= True
  configFlags.Detector.GeometryPreshower= True
  #configFlags.GeoModel.Align.Dynamic    = False
  ##from AthenaConfiguration.TestDefaults import defaultTestFiles
  #
  configFlags.lock()
  # Construct ComponentAccumulator
  acc = MainServicesCfg(configFlags)
  acc.merge(PoolReadCfg(configFlags))
  acc.merge(ScintGeometryCfg(configFlags)) # FIXME This sets up the whole Scint geometry would be nicer just to set up min required
  #acc.getService("StoreGateSvc").Dump=True
  acc.getService("ConditionStore").Dump=True
  acc.printConfig(withDetails=True)
  f=open('ScintGMCfg2.pkl','wb')
  acc.store(f)
  f.close()
  configFlags.dump()
  # Execute and finish
  acc.run(maxEvents=3)
