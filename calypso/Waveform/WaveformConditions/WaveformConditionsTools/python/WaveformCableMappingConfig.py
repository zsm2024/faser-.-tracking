""" Define methods to configure WaveformCableMapping

Copyright (C) 2022 CERN for the benefit of the FASER collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders
WaveformCableMappingTool=CompFactory.WaveformCableMappingTool

def WaveformCableMappingToolCfg(flags, name="WaveformCableMappingTool", **kwargs):
    """ Return a configured WaveformCableMappingTool"""
    return WaveformCableMappingTool(name, **kwargs)

def WaveformCableMappingCfg(flags, **kwargs):
    """ Return configured ComponentAccumulator and tool for Waveform CableMapping 

    WaveformCableMappingTool may be provided in kwargs
    """

    acc = ComponentAccumulator()
    # tool = kwargs.get("WaveformCableMappingTool", WaveformCableMappingTool(flags))
    # Probably need to figure this out!
    dbInstance = kwargs.get("dbInstance", "TDAQ_OFL")
    dbFolder = kwargs.get("dbFolder", "/WAVE/DAQ/CableMapping")
    acc.merge(addFolders(flags, dbFolder, dbInstance, className="CondAttrListCollection"))
    return acc

