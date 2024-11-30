"""
Copyright (C) 2022 CERN for the benefit of the FASER collaboration
"""

from AthenaConfiguration.ComponentFactory import CompFactory
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg


def MyClustersCfg(flags, **kwargs):
    acc = FaserSCT_GeometryCfg(flags)
    my_clusters_alg = CompFactory.MyClustersAlg
    acc.addEventAlgo(my_clusters_alg (**kwargs))

    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += ["HIST DATAFILE='MyClusters.root' OPT='RECREATE'"]
    acc.addService(thistSvc)
    return acc
