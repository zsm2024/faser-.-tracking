""" Define methods to configure WaveformCableMapping

Copyright (C) 2022 CERN for the benefit of the FASER collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders

def WaveformDigitizationCfg(flags, **kwargs):
    """ Return configured ComponentAccumulator for Waveform Dititization 

    """

    acc = ComponentAccumulator()
    # tool = kwargs.get("WaveformCableMappingTool", WaveformCableMappingTool(flags))
    # Probably need to figure this out!
    dbInstance = kwargs.get("dbInstance", "TRIGGER_OFL")
    dbFolder = kwargs.get("dbFolder", "/WAVE/Digitization")
    caloDigiTag = kwargs.get("digiTag", "")

    if len(caloDigiTag) > 0:
        acc.merge(addFolders(flags, dbFolder, dbInstance, className="CondAttrListCollection", tag=caloDigiTag))
    else:
        acc.merge(addFolders(flags, dbFolder, dbInstance, className="CondAttrListCollection"))

    return acc

