"""Define methods to configure FaserSCT_NoisyStrips

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders
FaserSCT_NoisyStripsTool=CompFactory.FaserSCT_NoisyStripTool

def FaserSCT_NoisyStripsToolCfg(flags, name="NoisyStripsTool", **kwargs):
    """Return a configured FaserSCT_NoisyStripsTool"""
    return FaserSCT_NoisyStripsTool(name, **kwargs)

def FaserSCT_NoisyStripsCfg(flags, **kwargs):
    """Return configured ComponentAccumulator and tool for FaserSCT_NoisyStrips
    NoisyStripsTool may be provided in kwargs
    """
    acc = ComponentAccumulator()
    dbInstance = kwargs.get("dbInstance", "INDET_OFL")
    dbFolder = kwargs.get("dbFolder", "/SCT/DAQ/NoisyStrips")
    acc.merge(addFolders(flags, dbFolder, dbInstance, className="CondAttrListCollection"))
    # acc.addPublicTool(tool)
    return acc
