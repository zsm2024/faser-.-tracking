# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from LHCDataTools.LHCDataToolsConfig import LHCDataCfg

#LHCDataTool = CompFactory.LHCDataTool

def LHCDataAlgCfg(flags, name="LHCDataAlg", **kwargs):

    acc = ComponentAccumulator()

    tool = CompFactory.LHCDataTool(name="LHCDataTool")
    alg = CompFactory.LHCDataAlg(name, **kwargs)
    alg.LHCDataTool = tool

    acc.addEventAlgo(alg)
    acc.merge(LHCDataCfg(flags))

    return acc
