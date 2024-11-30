# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ISF_Algorithms.collection_merger_helpersNew import CollectionMergerCfg

TriggerSensorSDTool=CompFactory.TriggerSensorSDTool

def getTriggerSensorSD(ConfigFlags, name="TriggerSensorSD", **kwargs):

    result = ComponentAccumulator()
    bare_collection_name = "TriggerHits"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "TriggerHits"

    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name, mergeable_collection_suffix, merger_input_property)
    kwargs.setdefault("LogicalVolumeNames", ["Trigger::Plate"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    result.merge(acc)
    return result, TriggerSensorSDTool(name, **kwargs)

