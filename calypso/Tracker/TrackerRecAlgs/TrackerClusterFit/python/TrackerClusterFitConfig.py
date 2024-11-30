"""Define methods to construct configured SCT Cluster Fit tools and algorithms
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg

def ClusterFitAlgBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for Tracker ClusterFitAlg"""
    acc = FaserSCT_GeometryCfg( flags )
    # clusterTool = acc.popToolsAndMerge(FaserSCT_ClusterizationToolCfg(flags))
    # kwargs.setdefault("SCT_ClusteringTool", clusterTool)
    # kwargs.setdefault("DataObjectName", "SCT_RDOs")
    kwargs.setdefault("ClustersName", "SCT_ClusterContainer")
    # kwargs.setdefault("SCT_FlaggedCondData", "SCT_Flags")
    Tracker__ClusterFitAlg=CompFactory.Tracker.ClusterFitAlg
    acc.addEventAlgo(Tracker__ClusterFitAlg(**kwargs))
    
    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += ["HIST DATAFILE='ClusterFitHistograms.root' OPT='RECREATE'"]
    acc.addService(thistSvc)
    return acc

# with output defaults
def ClusterFitAlgCfg(flags, **kwargs):
    """Return ComponentAccumulator for Tracker ClusterFitAlg and Output"""
    acc = ClusterFitAlgBasicCfg(flags, **kwargs)
    # acc.merge(ClusterFitAlgOutputCfg(flags))
    return acc
