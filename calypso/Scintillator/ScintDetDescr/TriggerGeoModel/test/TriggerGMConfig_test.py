#!/usr/bin/env python
"""Run tests on TriggerGeoModel configuration

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    configFlags = initConfigFlags()
    configFlags.Input.Files = defaultTestFiles.HITS
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-01"             # Always needed; must match FaserVersion
    #configFlags.GeoModel.Align.Dynamic    = False
    configFlags.lock()

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from TriggerGeoModel.TriggerGeoModelConfig import TriggerGeometryCfg
    acc = TriggerGeometryCfg(configFlags)
    f=open('TriggerGeometryCfg.pkl','wb')
    acc.store(f)
    f.close()
