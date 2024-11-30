"""Define methods to configure FaserSCT_CableMapping

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders
FaserSCT_CableMappingTool=CompFactory.FaserSCT_CableMappingTool

def FaserSCT_CableMappingToolCfg(flags, name="CableMappingTool", **kwargs):
    """Return a configured FaserSCT_CableMappingTool"""
    return FaserSCT_CableMappingTool(name, **kwargs)

def FaserSCT_CableMappingCfg(flags, **kwargs):
    """Return configured ComponentAccumulator and tool for FaserSCT_CableMapping

    CableMappingTool may be provided in kwargs
    """
    acc = ComponentAccumulator()
    # tool = kwargs.get("CableMappingTool", FaserSCT_CableMappingToolCfg(flags))
    # folder arguments
    dbInstance = kwargs.get("dbInstance", "TDAQ_OFL")
    dbFolder = kwargs.get("dbFolder", "/SCT/DAQ/CableMapping")
    acc.merge(addFolders(flags, dbFolder, dbInstance, className="CondAttrListCollection"))
    # acc.addPublicTool(tool)
    return acc

