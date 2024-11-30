#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
# from AthenaConfiguration.ComponentFactory import CompFactory

def FaserGeometryCfg (flags):
    acc = ComponentAccumulator()

    from EmulsionGeoModel.EmulsionGeoModelConfig import EmulsionGeometryCfg
    acc.merge(EmulsionGeometryCfg(flags))

    from FaserGeoModel.ScintGMConfig import ScintGeometryCfg
    acc.merge(ScintGeometryCfg(flags))

    from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg
    acc.merge(FaserSCT_GeometryCfg(flags))

    from DipoleGeoModel.DipoleGeoModelConfig import DipoleGeometryCfg
    acc.merge(DipoleGeometryCfg( flags ))

    from EcalGeoModel.EcalGeoModelConfig import EcalGeometryCfg
    acc.merge(EcalGeometryCfg( flags ))   

    from FaserGeoModel.TrenchGMConfig import TrenchGeometryCfg
    acc.merge(TrenchGeometryCfg( flags ))

    return acc
