# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

EmulsionSensorSDTool=CompFactory.EmulsionSensorSDTool

def EmulsionSensorSDCfg(ConfigFlags, name="EmulsionSensorSD", **kwargs):

    result = ComponentAccumulator()
    bare_collection_name = "EmulsionHits"
    # mergeable_collection_suffix = "_G4"
    # merger_input_property = "PreshowerHits"

    # acc, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name, mergeable_collection_suffix, merger_input_property, "SCINT")
    # kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    kwargs.setdefault("LogicalVolumeNames", ["Emulsion::FrontFilm", "Emulsion::BackFilm"])
    kwargs.setdefault("OutputCollectionNames", [bare_collection_name])

    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.EmulsionSensorSDTool(name, **kwargs))
    return result

