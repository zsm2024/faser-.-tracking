"""Define methods to construct configured SCT Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
PileUpXingFolder, Tracker__TrackerSpacePointFinder, Tracker__StatisticsAlg, TrackerDD__SiElementPropertiesTableCondAlg, Tracker__TrackerSpacePointMakerTool = CompFactory.getComps("PileUpXingFolder", "Tracker::TrackerSpacePointFinder", "Tracker::StatisticsAlg", "TrackerDD::SiElementPropertiesTableCondAlg", "Tracker::TrackerSpacePointMakerTool")

from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg

def TrackerDDSiElementPropertiesTableCondAlgCfg(flags, **kwargs):
  acc = ComponentAccumulator()
  acc.addCondAlgo(TrackerDD__SiElementPropertiesTableCondAlg(name = "TrackerDDSiElementPropertiesTableCondAlg", **kwargs))
  return acc


def TrackerSpacePointMakerCommonCfg(flags, name="TrackerSpacePointMakerTool", **kwargs):
    """Return ComponentAccumulator with common FaserSCT Clusterization tool config"""
    acc = FaserSCT_GeometryCfg(flags)

    kwargs.setdefault("InputObjectName", "SCT_ClusterContainer")
    trackerSpacePointMakerTool = Tracker__TrackerSpacePointMakerTool(name)
    # attach ToolHandles
    acc.setPrivateTools(trackerSpacePointMakerTool)
    return acc


def TrackerSpacePointMakerToolCfg(flags, name="TrackerSpacePointMakerTool", **kwargs):
    """Return ComponentAccumulator with configured TrackerSpacePointMakerTool"""
    #kwargs.setdefault("HardScatterSplittingMode", 0)
    return TrackerSpacePointMakerCommonCfg(flags, name, **kwargs)


def TrackerSpacePointFinderBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for TrackerSpacePointFinder"""
    acc = ComponentAccumulator()
    finderTool = acc.popToolsAndMerge(TrackerSpacePointMakerToolCfg(flags))
    kwargs.setdefault("SpacePointFinderTool", finderTool)
    kwargs.setdefault("SCT_ClustersName", "SCT_ClusterContainer")
    kwargs.setdefault("SpacePointsSCTName", "SCT_SpacePointContainer")
    kwargs.setdefault("SpacePointsOverlapName", "SCT_SpacePointOverlapCollection")
    # kwargs.setdefault("SCT_FlaggedCondData", "SCT_Flags");
    acc.addEventAlgo(Tracker__TrackerSpacePointFinder(**kwargs))
    return acc

def StatisticsBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for TrackerSpacePointMakerAlg"""
    acc = ComponentAccumulator()
    acc.popToolsAndMerge(TrackerSpacePointMakerToolCfg(flags))
    kwargs.setdefault("SpacePointsSCTName", "SCT_SpacePointContainer")
    kwargs.setdefault("SCT_ClustersName", "SCT_ClusterContainer")
    acc.addEventAlgo(Tracker__StatisticsAlg(**kwargs))
    return acc

def TrackerSpacePointFinder_OutputCfg(flags):                                                                                                    
    """Return ComponentAccumulator with Output for SCT. Not standalone."""
    acc = ComponentAccumulator()
    itemList = [ "FaserSCT_SpacePointContainer#*"]
    acc.merge(OutputStreamCfg(flags, "ESD", itemList))
    ostream = acc.getEventAlgo("OutputStreamESD")
    ostream.TakeItemsFromInput = True
    return acc

# with output defaults
def TrackerSpacePointFinderCfg(flags, **kwargs):
    """Return ComponentAccumulator for SCT SpacePoints and Output"""
    acc=TrackerDDSiElementPropertiesTableCondAlgCfg(flags)
    acc.merge(TrackerSpacePointFinderBasicCfg(flags, **kwargs))
    if flags.Output.doWriteESD:
       acc.merge(TrackerSpacePointFinder_OutputCfg(flags))
    return acc

def StatisticsCfg(flags, **kwargs):
    acc=StatisticsBasicCfg(flags, **kwargs)
    THistSvc=CompFactory.THistSvc
    histSvc= THistSvc()
    histSvc.Output += [ "StatisticsAlg DATAFILE='statistics.root' OPT='RECREATE'" ]
    acc.addService(histSvc)
    acc.merge(TrackerSpacePointFinder_OutputCfg(flags))
    return acc
