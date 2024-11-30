"""Define methods to construct configured SCT Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
Tracker__TruthSeededTrackFinder, THistSvc=CompFactory.getComps("Tracker::TruthSeededTrackFinder", "THistSvc")
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg



def TruthSeededTrackFinderBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for TruthSeededTrackFinder"""
    acc = FaserSCT_GeometryCfg(flags)
    kwargs.setdefault("SpacePointsSCTName", "SCT_SpacePointContainer")
    kwargs.setdefault("FaserSpacePointsSeedsName", "Seeds_SpacePointContainer")
    acc.addEventAlgo(Tracker__TruthSeededTrackFinder(**kwargs))
   # attach ToolHandles
    return acc

def TruthSeededTrackFinder_OutputCfg(flags):
    """Return ComponentAccumulator with Output for SCT. Not standalone."""
    acc = ComponentAccumulator()
    acc.merge(OutputStreamCfg(flags, "ESD"))
    ostream = acc.getEventAlgo("OutputStreamESD")
    ostream.TakeItemsFromInput = True
    return acc

def TruthSeededTrackFinderCfg(flags, **kwargs):
    acc=TruthSeededTrackFinderBasicCfg(flags, **kwargs)
    histSvc= THistSvc()
    histSvc.Output += [ "TruthTrackSeeds DATAFILE='truthtrackseeds.root' OPT='RECREATE'" ]
    acc.addService(histSvc)
    acc.merge(TruthSeededTrackFinder_OutputCfg(flags))
    return acc
