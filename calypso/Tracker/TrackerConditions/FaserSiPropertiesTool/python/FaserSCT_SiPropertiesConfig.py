"""Define methods to configure SCT SiProperties

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
FaserSiPropertiesTool=CompFactory.FaserSiPropertiesTool
FaserSCT_SiPropertiesCondAlg=CompFactory.FaserSCT_SiPropertiesCondAlg

def FaserSCT_SiPropertiesToolCfg(flags, name="FaserSCT_SiPropertiesTool", **kwargs):
    """Return a SiPropertiesTool configured for SCT"""
    kwargs.setdefault("DetectorName", "SCT")
    kwargs.setdefault("ReadKey", "SCTSiliconPropertiesVector")
    return FaserSiPropertiesTool(name=name, **kwargs)

def FaserSCT_SiPropertiesCfg(flags, name="FaserSCT_SiPropertiesCondAlg", **kwargs):
    """Return configured ComponentAccumulator and tool for SCT_SiProperties

    SiConditionsTool and/or SiPropertiesTool may be provided in kwargs
    """
    acc = ComponentAccumulator()
    tool = kwargs.get("SiPropertiesTool", FaserSCT_SiPropertiesToolCfg(flags))
    alg = FaserSCT_SiPropertiesCondAlg(name, **kwargs)
    acc.addCondAlgo(alg)
    acc.setPrivateTools(tool)
    return acc

