# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from FaserSCT_ConditionsTools.FaserSCT_CableMappingConfig import FaserSCT_CableMappingCfg
# from FaserSCT_ConditionsTools.FaserSCT_DCSConditionsConfig import FaserSCT_DCSConditionsCfg

def TrackerByteStreamCfg(configFlags, **kwargs):
    acc = FaserSCT_CableMappingCfg(configFlags, **kwargs)
    # tool = acc.popToolsAndMerge(acc)

    return acc