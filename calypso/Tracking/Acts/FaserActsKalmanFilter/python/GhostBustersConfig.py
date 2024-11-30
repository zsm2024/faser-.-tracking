"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg
from FaserActsGeometry.ActsGeometryConfig import ActsTrackingGeometrySvcCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg


def GhostBusters_OutputCfg(flags):
    acc = ComponentAccumulator()
    itemList = ["xAOD::EventInfo#*",
                "xAOD::EventAuxInfo#*",
                "TrackCollection#*",
                ]
    acc.merge(OutputStreamCfg(flags, "ESD", itemList))
    ostream = acc.getEventAlgo("OutputStreamESD")
    ostream.TakeItemsFromInput = True
    return acc


def GhostBustersCfg(flags, **kwargs):
    acc = FaserSCT_GeometryCfg(flags)
    acts_tracking_geometry_svc = ActsTrackingGeometrySvcCfg(flags)
    acc.merge(acts_tracking_geometry_svc )
    acc.addEventAlgo(CompFactory.GhostBusters(**kwargs))
    if flags.Output.doWriteESD:
       acc.merge(GhostBusters_OutputCfg(flags))

    # thistSvc = CompFactory.THistSvc()
    # thistSvc.Output += ["HIST2 DATAFILE='GhostBusters.root' OPT='RECREATE'"]
    # acc.addService(thistSvc)

    return acc
