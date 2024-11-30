"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentFactory import CompFactory
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg


def NoisyStripFinderBasicCfg(flags, **kwargs):
    acc = FaserSCT_GeometryCfg(flags)
    kwargs.setdefault("DataObjectName", "SCT_RDOs")
    kwargs.setdefault("OutputHistRootName", "NoisyStripFinderHist.root")
    kwargs.setdefault("TriggerMask", 0x10)
    Tracker__NoisyStripFinder = CompFactory.Tracker.NoisyStripFinder
    acc.addEventAlgo(Tracker__NoisyStripFinder(**kwargs))
    return acc


def NoisyStripFinderCfg(flags, **kwargs):
    acc = NoisyStripFinderBasicCfg(flags, **kwargs)
    return acc

