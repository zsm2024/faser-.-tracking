"""Define methods to configure SiPropertiesTool

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
FaserSCT_SiliconHVCondAlg=CompFactory.FaserSCT_SiliconHVCondAlg
FaserSCT_SiliconTempCondAlg=CompFactory.FaserSCT_SiliconTempCondAlg
FaserSCT_SiliconConditionsTool=CompFactory.FaserSCT_SiliconConditionsTool

def FaserSCT_SiliconConditionsToolCfg(flags, name="FaserSCT_SiliconConditionsTool", **kwargs):
    """Return a configured FaserSCT_SiliconConditionsTool (a.k.a. SiConditionsTool)"""
    kwargs.setdefault("UseDB", True)
    kwargs.setdefault("ForceUseGeoModel", False)
    return FaserSCT_SiliconConditionsTool(name, **kwargs)

def FaserSCT_SiliconConditionsCfg(flags, name="FaserSCT_Silicon", **kwargs):
    """Return a ComponentAccumulator configured for SiliconConditions DB

    DCSConditionsTool may be provided in kwargs
    """
    acc = ComponentAccumulator()
    CondArgs = {}
    DCSConditionsTool = kwargs.get("DCSConditionsTool")
    if DCSConditionsTool:
        CondArgs["UseState"] = DCSConditionsTool.ReadAllDBFolders
        CondArgs["DCSConditionsTool"] = DCSConditionsTool
    acc.addCondAlgo(FaserSCT_SiliconHVCondAlg(name=name + "HVCondAlg", **CondArgs))
    acc.addCondAlgo(FaserSCT_SiliconTempCondAlg(name=name + "TempCondAlg", **CondArgs))
    return acc

