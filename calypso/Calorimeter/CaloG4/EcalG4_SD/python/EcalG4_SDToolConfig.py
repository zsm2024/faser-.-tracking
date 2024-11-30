# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

EcalSensorSDTool=CompFactory.EcalSensorSDTool

def EcalSensorSDCfg(ConfigFlags, name="EcalSensorSD", **kwargs):

    result = ComponentAccumulator()
    bare_collection_name = "EcalHits"
    # mergeable_collection_suffix = "_G4"
    # merger_input_property = "PreshowerHits"

    # acc, hits_collection_name = CollectionMergerCfg(ConfigFlags, bare_collection_name, mergeable_collection_suffix, merger_input_property, "SCINT")
    # kwargs.setdefault("OutputCollectionNames", [hits_collection_name])
    kwargs.setdefault("LogicalVolumeNames", ["Ecal::_dd_Geometry_DownstreamRegion_Ecal_Modules_OutCell"])
    kwargs.setdefault("OutputCollectionNames", [bare_collection_name])

    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.EcalSensorSDTool(name, **kwargs))
    return result

