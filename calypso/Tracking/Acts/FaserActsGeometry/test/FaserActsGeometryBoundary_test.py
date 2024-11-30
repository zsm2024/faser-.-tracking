#!/usr/bin/env python
"""
Copyright (C) 2002-2023 CERN for the benefit of the ATLAS and FASER collaboration
"""


def FaserActsGeometryBoundaryTestCfg(flags, name="FaserActsGeometryBoundaryTestAlg", **kwargs):

    from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    from FaserActsGeometry.ActsGeometryConfig import ActsTrackingGeometryToolCfg
    from AthenaConfiguration.ComponentFactory import CompFactory

    acc = FaserSCT_GeometryCfg(flags)
    acc.merge(MagneticFieldSvcCfg(flags))
    result, actsTrackingGeometryTool = ActsTrackingGeometryToolCfg(flags)
    test_alg = CompFactory.FaserActsGeometryBoundaryTestAlg
    test_alg.TrackingGeometryTool = actsTrackingGeometryTool
    acc.merge(result)
    acc.addEventAlgo(test_alg(name, **kwargs))
    return acc


if __name__ == "__main__":
    
    import sys
    from AthenaCommon.Configurable import Configurable
    from CalypsoConfiguration.AllConfigFlags import initConfigFlags
    from CalypsoConfiguration.MainServicesConfig import MainServicesCfg
    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg

    Configurable.configurableRun3Behavior = True
    configFlags = initConfigFlags()
    configFlags.Input.isMC = False
    #configFlags.GeoModel.Align.Dynamic = False
    configFlags.IOVDb.DatabaseInstance = "CONDBR3"
    configFlags.IOVDb.GlobalTag = "OFLCOND-FASER-04"
    configFlags.GeoModel.FaserVersion = "FASERNU-03"
    configFlags.Detector.GeometryFaserSCT = True
    configFlags.lock()

    acc = MainServicesCfg(configFlags)
    acc.merge(FaserGeometryCfg(configFlags))
    acc.merge(FaserActsGeometryBoundaryTestCfg(configFlags))

    replicaSvc = acc.getService("DBReplicaSvc")
    replicaSvc.COOLSQLiteVetoPattern = ""
    replicaSvc.UseCOOLSQLite = True
    replicaSvc.UseCOOLFrontier = False
    replicaSvc.UseGeomSQLite = True

    sys.exit(int(acc.run(maxEvents=1).isFailure()))
