"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentFactory import CompFactory
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg
from FaserActsGeometry.ActsGeometryConfig import ActsTrackingGeometrySvcCfg


def SeedingCfg(flags, **kwargs):
    acc = FaserSCT_GeometryCfg(flags)
    acts_tracking_geometry_svc = ActsTrackingGeometrySvcCfg(flags)
    acc.merge(acts_tracking_geometry_svc )
    seedingAlg = CompFactory.SeedingAlg(**kwargs)
    acc.addEventAlgo(seedingAlg)
    return acc
