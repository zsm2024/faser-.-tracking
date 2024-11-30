# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
# from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

VetoNuSensorSDTool=CompFactory.VetoNuSensorSDTool

def VetoNuSensorSDCfg(ConfigFlags, name="VetoNuSensorSD", **kwargs):

    result = ComponentAccumulator()
    bare_collection_name = "VetoNuHits"
    # mergeable_collection_suffix = "_G4"
    # merger_input_property = "VetoNuHits"

    # acc, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name, mergeable_collection_suffix, merger_input_property, 'SCINT')
    # kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    kwargs.setdefault("LogicalVolumeNames", ["VetoNu::Plate"])
    kwargs.setdefault("OutputCollectionNames", [bare_collection_name])

    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.VetoNuSensorSDTool(name, **kwargs))
    return result

