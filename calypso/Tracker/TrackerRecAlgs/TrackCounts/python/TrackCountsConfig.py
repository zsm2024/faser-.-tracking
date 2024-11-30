"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentFactory import CompFactory
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg


def TrackCountsAlgBasicCfg(flags, **kwargs):
    acc = FaserSCT_GeometryCfg(flags)
    kwargs.setdefault("TrackCollection", "ClusterFit")
    Tracker__TrackCountsAlg = CompFactory.Tracker.TrackCountsAlg
    acc.addEventAlgo(Tracker__TrackCountsAlg(**kwargs))

    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += ["HIST1 DATAFILE='TrackCountsHist.root' OPT='RECREATE'"]
    acc.addService(thistSvc)
    return acc


def TrackCountsAlgCfg(flags, **kwargs):
    acc = TrackCountsAlgBasicCfg(flags, **kwargs)
    return acc
