#!/usr/bin/env python
"""Run tests on VetoNuGeoModel configuration

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
    from VetoNuGeoModel.VetoNuGeoModelConfig import VetoNuGeometryCfg
    acc = VetoNuGeometryCfg(configFlags)
    f=open('VetoNuGeometryCfg.pkl','wb')
    acc.store(f)
    f.close()
