# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaCommon import Logging

#the physics region tools
from G4FaserTools.G4PhysicsRegionConfigNew import NeutrinoPhysicsRegionToolCfg, TrackerPhysicsRegionToolCfg, ScintillatorPhysicsRegionToolCfg, EcalPhysicsRegionToolCfg, CavernPhysicsRegionToolCfg

#the field config tools
from G4FaserTools.G4FieldConfigNew import FASERFieldManagerToolCfg, EmulsionFieldManagerToolCfg, VetoFieldManagerToolCfg, VetoNuFieldManagerToolCfg, TriggerFieldManagerToolCfg, PreshowerFieldManagerToolCfg, TrackerFieldManagerToolCfg, DipoleFieldManagerToolCfg, EcalFieldManagerToolCfg, TrenchFieldManagerToolCfg

from G4FaserTools.G4FaserToolsConfigNew import SensitiveDetectorMasterToolCfg

GeoDetectorTool=CompFactory.GeoDetectorTool

BoxEnvelope, MaterialDescriptionTool, G4AtlasDetectorConstructionTool = CompFactory.getComps("BoxEnvelope", "MaterialDescriptionTool", "G4AtlasDetectorConstructionTool",)

from AthenaCommon.SystemOfUnits import mm

#ToDo - finish migrating this (dnoel)
#Todo - just return component accumulator
#to still migrate: getCavernWorld, getCavernInfraGeoDetectorTool
#from ForwardRegionProperties.ForwardRegionPropertiesToolConfig import ForwardRegionPropertiesCfg

#put it here to avoid circular import?
# G4GeometryNotifierSvc=CompFactory.G4GeometryNotifierSvc
# def G4GeometryNotifierSvcCfg(ConfigFlags, name="G4GeometryNotifierSvc", **kwargs):
#     kwargs.setdefault("ActivateLVNotifier", True)
#     kwargs.setdefault("ActivatePVNotifier", False)
#     return G4GeometryNotifierSvc(name, **kwargs)

def G4GeometryNotifierSvcCfg(flags, name="G4FaserGeometryNotifierSvc", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ActivateLVNotifier", True)
    kwargs.setdefault("ActivatePVNotifier", False)
    result.addService(CompFactory.G4FaserGeometryNotifierSvc(name, **kwargs), primary = True)
    return result


def GeoDetectorToolCfg(flags, name='GeoDetectorTool', **kwargs):
    result = ComponentAccumulator()
    from FaserGeo2G4.FaserGeo2G4Config import FaserGeo2G4SvcCfg
    kwargs.setdefault("FaserGeo2G4Svc", result.getPrimaryAndMerge(FaserGeo2G4SvcCfg(flags)).name)
    #add the GeometryNotifierSvc
    kwargs.setdefault("GeometryNotifierSvc", result.getPrimaryAndMerge(G4GeometryNotifierSvcCfg(flags)).name)
    result.setPrivateTools(CompFactory.FaserGeoDetectorTool(name, **kwargs))
    return result


def EmulsionGeoDetectorToolCfg(flags, name='Emulsion', **kwargs):
    #set up geometry
    from EmulsionGeoModel.EmulsionGeoModelConfig import EmulsionGeometryCfg
    result=EmulsionGeometryCfg(flags)
    kwargs.setdefault("DetectorName", "Emulsion")
    result.setPrivateTools(result.popToolsAndMerge(GeoDetectorToolCfg(flags, name, **kwargs)))
    return result

def VetoGeoDetectorToolCfg(flags, name='Veto', **kwargs):
    #set up geometry
    from VetoGeoModel.VetoGeoModelConfig import VetoGeometryCfg
    result=VetoGeometryCfg(flags)
    kwargs.setdefault("DetectorName", "Veto")
    result.setPrivateTools(result.popToolsAndMerge(GeoDetectorToolCfg(flags, name, **kwargs)))
    return result
def VetoNuGeoDetectorToolCfg(flags, name='VetoNu', **kwargs):
    #set up geometry
    from VetoNuGeoModel.VetoNuGeoModelConfig import VetoNuGeometryCfg
    result=VetoNuGeometryCfg(flags)
    kwargs.setdefault("DetectorName", "VetoNu")
    result.setPrivateTools(result.popToolsAndMerge(GeoDetectorToolCfg(flags, name, **kwargs)))
    return result

def TriggerGeoDetectorToolCfg(flags, name='Trigger', **kwargs):
    from TriggerGeoModel.TriggerGeoModelConfig import TriggerGeometryCfg
    #set up geometry
    result=TriggerGeometryCfg(flags)
    kwargs.setdefault("DetectorName", "Trigger")
    result.setPrivateTools(result.popToolsAndMerge(GeoDetectorToolCfg(flags, name, **kwargs)))
    return result

def PreshowerGeoDetectorToolCfg(flags, name='Preshower', **kwargs):
    #set up geometry
    from PreshowerGeoModel.PreshowerGeoModelConfig import PreshowerGeometryCfg
    result=PreshowerGeometryCfg(flags)
    kwargs.setdefault("DetectorName", "Preshower")
    result.setPrivateTools(result.popToolsAndMerge(GeoDetectorToolCfg(flags, name, **kwargs)))
    return result

def SCTGeoDetectorToolCfg(flags, name='SCT', **kwargs):
    #set up geometry
    from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg
    result=FaserSCT_GeometryCfg(flags)
    kwargs.setdefault("DetectorName", "SCT")
    result.setPrivateTools(result.popToolsAndMerge(GeoDetectorToolCfg(flags, name, **kwargs)))
    return result

def DipoleGeoDetectorToolCfg(flags, name='Dipole', **kwargs):
    #set up geometry
    from DipoleGeoModel.DipoleGeoModelConfig import DipoleGeometryCfg
    result=DipoleGeometryCfg(flags)
    kwargs.setdefault("DetectorName", "Dipole")
    result.setPrivateTools(result.popToolsAndMerge(GeoDetectorToolCfg(flags, name, **kwargs)))
    return result

def EcalGeoDetectorToolCfg(flags, name='Ecal', **kwargs):
    #set up geometry
    from EcalGeoModel.EcalGeoModelConfig import EcalGeometryCfg
    result=EcalGeometryCfg(flags)
    kwargs.setdefault("DetectorName", "Ecal")
    result.setPrivateTools(result.popToolsAndMerge(GeoDetectorToolCfg(flags, name, **kwargs)))
    return result

def TrenchGeoDetectorToolCfg(flags, name='Trench', **kwargs):
    #set up geometry
    from FaserGeoModel.TrenchGMConfig import TrenchGeometryCfg
    result=TrenchGeometryCfg(flags)
    kwargs.setdefault("DetectorName", "Trench")
    result.setPrivateTools(result.popToolsAndMerge(GeoDetectorToolCfg(flags, name, **kwargs)))
    return result

def generateSubDetectorList(ConfigFlags):
    result = ComponentAccumulator()
    SubDetectorList=[]

    if ConfigFlags.Detector.GeometryEmulsion:
        toolEmulsion = result.popToolsAndMerge(EmulsionGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolEmulsion ]

    if ConfigFlags.Detector.GeometryVeto:
        toolVeto = result.popToolsAndMerge(VetoGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolVeto ]

    if ConfigFlags.Detector.GeometryVetoNu:
        toolVetoNu = result.popToolsAndMerge(VetoNuGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolVetoNu ]

    if ConfigFlags.Detector.GeometryTrigger:
        toolTrigger = result.popToolsAndMerge(TriggerGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolTrigger ]

    if ConfigFlags.Detector.GeometryPreshower:
        toolPreshower = result.popToolsAndMerge(PreshowerGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolPreshower ]

    if ConfigFlags.Detector.GeometryFaserSCT:
        toolSCT = result.popToolsAndMerge(SCTGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolSCT ]

    if ConfigFlags.Detector.GeometryDipole:
        toolDipole = result.popToolsAndMerge(DipoleGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolDipole ]

    if ConfigFlags.Detector.GeometryEcal:
        toolEcal = result.popToolsAndMerge(EcalGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolEcal ]

    if ConfigFlags.Detector.GeometryTrench:
        toolTrench = result.popToolsAndMerge(TrenchGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolTrench ]

    result.setPrivateTools(SubDetectorList)
    return result

def FASEREnvelopeCfg(ConfigFlags, name="Faser", **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("OffsetX", 0.0 * mm)
    kwargs.setdefault("OffsetY", 0.0 * mm)
    kwargs.setdefault("OffsetZ", 0.0 * mm)
    kwargs.setdefault("dX", 6000.0 * mm) 
    kwargs.setdefault("dY", 2500.0 * mm) 
    kwargs.setdefault("dZ", 12000.0 * mm) 

    kwargs.setdefault("DetectorName", "Faser")
    SubDetectorList = result.popToolsAndMerge(generateSubDetectorList(ConfigFlags))
    kwargs.setdefault("SubDetectors", SubDetectorList)
    result.setPrivateTools(BoxEnvelope(name, **kwargs))
    return result

def MaterialDescriptionToolCfg(ConfigFlags, name="MaterialDescriptionTool", **kwargs):
    ## kwargs.setdefault("SomeProperty", aValue)
    result = ComponentAccumulator()
    result.setPrivateTools(MaterialDescriptionTool(name, **kwargs))
    return result


# def VoxelDensityToolCfg(ConfigFlags, name="VoxelDensityTool", **kwargs):
#     ## kwargs.setdefault("SomeProperty", aValue)
#     voxelDensitySettings = {}
#     # if ConfigFlags.Detector.GeometryITkPixel:
#     #     voxelDensitySettings["ITkPixelDetector"] = 0.05
#     # if ConfigFlags.Detector.GeometryITkStrip:
#     #     voxelDensitySettings["ITkStrip::Barrel"] = 0.05
#     #     voxelDensitySettings["ITkStrip::ITkStrip_Forward"] = 0.05
#     #     ##The below is only needed temporarily, while we wait for
#     #     ##improved naming to be propagated to all necessary geo tags
#     #     voxelDensitySettings["ITkStrip::SCT_Forward"] = 0.05
#     kwargs.setdefault("VolumeVoxellDensityLevel",voxelDensitySettings)
#     result = ComponentAccumulator()
#     result.setPrivateTools(VoxelDensityTool(name, **kwargs))
#     return result

def getFASER_RegionCreatorList(ConfigFlags):
    regionCreatorList = []

    if ConfigFlags.Detector.GeometryNeutrino:
        regionCreatorList += [NeutrinoPhysicsRegionToolCfg(ConfigFlags)]

    if ConfigFlags.Detector.GeometryTracker:
        regionCreatorList += [TrackerPhysicsRegionToolCfg(ConfigFlags)]

    if ConfigFlags.Detector.GeometryScintillator:
        regionCreatorList += [ScintillatorPhysicsRegionToolCfg(ConfigFlags)]

    if ConfigFlags.Detector.GeometryFaserCalo:
        regionCreatorList += [EcalPhysicsRegionToolCfg(ConfigFlags)]
    
    if ConfigFlags.Detector.GeometryFaserCavern:
        regionCreatorList += [CavernPhysicsRegionToolCfg(ConfigFlags)]

    return regionCreatorList

def FASER_FieldMgrListCfg(ConfigFlags):
    result = ComponentAccumulator()
    fieldMgrList = []

    acc   = FASERFieldManagerToolCfg(ConfigFlags)
    tool  = result.popToolsAndMerge(acc)
    fieldMgrList += [tool]

    if ConfigFlags.Detector.GeometryEmulsion:
        acc = EmulsionFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]

    if ConfigFlags.Detector.GeometryVeto:
        acc = VetoFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]

    if ConfigFlags.Detector.GeometryVetoNu:
        acc = VetoNuFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]

    if ConfigFlags.Detector.GeometryTrigger:
        acc = TriggerFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]

    if ConfigFlags.Detector.GeometryPreshower:
        acc = PreshowerFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]

    if ConfigFlags.Detector.GeometryTracker:
        acc = TrackerFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]

    if ConfigFlags.Detector.GeometryDipole:
        acc = DipoleFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]

    if ConfigFlags.Detector.GeometryEcal:
        acc = EcalFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]

    if ConfigFlags.Detector.GeometryTrench:
        acc = TrenchFieldManagerToolCfg(ConfigFlags)
        tool  = result.popToolsAndMerge(acc)
        fieldMgrList += [tool]


    result.setPrivateTools(fieldMgrList)
    return result

def getGeometryConfigurationTools(ConfigFlags):
    geoConfigToolList = []
    # The methods for these tools should be defined in the
    # package containing each tool, so G4FaserTools in this case
    result =ComponentAccumulator()
    geoConfigToolList += [result.popToolsAndMerge(MaterialDescriptionToolCfg(ConfigFlags))]
    # geoConfigToolList += [result.popToolsAndMerge(VoxelDensityToolCfg(ConfigFlags))]
    return result, geoConfigToolList


def G4AtlasDetectorConstructionToolCfg(ConfigFlags, name="G4FaserDetectorConstructionTool", **kwargs):
    result = ComponentAccumulator()

    ## For now just have the same geometry configurations tools loaded for ATLAS and TestBeam
    geoConfAcc, listOfGeoConfTools = getGeometryConfigurationTools(ConfigFlags)
    result.merge(geoConfAcc)
    kwargs.setdefault("GeometryConfigurationTools", listOfGeoConfTools)

    # Getting this tool by name works, but not if you use getSensitiveDetectorMasterTool()
    tool = result.popToolsAndMerge(SensitiveDetectorMasterToolCfg(ConfigFlags))
    result.addPublicTool(tool)
    kwargs.setdefault("SenDetMasterTool", result.getPublicTool(tool.name))

    toolGeo = result.popToolsAndMerge(FASEREnvelopeCfg(ConfigFlags))
    kwargs.setdefault("World", toolGeo)
    kwargs.setdefault("RegionCreators", getFASER_RegionCreatorList(ConfigFlags))
    #if hasattr(simFlags, 'MagneticField') and simFlags.MagneticField.statusOn:
    if True:
        acc = FASER_FieldMgrListCfg(ConfigFlags)
        fieldMgrList = result.popToolsAndMerge(acc)
        kwargs.setdefault("FieldManagers", fieldMgrList)
    
    result.setPrivateTools(G4AtlasDetectorConstructionTool(name, **kwargs))
    return result


