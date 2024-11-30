"""Define methods to construct configured SCT Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
PileUpXingFolder=CompFactory.PileUpXingFolder

from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg
from FaserSiLorentzAngleTool.FaserSCT_LorentzAngleConfig import FaserSCT_LorentzAngleCfg
from FaserSCT_ConditionsTools.FaserSCT_ConditionsSummaryToolConfig import FaserSCT_ConditionsSummaryToolCfg


def FaserSCT_ClusterizationCommonCfg(flags, name="FaserSCT_ClusterizationToolCommon", **kwargs):
    """Return ComponentAccumulator with common FaserSCT Clusterization tool config"""
    acc = FaserSCT_GeometryCfg(flags)

    Tracker__TrackerClusterMakerTool,Tracker__FaserSCT_ClusteringTool=CompFactory.getComps("Tracker::TrackerClusterMakerTool", "Tracker::FaserSCT_ClusteringTool")
    trackerClusterMakerTool = Tracker__TrackerClusterMakerTool(name = "TrackerClusterMakerTool")
    faserSCT_LorentzAngleTool=acc.popToolsAndMerge(FaserSCT_LorentzAngleCfg(flags))
    kwargs.setdefault("timeBins", "01X")

    FaserSCT_ConditionsSummaryTool = acc.popToolsAndMerge(FaserSCT_ConditionsSummaryToolCfg(flags))
    clusteringTool = Tracker__FaserSCT_ClusteringTool(name,
                                                      globalPosAlg=trackerClusterMakerTool,
                                                      FaserSiLorentzAngleTool=faserSCT_LorentzAngleTool,
                                                      conditionsTool=FaserSCT_ConditionsSummaryTool,
                                                      **kwargs)
    # clusteringTool.timeBins = "01X" 
    # attach ToolHandles
    acc.setPrivateTools(clusteringTool)
    return acc 


def FaserSCT_ClusterizationToolCfg(flags, name="FaserSCT_ClusterizationTool", **kwargs):
    """Return ComponentAccumulator with configured FaserSCT Clusterization tool"""
    #kwargs.setdefault("HardScatterSplittingMode", 0)
    return FaserSCT_ClusterizationCommonCfg(flags, name, **kwargs)


def FaserSCT_ClusterizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for FaserSCT Clusterization"""
    acc = ComponentAccumulator()
    pattern = kwargs.pop("ClusterToolTimingPattern", "")
    checkBadChannels = kwargs.pop("checkBadChannels", "False")
    # print("ClusterToolTimingPattern = ", pattern)
    if len(pattern) > 0 :
        clusterTool = acc.popToolsAndMerge(FaserSCT_ClusterizationToolCfg(flags, timeBins=pattern, checkBadChannels=checkBadChannels))
    else:
        clusterTool = acc.popToolsAndMerge(FaserSCT_ClusterizationToolCfg(flags, checkBadChannels=checkBadChannels))

    FaserSCT_ConditionsSummaryTool = acc.popToolsAndMerge(FaserSCT_ConditionsSummaryToolCfg(flags))

    kwargs.setdefault("SCT_ClusteringTool", clusterTool)
    kwargs.setdefault("DataObjectName", "SCT_RDOs")
    kwargs.setdefault("ClustersName", "SCT_ClusterContainer")
    kwargs.setdefault("conditionsTool", FaserSCT_ConditionsSummaryTool)
    # kwargs.setdefault("SCT_FlaggedCondData", "SCT_Flags")
    Tracker__FaserSCT_Clusterization=CompFactory.Tracker.FaserSCT_Clusterization
    acc.addEventAlgo(Tracker__FaserSCT_Clusterization(**kwargs))
    return acc

def FaserSCT_OutputCfg(flags):                                                                                                    
    """Return ComponentAccumulator with Output for SCT. Not standalone."""
    acc = ComponentAccumulator()
    ItemList = ["Tracker::FaserSCT_ClusterContainer#*"]
    acc.merge(OutputStreamCfg(flags, "ESD", ItemList))
    ostream = acc.getEventAlgo("OutputStreamESD")
    ostream.TakeItemsFromInput = True
    return acc


# with output defaults
def FaserSCT_ClusterizationCfg(flags, **kwargs):
    """Return ComponentAccumulator for SCT Clusterization and Output"""
    acc = FaserSCT_ClusterizationBasicCfg(flags, **kwargs)
    if flags.Output.doWriteESD:
        acc.merge(FaserSCT_OutputCfg(flags))
    return acc
