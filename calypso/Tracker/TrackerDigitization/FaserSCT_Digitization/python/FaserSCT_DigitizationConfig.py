# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
# The earliest bunch crossing time for which interactions will be sent
# to the SCT Digitization code.
def FaserSCT_FirstXing():
    return -50
# The latest bunch crossing time for which interactions will be sent
# to the SCT Digitization code.
def FaserSCT_LastXing():
    return 25



######################################################################################
def getFaserSCT_RandomDisabledCellGenerator(name="FaserSCT_RandomDisabledCellGenerator", **kwargs):
    kwargs.setdefault("TotalBadChannels", 0.01)
    from FaserSCT_Digitization.FaserSCT_DigitizationConf import FaserSCT_RandomDisabledCellGenerator
    return FaserSCT_RandomDisabledCellGenerator(name, **kwargs)


######################################################################################
def getFaserSCT_Amp(name="FaserSCT_Amp", **kwargs):
    kwargs.setdefault("CrossFactor2sides", 0.1)
    kwargs.setdefault("CrossFactorBack", 0.07)
    kwargs.setdefault("PeakTime", 21)
    kwargs.setdefault("deltaT", 1.0)
    kwargs.setdefault("Tmin", -25.0)
    kwargs.setdefault("Tmax", 150.0)
    from FaserSCT_Digitization.FaserSCT_DigitizationConf import FaserSCT_Amp
    return FaserSCT_Amp(name, **kwargs)


######################################################################################
def getFaserSCT_SurfaceChargesGenerator(name="FaserSCT_SurfaceChargesGenerator", **kwargs):
    ## Set up services used by SCT_SurfaceChargesGenerator
    # Set up SCT_DCSConditiosnTool
    from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
    sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
    sct_DCSConditionsToolSetup.setup()
    # Set up SCT_SiliconConditionsTool
    from SCT_ConditionsTools.SCT_SiliconConditionsToolSetup import SCT_SiliconConditionsToolSetup
    sct_SiliconConditionsToolSetup = SCT_SiliconConditionsToolSetup()
    sct_SiliconConditionsToolSetup.setDcsTool(sct_DCSConditionsToolSetup.getTool())
    sct_SiliconConditionsToolSetup.setup()
    # Set up SCT_SiPropertiesTool
    from FaserSiPropertiesTool.FaserSCT_SiPropertiesToolSetup import FaserSCT_SiPropertiesToolSetup
    sct_SiPropertiesToolSetup = FaserSCT_SiPropertiesToolSetup()
    sct_SiPropertiesToolSetup.setSiliconTool(sct_SiliconConditionsToolSetup.getTool())
    sct_SiPropertiesToolSetup.setup()
    ## Charge trapping tool - used by SCT_SurfaceChargesGenerator
#    from AthenaCommon.AppMgr import ToolSvc
    ## SiLorentzAngleTool for SCT_SurfaceChargesGenerator
    from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
    sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()

    kwargs.setdefault("FixedTime", -999)
    kwargs.setdefault("SubtractTime", -999)
    kwargs.setdefault("SurfaceDriftTime", 10)
    kwargs.setdefault("NumberOfCharges", 1)
    kwargs.setdefault("SmallStepLength", 5)
    kwargs.setdefault("DepletionVoltage", 70)
    kwargs.setdefault("BiasVoltage", 150)
    kwargs.setdefault("SiPropertiesTool", sct_SiPropertiesToolSetup.getTool())
    kwargs.setdefault("LorentzAngleTool", sctLorentzAngleToolSetup.SCTLorentzAngleTool)
    from AthenaCommon.GlobalFlags import globalflags
    kwargs.setdefault("isOverlay", globalflags.isOverlay())

    # kwargs.setdefault("doTrapping", True) # ATL-INDET-INT-2016-019

    # from Digitization.DigitizationFlags import digitizationFlags
    # if 'doDetailedSurfChargesGen' in digitizationFlags.experimentalDigi():
    #     kwargs.setdefault("ChargeDriftModel", 1)
    #     kwargs.setdefault("EFieldModel", 2)
    #     kwargs.setdefault("MagneticField", -2.0)
    #     kwargs.setdefault("SensorTemperature", 273.15)
    #     kwargs.setdefault("TransportTimeStep", 0.25)
    #     kwargs.setdefault("TransportTimeMax", 25.0)
    #     from FaserSCT_Digitization.FaserSCT_DigitizationConf import SCT_DetailedSurfaceChargesGenerator
    #     return SCT_DetailedSurfaceChargesGenerator(name, **kwargs)
    # else:
    # from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_RadDamageSummaryTool
    # kwargs.setdefault("RadDamageSummaryTool", SCT_RadDamageSummaryTool(name = "InDetSCT_RadDamageSummaryTool"))
    from FaserSCT_Digitization.FaserSCT_DigitizationConf import FaserSCT_SurfaceChargesGenerator
    return FaserSCT_SurfaceChargesGenerator(name, **kwargs)

######################################################################################
def getFaserSCT_FrontEnd(name="FaserSCT_FrontEnd", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    #Setup noise treament in SCT_FrontEnd
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
    # If noise is turned off:
    if not digitizationFlags.doInDetNoise.get_Value():
        ###kwargs.setdefault("OnlyHitElements", True)
        print('FaserSCT_Digitization:::: Turned off Noise in FaserSCT_FrontEnd')
        kwargs.setdefault("NoiseOn", False)
        kwargs.setdefault("AnalogueNoiseOn", False)
    else:
        kwargs.setdefault("NoiseOn", True)
        kwargs.setdefault("AnalogueNoiseOn", True)
    # In overlay MC, only analogue noise is on. Noise hits are not added.
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.isOverlay() and globalflags.DataSource == 'geant4':
        kwargs["NoiseOn"] = False
        kwargs["AnalogueNoiseOn"] = True
    # Use Calibration data from Conditions DB, still for testing purposes only
    kwargs.setdefault("UseCalibData", True)

    # Setup the ReadCalibChip folders and Svc
    from SCT_ConditionsTools.SCT_ReadCalibChipDataToolSetup import SCT_ReadCalibChipDataToolSetup
    sct_ReadCalibChipDataToolSetup = SCT_ReadCalibChipDataToolSetup()
    sct_ReadCalibChipDataToolSetup.setup()
    kwargs.setdefault("SCT_ReadCalibChipDataTool", sct_ReadCalibChipDataToolSetup.getTool())
    # DataCompressionMode: 1 is level mode x1x (default), 2 is edge mode 01x, 3 is expanded any hit xxx
    from AthenaCommon.BeamFlags import jobproperties
    if globalflags.isOverlay() and globalflags.DataSource == 'geant4':
        kwargs.setdefault("DataCompressionMode", 2)
    elif (jobproperties.Beam.bunchSpacing() <= 50):
        kwargs.setdefault("DataCompressionMode", 1) 
    else: 
        kwargs.setdefault("DataCompressionMode", 3) 
    # DataReadOutMode: 0 is condensed mode and 1 is expanded mode
    if globalflags.isOverlay() and globalflags.DataSource == 'geant4':
        kwargs.setdefault("DataReadOutMode", 0)
    else:
        kwargs.setdefault("DataReadOutMode", 1)
    from FaserSCT_Digitization.FaserSCT_DigitizationConf import FaserSCT_FrontEnd
    return FaserSCT_FrontEnd(name, **kwargs)

######################################################################################
def getPileupFaserSCT_FrontEnd(name="PileupFaserSCT_FrontEnd", **kwargs):

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

    return getFaserSCT_FrontEnd(name, **kwargs)

######################################################################################


def commonFaserSCT_DigitizationConfig(name,**kwargs):

    from Digitization.DigitizationFlags import digitizationFlags
    # If noise is turned off:
    if not digitizationFlags.doInDetNoise.get_Value():
        kwargs.setdefault("OnlyHitElements", True)

    kwargs.setdefault("InputObjectName", "SCT_Hits")
    kwargs.setdefault("EnableHits", True)
    kwargs.setdefault("BarrelOnly", False)

    # Use of random disabled cells
    #kwargs.setdefault("RandomDisabledCells", True)

    # Set FixedTime for cosmics for use in SurfaceChargesGenerator
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType == "cosmics" :
        kwargs.setdefault("CosmicsRun", True)
        kwargs.setdefault("FixedTime", 10)

    # write out SCT1_RawData
    #kwargs.setdefault("WriteSCT1_RawData", False)

#    if digitizationFlags.doXingByXingPileUp():
#        kwargs.setdefault("FirstXing", SCT_FirstXing())
#        kwargs.setdefault("LastXing",  SCT_LastXing() )

    from AthenaCommon import CfgMgr
    return CfgMgr.FaserSCT_DigitizationTool(name,**kwargs)
#    else:
#        from AthenaCommon import CfgMgr
#        return CfgMgr.SCT_Digitization(name, **kwargs)

######################################################################################

def FaserSCT_DigitizationTool(name="FaserSCT_DigitizationTool", **kwargs):
#    from Digitization.DigitizationFlags import digitizationFlags
    kwargs.setdefault("OutputObjectName", "SCT_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_SDO_Map")

    # kwargs.setdefault("HardScatterSplittingMode", 0)
    return commonFaserSCT_DigitizationConfig(name,**kwargs)

######################################################################################

def FaserSCT_GeantinoTruthDigitizationTool(name="FaserSCT_GeantinoTruthDigitizationTool", **kwargs):
    kwargs.setdefault("ParticleBarcodeVeto", 0)
    return FaserSCT_DigitizationTool(name,**kwargs)

######################################################################################

def FaserSCT_DigitizationToolHS(name="FaserSCT_DigitizationToolHS",**kwargs):
    kwargs.setdefault("OutputObjectName", "SCT_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_SDO_Map")
    # kwargs.setdefault("HardScatterSplittingMode", 1)
    return commonFaserSCT_DigitizationConfig(name,**kwargs)

######################################################################################

def FaserSCT_DigitizationToolPU(name="FaserSCT_DigitizationToolPU",**kwargs):
    kwargs.setdefault("OutputObjectName", "SCT_PU_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_PU_SDO_Map")
    # kwargs.setdefault("HardScatterSplittingMode", 2)
    return commonFaserSCT_DigitizationConfig(name,**kwargs)

######################################################################################

def FaserSCT_DigitizationToolSplitNoMergePU(name="FaserSCT_DigitizationToolSplitNoMergePU",**kwargs):

    kwargs.setdefault("InputObjectName", "PileupSCT_Hits")
    # kwargs.setdefault("HardScatterSplittingMode", 0)
    kwargs.setdefault("OutputObjectName", "SCT_PU_RDOs")
    kwargs.setdefault("OutputSDOName", "SCT_PU_SDO_Map")
    kwargs.setdefault("OnlyHitElements", True)
    kwargs.setdefault("FrontEnd", "PileupFaserSCT_FrontEnd")

    return commonFaserSCT_DigitizationConfig(name,**kwargs)

######################################################################################

def FaserSCT_OverlayDigitizationTool(name="FaserSCT_OverlayDigitizationTool",**kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("OnlyUseContainerName", False)
        kwargs.setdefault("OutputObjectName", overlayFlags.sigPrefix() + "SCT_RDOs")
        kwargs.setdefault("OutputSDOName", overlayFlags.sigPrefix() + "SCT_SDO_Map")
    else:
        kwargs.setdefault("OutputObjectName", overlayFlags.evtStore() + "+SCT_RDOs")
        kwargs.setdefault("OutputSDOName", overlayFlags.evtStore() + "+SCT_SDO_Map")
    # kwargs.setdefault("HardScatterSplittingMode", 0)
    return commonFaserSCT_DigitizationConfig(name,**kwargs)

######################################################################################

def getFaserSiliconRange(name="FaserSiliconRange" , **kwargs):
    #this is the time of the xing in ns
    kwargs.setdefault('FirstXing', FaserSCT_FirstXing() )
    kwargs.setdefault('LastXing',  FaserSCT_LastXing()  )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["FaserSiHitCollection#SCT_Hits"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)

######################################################################################

def FaserSCT_DigitizationHS(name="FaserSCT_DigitizationHS",**kwargs):
    kwargs.setdefault("DigitizationTool", "FaserSCT_DigitizationToolHS")
    from FaserSCT_Digitization.FaserSCT_DigitizationConf import FaserSCT_Digitization
    return FaserSCT_Digitization(name,**kwargs)

######################################################################################

def FaserSCT_DigitizationPU(name="FaserSCT_DigitizationPU",**kwargs):
    kwargs.setdefault("DigitizationTool", "FaserSCT_DigitizationToolPU")
    return CfgMgr.FaserSCT_Digitization(name,**kwargs)

######################################################################################

def FaserSCT_OverlayDigitization(name="FaserSCT_OverlayDigitization",**kwargs):
    kwargs.setdefault("DigitizationTool", "FaserSCT_OverlayDigitizationTool")
    # Multi-threading settinggs
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    if is_hive:
        kwargs.setdefault('Cardinality', concurrencyProps.ConcurrencyFlags.NumThreads())

    return CfgMgr.FaserSCT_Digitization(name,**kwargs)
