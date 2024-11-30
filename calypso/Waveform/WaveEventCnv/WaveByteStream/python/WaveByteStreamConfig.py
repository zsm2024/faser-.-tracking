# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from WaveformConditionsTools.WaveformCableMappingConfig import WaveformCableMappingCfg
from WaveformConditionsTools.WaveformRangeConfig import WaveformRangeCfg

def WaveByteStreamCfg(configFlags, **kwargs):
    acc = ComponentAccumulator()
    acc.merge(WaveformCableMappingCfg(configFlags, **kwargs))
    acc.merge(WaveformRangeCfg(configFlags, **kwargs))
    return acc
        
