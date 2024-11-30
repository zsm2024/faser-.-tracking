# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
# from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

FaserSctSensorSDTool=CompFactory.FaserSctSensorSDTool

def SctSensorSDCfg(ConfigFlags, name="SctSensorSD", **kwargs):

    result = ComponentAccumulator()
    bare_collection_name = "SCT_Hits"
    # mergeable_collection_suffix = "_G4"
    # merger_input_property = "SCTHits"

    # acc, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name, mergeable_collection_suffix, merger_input_property, "TRACKER")
    # kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    kwargs.setdefault("LogicalVolumeNames", ["SCT::BRLSensor"])
    kwargs.setdefault("OutputCollectionNames", [bare_collection_name])

    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.FaserSctSensorSDTool(name, **kwargs))
    return result

