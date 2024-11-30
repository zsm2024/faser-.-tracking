"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentFactory import CompFactory
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg


def TrackerTruthCfg(flags, **kwargs):
    acc = FaserSCT_GeometryCfg(flags)
    kwargs.setdefault("FaserSiHitCollection", "SCT_Hits")
    TrackerTruthAlg = CompFactory.Tracker.TrackerTruthAlg
    acc.addEventAlgo(TrackerTruthAlg(**kwargs))

    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += ["HIST1 DATAFILE='TrackerTruth.root' OPT='RECREATE'"]
    acc.addService(thistSvc)

    return acc
