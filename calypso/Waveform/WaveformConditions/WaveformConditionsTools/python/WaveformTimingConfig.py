""" Define methods to configure WaveformTimingTool

Copyright (C) 2022 CERN for the benefit of the FASER collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders
WaveformTimingTool=CompFactory.WaveformTimingTool

def WaveformTimingToolCfg(flags, name="WaveformTimingTool", **kwargs):
    """ Return a configured WaveformTimingTool"""
    return WaveformTimingTool(name, **kwargs)

def WaveformTimingCfg(flags, **kwargs):
    """ Return configured ComponentAccumulator and tool for Waveform Timing 

    WaveformTimingTool may be provided in kwargs
    """

    acc = ComponentAccumulator()
    # tool = kwargs.get("WaveformTimingTool", WaveformTimingTool(flags))
    # Probably need to figure this out!
    dbInstance = kwargs.get("dbInstance", "TRIGGER_OFL")
    # This is now set up globally, don't need this here
    #if flags.Input.isMC:
    #    dbname = "OFLP200"
    #else:
    #    dbname = "CONDBR3"

    #acc.merge(addFolders(flags, "/WAVE/DAQ/Timing", dbInstance, className="AthenaAttributeList", db=dbname))
    #acc.merge(addFolders(flags, "/WAVE/DAQ/TimingOffset", dbInstance, className="CondAttrListCollection", db=dbname))
    acc.merge(addFolders(flags, "/WAVE/DAQ/Timing", dbInstance, className="AthenaAttributeList"))
    acc.merge(addFolders(flags, "/WAVE/DAQ/TimingOffset", dbInstance, className="CondAttrListCollection"))
    return acc

