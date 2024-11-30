# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon import Logging
import os

def Pythia8DecayerPhysicsToolCfg(flags, name='Pythia8DecayerPhysicsTool', **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools( CompFactory.Pythia8DecayerPhysicsTool(name,**kwargs) )
    return result
