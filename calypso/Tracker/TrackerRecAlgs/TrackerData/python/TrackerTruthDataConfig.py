"""
Copyright (C) 2022 CERN for the benefit of the FASER collaboration
"""

from AthenaConfiguration.ComponentFactory import CompFactory
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg


def TrackerTruthDataCfg(flags, **kwargs):
    acc = FaserSCT_GeometryCfg(flags)
    kwargs.setdefault("FaserSiHitCollection", "SCT_Hits")
    TrackerTruthDataAlg = CompFactory.Tracker.TrackerTruthDataAlg
    acc.addEventAlgo(TrackerTruthDataAlg(**kwargs))

    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += ["HIST1 DATAFILE='TrackerTruthData.root' OPT='RECREATE'"]
    acc.addService(thistSvc)

    return acc
