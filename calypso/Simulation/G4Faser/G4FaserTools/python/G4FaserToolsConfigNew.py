# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon import CfgMgr
SensitiveDetectorMasterTool=CompFactory.SensitiveDetectorMasterTool
FastSimulationMasterTool=CompFactory.FastSimulationMasterTool

def generateFastSimulationListCfg(ConfigFlags):
    result = ComponentAccumulator()
    FastSimulationList = []
    result.setPrivateTools(FastSimulationList)
    return result

def FastSimulationMasterToolCfg(ConfigFlags, **kwargs):
    result = ComponentAccumulator()
    FastSimulationList = result.popToolsAndMerge(generateFastSimulationListCfg(ConfigFlags))
    kwargs.setdefault("FastSimulations", FastSimulationList)
    result.setPrivateTools(FastSimulationMasterTool(name="FastSimulationMasterTool", **kwargs))
    return result

def NeutrinoSensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []

    if ConfigFlags.Detector.EnableEmulsion:
        from EmulsionG4_SD.EmulsionG4_SDToolConfig import EmulsionSensorSDCfg
        tools += [ result.popToolsAndMerge(EmulsionSensorSDCfg(ConfigFlags)) ]

    result.setPrivateTools(tools)
    return result


def CaloSensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []

    if ConfigFlags.Detector.EnableEcal:
        from EcalG4_SD.EcalG4_SDToolConfig import EcalSensorSDCfg
        tools += [ result.popToolsAndMerge(EcalSensorSDCfg(ConfigFlags)) ]

    result.setPrivateTools(tools)
    return result

def ScintSensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []

    if ConfigFlags.Detector.EnableVeto:
        from VetoG4_SD.VetoG4_SDToolConfig import VetoSensorSDCfg
        tools += [ result.popToolsAndMerge(VetoSensorSDCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.EnableVetoNu:
        from VetoNuG4_SD.VetoNuG4_SDToolConfig import VetoNuSensorSDCfg
        tools += [ result.popToolsAndMerge(VetoNuSensorSDCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.EnableTrigger:
        from TriggerG4_SD.TriggerG4_SDToolConfig import TriggerSensorSDCfg
        tools += [ result.popToolsAndMerge(TriggerSensorSDCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.EnablePreshower:
        from PreshowerG4_SD.PreshowerG4_SDToolConfig import PreshowerSensorSDCfg
        tools += [ result.popToolsAndMerge(PreshowerSensorSDCfg(ConfigFlags)) ]

    result.setPrivateTools(tools)
    return result

def TrackerSensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []

    if ConfigFlags.Detector.EnableFaserSCT:
        from FaserSCT_G4_SD.FaserSCT_G4_SDToolConfig import SctSensorSDCfg
        tools += [ result.popToolsAndMerge(SctSensorSDCfg(ConfigFlags)) ]

    result.setPrivateTools(tools)
    return result

def SensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []

    tools += result.popToolsAndMerge(CaloSensitiveDetectorListCfg(ConfigFlags))
    tools += result.popToolsAndMerge(ScintSensitiveDetectorListCfg(ConfigFlags))
    tools += result.popToolsAndMerge(TrackerSensitiveDetectorListCfg(ConfigFlags))
    tools += result.popToolsAndMerge(NeutrinoSensitiveDetectorListCfg(ConfigFlags))

    result.setPrivateTools(tools)
    return result

def SensitiveDetectorMasterToolCfg(ConfigFlags, name="SensitiveDetectorMasterTool", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("SensitiveDetectors", result.popToolsAndMerge(SensitiveDetectorListCfg(ConfigFlags)))

    result.setPrivateTools(CompFactory.SensitiveDetectorMasterTool(name, **kwargs))
    return result

def getEmptySensitiveDetectorMasterTool(name="EmptySensitiveDetectorMasterTool", **kwargs):
    result = ComponentAccumulator()
    tool = result.popToolsAndMerge(CompFactory.SensitiveDetectorMasterTool(name, **kwargs))
    result.setPrivateTools(tool)
    return result
