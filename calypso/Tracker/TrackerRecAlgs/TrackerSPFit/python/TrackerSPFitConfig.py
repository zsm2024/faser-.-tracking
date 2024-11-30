"""Define methods to construct configured SCT Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
#from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
Tracker__TrackerSPFit = CompFactory.getComps("Tracker::TrackerSPFit")

#from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg

def TrackerSPFitBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for TrackerSpacePointMakerAlg"""
#    acc = ComponentAccumulator()
    acc = FaserSCT_GeometryCfg(flags)
    kwargs.setdefault("SpacePointsSCTName", "SCT_SpacePointContainer")
    kwargs.setdefault("SCT_ClustersName", "SCT_ClusterContainer")
    acc.addEventAlgo(Tracker__TrackerSPFit(**kwargs))
    return acc


def TrackerSPFitCfg(flags, **kwargs):
    acc=TrackerSPFitBasicCfg(flags, **kwargs)
    THistSvc=CompFactory.THistSvc
    histSvc= THistSvc()
    histSvc.Output += [ "TrackerSPFit DATAFILE='sp_fit.root' OPT='RECREATE'" ]
    acc.addService(histSvc)
    return acc
