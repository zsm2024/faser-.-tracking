"""Define methods to configure ReadCalibChipDataTool

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
FaserSCT_ReadCalibChipDataTool=CompFactory.FaserSCT_ReadCalibChipDataTool
FaserSCT_ReadCalibChipNoiseCondAlg=CompFactory.FaserSCT_ReadCalibChipNoiseCondAlg
FaserSCT_ReadCalibChipGainCondAlg=CompFactory.FaserSCT_ReadCalibChipGainCondAlg

def FaserSCT_ReadCalibChipDataToolCfg(flags, name="TrackerSCT_ReadCalibChipDataTool", **kwargs):
    """Return a ReadCalibChipDataTool configured for FaserSCT"""
    return FaserSCT_ReadCalibChipDataTool(name, **kwargs)

def FaserSCT_ReadCalibChipDataCfg(flags, name="FaserSCT_ReadCalibChip", **kwargs):
    """Return configured ComponentAccumulator with FaserSCT_ReadCalibChipDataCfg tool

    Accepts optional noiseFolder and gainFolder keyword arguments
    """
    acc = ComponentAccumulator()
    # folders
    noiseFolder = kwargs.get("noiseFolder", "/SCT/DAQ/Calibration/ChipNoise")
    acc.merge(addFoldersSplitOnline(flags, "SCT", noiseFolder, noiseFolder, "CondAttrListCollection"))
    gainFolder = kwargs.get("gainFolder", "/SCT/DAQ/Calibration/ChipGain")
    acc.merge(addFoldersSplitOnline(flags, "SCT", gainFolder, gainFolder, "CondAttrListCollection"))
    # Algorithms
    noiseAlg = FaserSCT_ReadCalibChipNoiseCondAlg(name=name + "NoiseCondAlg", ReadKey=noiseFolder)
    acc.addCondAlgo(noiseAlg)
    gainAlg = FaserSCT_ReadCalibChipGainCondAlg(name=name + "GainCondAlg", ReadKey=gainFolder)
    acc.addCondAlgo(gainAlg)
    tool = kwargs.get("ReadCalibChipDataTool", FaserSCT_ReadCalibChipDataToolCfg(flags))
    acc.setPrivateTools(tool)
    return acc

