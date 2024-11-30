"""Define methods to configure SCT_DCSConditions

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders
FaserSCT_DCSConditionsTool=CompFactory.FaserSCT_DCSConditionsTool
FaserSCT_DCSConditionsStatCondAlg=CompFactory.FaserSCT_DCSConditionsStatCondAlg
FaserSCT_DCSConditionsHVCondAlg=CompFactory.FaserSCT_DCSConditionsHVCondAlg
FaserSCT_DCSConditionsTempCondAlg=CompFactory.FaserSCT_DCSConditionsTempCondAlg

def FaserSCT_DCSConditionsToolCfg(flags, name="TrackerSCT_DCSConditionsTool", **kwargs):
    """Return a configured FaserSCT_DCSConditionsTool"""
    kwargs.setdefault("ReadAllDBFolders", True)
    kwargs.setdefault("ReturnHVTemp", True)
    return FaserSCT_DCSConditionsTool(name, **kwargs)

def FaserSCT_DCSConditionsCfg(flags, name="TrackerSCT_DCSConditions", **kwargs):
    """Return configured ComponentAccumulator and tool for FaserSCT_DCSConditions

    DCSConditionsTool may be provided in kwargs
    """
    acc = ComponentAccumulator()
    tool = kwargs.get("DCSConditionsTool", FaserSCT_DCSConditionsToolCfg(flags))
    # folder arguments
    dbInstance = kwargs.get("dbInstance", "DCS_OFL")
    hvFolder = kwargs.get("hvFolder", "/SCT/DCS/HV")
    tempFolder = kwargs.get("tempFolder", "/SCT/DCS/MODTEMP")
    stateFolder = kwargs.get("stateFolder", "/SCT/DCS/CHANSTAT")
    if tool.ReadAllDBFolders == tool.ReturnHVTemp:
        acc.merge(addFolders(flags, stateFolder, dbInstance, className="CondAttrListCollection",db="OFLP200"))
        # algo
        statArgs = {
            "name": name + "StatCondAlg",
            "ReturnHVTemp": tool.ReturnHVTemp,
            "ReadKeyHV": hvFolder,
            "ReadKeyState": stateFolder,
        }
        statAlg = FaserSCT_DCSConditionsStatCondAlg(**statArgs)
        acc.addCondAlgo(statAlg)
    if tool.ReturnHVTemp:
        acc.merge(addFolders(flags, [hvFolder, tempFolder], dbInstance, className="CondAttrListCollection",db="OFLP200"))
        hvAlg = FaserSCT_DCSConditionsHVCondAlg(name=name + "HVCondAlg", ReadKey=hvFolder)
        acc.addCondAlgo(hvAlg)
        tempAlg = FaserSCT_DCSConditionsTempCondAlg(name=name + "TempCondAlg", ReadKey=tempFolder)
        acc.addCondAlgo(tempAlg)
    acc.setPrivateTools(tool)
    return acc

