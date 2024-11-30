"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentFactory import CompFactory
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg



def TrackSeedPerformanceWriterCfg(flags, **kwargs):
    acc = FaserSCT_GeometryCfg(flags)
    kwargs.setdefault("TrackCollection", "SegmentFit")
    TrackSeedPerformanceWriter = CompFactory.Tracker.TrackSeedPerformanceWriter
    acc.addEventAlgo(TrackSeedPerformanceWriter(**kwargs))

    itemList = ["xAOD::EventInfo#*",
                "xAOD::EventAuxInfo#*",
                "xAOD::FaserTriggerData#*",
                "xAOD::FaserTriggerDataAux#*",
                "FaserSCT_RDO_Container#*",
                "Tracker::FaserSCT_ClusterContainer#*",
                "TrackCollection#*"
                ]
    acc.merge(OutputStreamCfg(flags, "ESD", itemList))

    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += ["HIST1 DATAFILE='TrackSeedPerformanceWriter.root' OPT='RECREATE'"]
    acc.addService(thistSvc)
    return acc
