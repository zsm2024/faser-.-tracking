#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentFactory import CompFactory

def TrenchGeometryCfg( flags ):
    from FaserGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )

    if flags.Detector.GeometryTrench:
        TrenchDetectorTool=CompFactory.TrenchDetectorTool
        acc.getPrimary().DetectorTools += [ TrenchDetectorTool() ]
    return acc
