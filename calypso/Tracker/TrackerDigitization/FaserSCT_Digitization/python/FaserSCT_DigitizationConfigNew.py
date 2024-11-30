"""Define methods to construct configured SCT Digitization tools and algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging

PileUpXingFolder=CompFactory.PileUpXingFolder
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
#SCT_RadDamageSummaryTool=CompFactory.SCT_RadDamageSummaryTool
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg

from FaserSCT_ConditionsTools.FaserSCT_DCSConditionsConfig import FaserSCT_DCSConditionsCfg
from FaserSCT_ConditionsTools.FaserSCT_SiliconConditionsConfig import FaserSCT_SiliconConditionsToolCfg, FaserSCT_SiliconConditionsCfg
from FaserSCT_ConditionsTools.FaserSCT_ReadCalibChipDataConfig import FaserSCT_ReadCalibChipDataCfg
from FaserSiPropertiesTool.FaserSCT_SiPropertiesConfig import FaserSCT_SiPropertiesCfg
from FaserSiLorentzAngleTool.FaserSCT_LorentzAngleConfig import FaserSCT_LorentzAngleCfg
from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
from Digitization.PileUpToolsConfig import PileUpToolsCfg


# The earliest and last bunch crossing times for which interactions will be sent
# to the SCT Digitization code
def FaserSCT_FirstXing():
    return -50


def FaserSCT_LastXing():
    return 25


def FaserSCT_DigitizationCommonCfg(flags, name="FaserSCT_DigitizationToolCommon", **kwargs):
    """Return ComponentAccumulator with common FaserSCT digitization tool config"""
    acc = FaserSCT_GeometryCfg(flags)

    if not flags.Digitization.DoInnerDetectorNoise:
        kwargs.setdefault("OnlyHitElements", True)
    kwargs.setdefault("InputObjectName", "SCT_Hits")
    kwargs.setdefault("EnableHits", True)
    #kwargs.setdefault("BarrelOnly", False)
    # Set FixedTime for cosmics for use in SurfaceChargesGenerator
    if flags.Beam.Type == "cosmics":
        kwargs.setdefault("CosmicsRun", True)
        kwargs.setdefault("FixedTime", 10)
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", FaserSCT_FirstXing())
        kwargs.setdefault("LastXing", FaserSCT_LastXing() )
    FaserSCT_DigitizationTool=CompFactory.FaserSCT_DigitizationTool
    tool = FaserSCT_DigitizationTool(name, **kwargs)
    # attach ToolHandles
    tool.FrontEnd = acc.popToolsAndMerge(FaserSCT_FrontEndCfg(flags))
    tool.SurfaceChargesGenerator = acc.popToolsAndMerge(FaserSCT_SurfaceChargesGeneratorCfg(flags))
    tool.RandomDisabledCellGenerator = FaserSCT_RandomDisabledCellGeneratorCfg(flags)
    acc.setPrivateTools(tool)
    return acc


def FaserSCT_DigitizationToolCfg(flags, name="FaserSCT_DigitizationTool", **kwargs):
    """Return ComponentAccumulator with configured FaserSCT digitization tool"""
    kwargs.setdefault("OutputObjectName", "SCT_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_SDO_Map")
    #kwargs.setdefault("HardScatterSplittingMode", 0)
    return FaserSCT_DigitizationCommonCfg(flags, name, **kwargs)


def FaserSCT_DigitizationHSToolCfg(flags, name="FaserSCT_DigitizationHSTool", **kwargs):
    """Return ComponentAccumulator with hard scatter configured FaserSCT digitization tool"""
    kwargs.setdefault("OutputObjectName", "SCT_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_SDO_Map")
    #kwargs.setdefault("HardScatterSplittingMode", 1)
    return FaserSCT_DigitizationCommonCfg(flags, name, **kwargs)


def FaserSCT_DigitizationPUToolCfg(flags, name="FaserSCT_DigitizationPUTool",**kwargs):
    """Return ComponentAccumulator with pileup configured FaserSCT digitization tool"""
    kwargs.setdefault("OutputObjectName", "SCT_PU_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_PU_SDO_Map")
    #kwargs.setdefault("HardScatterSplittingMode", 2)
    return FaserSCT_DigitizationCommonCfg(flags, name, **kwargs)


def FaserSCT_OverlayDigitizationToolCfg(flags, name="FaserSCT_OverlayDigitizationTool",**kwargs):
    """Return ComponentAccumulator with overlay configured FaserSCT digitization tool"""
    acc = ComponentAccumulator()
    kwargs.setdefault("OnlyUseContainerName", False)
    kwargs.setdefault("OutputObjectName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "SCT_RDOs")
    kwargs.setdefault("OutputSDOName", "StoreGateSvc+" + flags.Overlay.SigPrefix + "SCT_SDO_Map")
    #kwargs.setdefault("HardScatterSplittingMode", 0)
    tool = acc.popToolsAndMerge(FaserSCT_DigitizationCommonCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc


def FaserSCT_DigitizationToolSplitNoMergePUCfg(flags, name="FaserSCT_DigitizationToolSplitNoMergePU",**kwargs):
    """Return ComponentAccumulator with merged pileup configured FaserSCT digitization tool"""
    kwargs.setdefault("InputObjectName", "PileupSCT_Hits")
    #kwargs.setdefault("HardScatterSplittingMode", 0)
    kwargs.setdefault("OutputObjectName", "SCT_PU_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_PU_SDO_Map")
    kwargs.setdefault("OnlyHitElements", True)
    kwargs.setdefault("FrontEnd", "PileupFaserSCT_FrontEnd")
    return FaserSCT_DigitizationCommonCfg(flags, name, **kwargs)


def FaserSCT_DigitizationToolGeantinoTruthCfg(flags, name="FaserSCT_GeantinoTruthDigitizationTool", **kwargs):
    """Return Geantino truth configured digitization tool"""
    kwargs.setdefault("ParticleBarcodeVeto", 0)
    return FaserSCT_DigitizationToolCfg(flags, name, **kwargs)


def FaserSCT_RandomDisabledCellGeneratorCfg(flags, name="FaserSCT_RandomDisabledCellGenerator", **kwargs):
    """Return configured random cell disabling tool"""
    kwargs.setdefault("TotalBadChannels", 0.01)
    FaserSCT_RandomDisabledCellGenerator=CompFactory.FaserSCT_RandomDisabledCellGenerator
    return FaserSCT_RandomDisabledCellGenerator(name, **kwargs)


def FaserSCT_AmpCfg(flags, name="FaserSCT_Amp", **kwargs):
    """Return configured amplifier and shaper tool"""
    kwargs.setdefault("CrossFactor2sides", 0.1)
    kwargs.setdefault("CrossFactorBack", 0.07)
    kwargs.setdefault("PeakTime", 21)
    kwargs.setdefault("deltaT", 1.0)
    kwargs.setdefault("Tmin", -25.0)
    kwargs.setdefault("Tmax", 150.0)
    FaserSCT_Amp=CompFactory.FaserSCT_Amp
    return FaserSCT_Amp(name, **kwargs)


def FaserSCT_SurfaceChargesGeneratorCfg(flags, name="FaserSCT_SurfaceChargesGenerator", **kwargs):
    """Return ComponentAccumulator with configured surface charges tool"""
    acc = ComponentAccumulator()
    kwargs.setdefault("FixedTime", -999)
    kwargs.setdefault("SubtractTime", -999)
    kwargs.setdefault("SurfaceDriftTime", 10)
    kwargs.setdefault("NumberOfCharges", 1)
    kwargs.setdefault("SmallStepLength", 5)
    kwargs.setdefault("DepletionVoltage", 70)
    kwargs.setdefault("BiasVoltage", 150)
    kwargs.setdefault("isOverlay", False)  # obsolete(?) Overlay flag
    # kwargs.setdefault("doTrapping", True) # ATL-INDET-INT-2016-019
    # experimental SCT_DetailedSurfaceChargesGenerator config dropped here
    FaserSCT_SurfaceChargesGenerator=CompFactory.FaserSCT_SurfaceChargesGenerator
    tool = FaserSCT_SurfaceChargesGenerator(name, **kwargs)
    DCSCondTool = acc.popToolsAndMerge(FaserSCT_DCSConditionsCfg(flags))
    SiliCondTool = FaserSCT_SiliconConditionsToolCfg(flags)
    SiliCondAcc = FaserSCT_SiliconConditionsCfg(flags, DCSConditionsTool=DCSCondTool)
    SiliPropsAcc = FaserSCT_SiPropertiesCfg(flags, SiConditionsTool=SiliCondTool)
    acc.merge(SiliCondAcc)
    tool.SiConditionsTool = SiliCondTool
    tool.SiPropertiesTool = acc.popToolsAndMerge(SiliPropsAcc)
    tool.LorentzAngleTool = acc.popToolsAndMerge(FaserSCT_LorentzAngleCfg(flags))
    acc.setPrivateTools(tool)
    return acc


def FaserSCT_FrontEndCfg(flags, name="FaserSCT_FrontEnd", **kwargs):
    """Return ComponentAccumulator with configured front-end electronics tool"""
    # Setup noise treament in FaserSCT_FrontEnd
    # To set the mean noise values for the different module types
    # Default values set at 0 degrees, plus/minus ~5 enc per plus/minus degree
    kwargs.setdefault("NoiseBarrel", 1500.0)
    kwargs.setdefault("NoiseBarrel3", 1541.0)
    kwargs.setdefault("NoiseInners", 1090.0)
    kwargs.setdefault("NoiseMiddles", 1557.0)
    kwargs.setdefault("NoiseShortMiddles", 940.0)
    kwargs.setdefault("NoiseOuters", 1618.0)
    kwargs.setdefault("NOBarrel", 1.5e-5)
    kwargs.setdefault("NOBarrel3", 2.1e-5)
    kwargs.setdefault("NOInners", 5.0e-9)
    kwargs.setdefault("NOMiddles", 2.7e-5)
    kwargs.setdefault("NOShortMiddles", 2.0e-9)
    kwargs.setdefault("NOOuters", 3.5e-5)
    if not flags.Digitization.DoInnerDetectorNoise:
        log = logging.getLogger("FaserSCT_FrontEndCfg")
        log.info("FaserSCT_Digitization:::: Turned off Noise in FaserSCT_FrontEnd")
        kwargs.setdefault("NoiseOn", False)
        kwargs.setdefault("AnalogueNoiseOn", False)
    else:
        kwargs.setdefault("NoiseOn", True)
        kwargs.setdefault("AnalogueNoiseOn", True)
    # In overlay MC, only analogue noise is on. Noise hits are not added.
    if False and flags.Input.isMC:  # obsolete(?) Overlay flag
        kwargs["NoiseOn"] = False
        kwargs["AnalogueNoiseOn"] = True
    # Use Calibration data from Conditions DB, still for testing purposes only
    kwargs.setdefault("UseCalibData", True)
    # Setup the ReadCalibChip folders and Svc
    acc = FaserSCT_ReadCalibChipDataCfg(flags)
    kwargs.setdefault("SCT_ReadCalibChipDataTool", acc.popPrivateTools())
    # DataCompressionMode: 1 is level mode x1x (default), 2 is edge mode 01x, 3 is expanded any hit xxx
    if False and flags.Input.isMC:  # obsolete(?) Overlay flag
        kwargs.setdefault("DataCompressionMode", 2)
    elif flags.Beam.BunchSpacing <= 50:
        kwargs.setdefault("DataCompressionMode", 1)
    else:
        kwargs.setdefault("DataCompressionMode", 3)
    # DataReadOutMode: 0 is condensed mode and 1 is expanded mode
    if False and flags.Input.isMC:  # obsolete(?) Overlay flag
        kwargs.setdefault("DataReadOutMode", 0)
    else:
        kwargs.setdefault("DataReadOutMode", 1)
    FaserSCT_FrontEnd=CompFactory.FaserSCT_FrontEnd
    acc.setPrivateTools(FaserSCT_FrontEnd(name, **kwargs))
    return acc


def FaserSCT_FrontEndPileupCfg(flags, name="PileupFaserSCT_FrontEnd", **kwargs):
    """Return ComponentAccumulator with pileup-configured front-end electronics tool"""
    kwargs.setdefault("NoiseBarrel", 0.0)
    kwargs.setdefault("NoiseBarrel3", 0.0)
    kwargs.setdefault("NoiseInners", 0.0)
    kwargs.setdefault("NoiseMiddles", 0.0)
    kwargs.setdefault("NoiseShortMiddles", 0.0)
    kwargs.setdefault("NoiseOuters", 0.0)
    kwargs.setdefault("NOBarrel", 0.0)
    kwargs.setdefault("NOBarrel3", 0.0)
    kwargs.setdefault("NOInners", 0.0)
    kwargs.setdefault("NOMiddles", 0.0)
    kwargs.setdefault("NOShortMiddles", 0.0)
    kwargs.setdefault("NOOuters", 0.0)
    kwargs.setdefault("NoiseOn", False)
    return FaserSCT_FrontEndCfg(flags, name, **kwargs)

def FaserSCT_RangeCfg(flags, name="SiliconRange", **kwargs):
    """Return a FaserSCT configured PileUpXingFolder tool"""
    kwargs.setdefault("FirstXing", FaserSCT_FirstXing())
    kwargs.setdefault("LastXing", FaserSCT_LastXing())
    kwargs.setdefault("CacheRefreshFrequency", 1.0) # default 0 no dataproxy reset
    kwargs.setdefault("ItemList", ["FaserSiHitCollection#SCT_Hits"] )
    PileUpXingFolder=CompFactory.PileUpXingFolder
    return PileUpXingFolder(name, **kwargs)


def FaserSCT_OutputCfg(flags):
    """Return ComponentAccumulator with Output for SCT. Not standalone."""
    acc = ComponentAccumulator()
    ItemList = ["FaserSCT_RDO_Container#*"]
    if flags.Digitization.TruthOutput:
        ItemList += ["TrackerSimDataCollection#*"]
        ItemList += ["FaserSiHitCollection#*"] # Also write out the raw hits
        acc.merge(TruthDigitizationOutputCfg(flags))
    acc.merge(OutputStreamCfg(flags, "RDO"))
    ostream = acc.getEventAlgo("OutputStreamRDO")
    #ostream.TakeItemsFromInput = True   # Don't write hits to RDO by default
    ostream.ItemList += ItemList
    return acc


def FaserSCT_DigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator for FaserSCT digitization"""
    acc = ComponentAccumulator()
    if "PileUpTools" not in kwargs:
        PileUpTools = acc.popToolsAndMerge(FaserSCT_DigitizationToolCfg(flags))
        kwargs["PileUpTools"] = PileUpTools
    acc.merge(PileUpToolsCfg(flags, **kwargs))
    return acc


def FaserSCT_OverlayDigitizationBasicCfg(flags, **kwargs):
    """Return ComponentAccumulator with FaserSCT Overlay digitization"""
    acc = ComponentAccumulator()
    if "DigitizationTool" not in kwargs:
        tool = acc.popToolsAndMerge(FaserSCT_OverlayDigitizationToolCfg(flags))
        kwargs["DigitizationTool"] = tool
    FaserSCT_Digitization=CompFactory.FaserSCT_Digitization
    acc.addEventAlgo(FaserSCT_Digitization(**kwargs))
    return acc


# with output defaults
def FaserSCT_DigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator for SCT digitization and Output"""
    acc = FaserSCT_DigitizationBasicCfg(flags, **kwargs)
    acc.merge(FaserSCT_OutputCfg(flags))
    return acc


def FaserSCT_OverlayDigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator with FaserSCT Overlay digitization and Output"""
    acc = FaserSCT_OverlayDigitizationBasicCfg(flags, **kwargs)
    acc.merge(FaserSCT_OutputCfg(flags))
    return acc


# additional specialisations
def FaserSCT_DigitizationHSCfg(flags, name="FaserSCT_DigitizationHS", **kwargs):
    """Return ComponentAccumulator for Hard-Scatter-only FaserSCT digitization and Output"""
    acc = FaserSCT_DigitizationHSToolCfg(flags)
    kwargs["PileUpTools"] = acc.popPrivateTools()
    acc = FaserSCT_DigitizationBasicCfg(flags, name=name, **kwargs)
    acc.merge(FaserSCT_OutputCfg(flags))
    return acc


def FaserSCT_DigitizationPUCfg(flags, name="FaserSCT_DigitizationPU", **kwargs):
    """Return ComponentAccumulator with Pile-up-only SCT digitization and Output"""
    acc = FaserSCT_DigitizationPUToolCfg(flags)
    kwargs["PileUpTools"] = acc.popPrivateTools()
    acc = FaserSCT_DigitizationBasicCfg(flags, name=name, **kwargs)
    acc.merge(FaserSCT_OutputCfg(flags))
    return acc
