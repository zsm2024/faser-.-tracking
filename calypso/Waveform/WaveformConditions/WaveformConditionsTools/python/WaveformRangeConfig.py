""" Define methods to configure WaveformRangeTool

Copyright (C) 2022 CERN for the benefit of the FASER collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders
WaveformRangeTool=CompFactory.WaveformRangeTool

def WaveformRangeToolCfg(flags, name="WaveformRangeTool", **kwargs):
    """ Return a configured WaveformRangeTool"""
    return WaveformRangeTool(name, **kwargs)

def WaveformRangeCfg(flags, **kwargs):
    """ Return configured ComponentAccumulator and tool for Waveform Range 

    WaveformRangeTool may be provided in kwargs
    """

    acc = ComponentAccumulator()
    # tool = kwargs.get("WaveformRangeTool", WaveformRangeTool(flags))
    # Probably need to figure this out!

    dbInstance = kwargs.get("dbInstance", "COOLOFL_DIGITIZER")
    dbFolder = kwargs.get("dbFolder", "/WAVE/DAQ/Range")
    dbName = flags.IOVDb.DatabaseInstance # e.g. CONDBR3 

    # COOLOFL_DIGITIZER is not known to ATLAS IOVDBSvc
    # Must use non-shorthand folder specification here
    folder_string = f"<db>{dbInstance}/{dbName}</db> {dbFolder}"
    acc.merge(addFolders(flags, folder_string, className="CondAttrListCollection"))
    return acc

