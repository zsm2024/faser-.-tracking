"""Define methods to construct configured SCT Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

Tracker__TrackerSeedFinder, THistSvc=CompFactory.getComps("Tracker::TrackerSeedFinder", "THistSvc")

from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg



def TrackerSeedFinderBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for TrackerSeedFinder"""
    acc = ComponentAccumulator()
    kwargs.setdefault("SpacePointsSCTName", "SCT_SpacePointContainer")
    kwargs.setdefault("FaserSpacePointsSeedsName", "Seeds_SpacePointContainer")
    acc.addEventAlgo(Tracker__TrackerSeedFinder(**kwargs))
   # attach ToolHandles
    return acc

def TrackerSeedFinder_OutputCfg(flags):
    """Return ComponentAccumulator with Output for SCT. Not standalone."""
    acc = ComponentAccumulator()
    acc.merge(OutputStreamCfg(flags, "ESD"))
    ostream = acc.getEventAlgo("OutputStreamESD")
    ostream.TakeItemsFromInput = True
    return acc

def TrackerSeedFinderCfg(flags, **kwargs):
    acc=TrackerSeedFinderBasicCfg(flags, **kwargs)
    histSvc= THistSvc()
    histSvc.Output += [ "TrackerSeedFinder DATAFILE='trackseedfinder.root' OPT='RECREATE'" ]
    acc.addService(histSvc)
    acc.merge(TrackerSeedFinder_OutputCfg(flags))
    return acc
