# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders
from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg

FaserSCT_ConfigurationCondAlg = CompFactory.FaserSCT_ConfigurationCondAlg
FaserSCT_ConfigurationConditionsTool = CompFactory.FaserSCT_ConfigurationConditionsTool

def FaserSCT_ConfigurationCondAlgCfg(flags, name="FaserSCT_ConfigurationCondAlg", **kwargs):
    acc = ComponentAccumulator()
    dbInstance = kwargs.get("dbInstance", "INDET_OFL")
    dbFolder = kwargs.get("dbFolder", "/SCT/DAQ/NoisyStrips")
    #dbFolder = kwargs.get("dbFolder", "/INDET/NoisyStrips")
    acc.merge(addFolders(flags, dbFolder, dbInstance, className="CondAttrListCollection"))
    acc.addCondAlgo(FaserSCT_ConfigurationCondAlg(name, **kwargs))
    return acc


def FaserSCT_ConfigurationConditionsToolCfg(flags, name="FaserSCT_ConfigurationCondAlg", **kwargs):
    acc = ComponentAccumulator()
    acc.merge(FaserSCT_ConfigurationCondAlgCfg(flags, name="FaserSCT_ConfigurationCondAlg", **kwargs))
    acc.setPrivateTools(FaserSCT_ConfigurationConditionsTool(name, **kwargs))
    return acc


def FaserSCT_ConditionsSummaryToolCfg(flags, name="FaserSCT_ConditionsSummaryTool", **kwargs):
    acc = ComponentAccumulator()
    ConditionsTools = []
    SCT_ConfigurationConditionsTool = acc.popToolsAndMerge(FaserSCT_ConfigurationConditionsToolCfg(flags))
    ConditionsTools += [ SCT_ConfigurationConditionsTool ]
    kwargs.setdefault("ConditionsTools", ConditionsTools)
    acc.setPrivateTools(CompFactory.FaserSCT_ConditionsSummaryTool(name=name, **kwargs))
    return acc
