"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentFactory import CompFactory
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg


def TrackerSegmentFitDataCfg(flags, **kwargs):
    acc = FaserSCT_GeometryCfg(flags)
    kwargs.setdefault("TrackCollection", "SegmentFit")
    TrackerSegmentFitDataAlg = CompFactory.Tracker.TrackerSegmentFitDataAlg
    acc.addEventAlgo(TrackerSegmentFitDataAlg(**kwargs))

    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += ["HIST2 DATAFILE='TrackerSegmentFitData.root' OPT='RECREATE'"]
    acc.addService(thistSvc)

    return acc
