# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

PreshowerSensorSDTool=CompFactory.PreshowerSensorSDTool

def getPreshowerSensorSD(ConfigFlags, name="PreshowerSensorSD", **kwargs):

    result = ComponentAccumulator()
    bare_collection_name = "PreshowerHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "PreshowerHits"

    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name, mergeable_collection_suffix, merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Preshower::Plate"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.merge(acc)
    return result, PreshowerSensorSDTool(name, **kwargs)

