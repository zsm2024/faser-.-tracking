"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentFactory import CompFactory
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg


def FaserSpacePointsCfg(flags, **kwargs):
    acc = FaserSCT_GeometryCfg(flags)
    Tracker__FaserSpacePoints = CompFactory.Tracker.FaserSpacePoints
    acc.addEventAlgo(Tracker__FaserSpacePoints(**kwargs))

    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += ["HIST DATAFILE='SpacePoints.root' OPT='RECREATE'"]
    acc.addService(thistSvc)
    return acc
