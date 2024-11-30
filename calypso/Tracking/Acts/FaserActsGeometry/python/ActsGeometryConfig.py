# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.ComponentFactory import CompFactory
FaserActsExtrapolationTool,FaserActsMaterialTrackWriterSvc,FaserActsMaterialJsonWriterTool,FaserActsWriteTrackingGeometry,FaserActsTrackingGeometrySvc,FaserActsTrackingGeometryTool,FaserActsSurfaceMappingTool,FaserActsVolumeMappingTool,FaserActsObjWriterTool,FaserActsAlignmentCondAlg = CompFactory.getComps("FaserActsExtrapolationTool","FaserActsMaterialTrackWriterSvc","FaserActsMaterialJsonWriterTool","FaserActsWriteTrackingGeometry","FaserActsTrackingGeometrySvc","FaserActsTrackingGeometryTool","FaserActsSurfaceMappingTool","FaserActsVolumeMappingTool","FaserActsObjWriterTool","FaserActsAlignmentCondAlg")



def ActsTrackingGeometrySvcCfg(configFlags, name = "ActsTrackingGeometrySvc" , **kwargs) :
  result = ComponentAccumulator()
  actsTrackingGeometrySvc = FaserActsTrackingGeometrySvc(name = "FaserActsTrackingGeometrySvc", **kwargs)

  if configFlags.TrackingGeometry.MaterialSource == "Input":
    actsTrackingGeometrySvc.UseMaterialMap = True
    actsTrackingGeometrySvc.MaterialMapInputFile = "material-maps.json"
  if configFlags.TrackingGeometry.MaterialSource.find(".json") != -1:  
    actsTrackingGeometrySvc.UseMaterialMap = True
    actsTrackingGeometrySvc.MaterialMapInputFile = configFlags.TrackingGeometry.MaterialSource
  result.addService(actsTrackingGeometrySvc)
  return result

def ActsPropStepRootWriterSvcCfg(configFlags, 
                                 name="ActsPropStepRootWriterSvc",
                                 FilePath="propsteps.root",
                                 TreeName="propsteps"):
    result = ComponentAccumulator()

    ActsPropStepRootWriterSvc = CompFactory.ActsPropStepRootWriterSvc
    svc = ActsPropStepRootWriterSvc(name=name, 
                                    FilePath=FilePath, 
                                    TreeName=TreeName)

    result.addService(svc)

    return result

def ActsTrackingGeometryToolCfg(configFlags, name = "ActsTrackingGeometryTool" ) :
  result = ComponentAccumulator()
  acc = ActsTrackingGeometrySvcCfg(configFlags)
  result.merge(acc) 
  result.merge(ActsAlignmentCondAlgCfg(configFlags))
  Acts_ActsTrackingGeometryTool = FaserActsTrackingGeometryTool
  actsTrackingGeometryTool = Acts_ActsTrackingGeometryTool("TrackingGeometryTool")
  result.addPublicTool(actsTrackingGeometryTool)
  
  return result, actsTrackingGeometryTool

def NominalAlignmentCondAlgCfg(configFlags, name = "NominalAlignmentCondAlg", **kwargs) :
  result = ComponentAccumulator()
  
  acc = ActsTrackingGeometrySvcCfg(configFlags)
  result.merge(acc)
  
  Acts_NominalAlignmentCondAlg = CompFactory.NominalAlignmentCondAlg
  nominalAlignmentCondAlg = Acts_NominalAlignmentCondAlg(name, **kwargs)
  result.addCondAlgo(nominalAlignmentCondAlg)
  
  return result

def ActsAlignmentCondAlgCfg(configFlags, name = "ActsAlignmentCondAlg", **kwargs) :
  result = ComponentAccumulator()
  
  acc = ActsTrackingGeometrySvcCfg(configFlags)
  result.merge(acc)
  
  Acts_ActsAlignmentCondAlg = CompFactory.FaserActsAlignmentCondAlg
  actsAlignmentCondAlg = Acts_ActsAlignmentCondAlg(name, **kwargs)
  result.addCondAlgo(actsAlignmentCondAlg)
  
  return result

from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
def ActsExtrapolationToolCfg(configFlags, name="FaserActsExtrapolationTool", **kwargs) :
  result=ComponentAccumulator()
  
  acc  = MagneticFieldSvcCfg(configFlags)
  result.merge(acc)
  
  acc, actsTrackingGeometryTool = ActsTrackingGeometryToolCfg(configFlags) 
  result.merge(acc)
  
  Acts_ActsExtrapolationTool = CompFactory.FaserActsExtrapolationTool
  actsExtrapolationTool = Acts_ActsExtrapolationTool(name, **kwargs)
  result.addPublicTool(actsExtrapolationTool, primary=True)
  return result


def ActsMaterialTrackWriterSvcCfg(name="FaserActsMaterialTrackWriterSvc",
                                  FilePath="MaterialTracks_mapping.root",
                                  TreeName="material-tracks") :
  result = ComponentAccumulator()

  Acts_ActsMaterialTrackWriterSvc = CompFactory.FaserActsMaterialTrackWriterSvc
  ActsMaterialTrackWriterSvc = Acts_ActsMaterialTrackWriterSvc(name, 
                                                               FilePath=FilePath,
                                                               TreeName=TreeName)

  from AthenaCommon.Constants import INFO
  ActsMaterialTrackWriterSvc.OutputLevel = INFO
  result.addService(ActsMaterialTrackWriterSvc, primary=True)
  return result


def ActsSurfaceMappingToolCfg(configFlags, name = "FaserActsSurfaceMappingTool" ) :
  result=ComponentAccumulator()
    
  acc, actsTrackingGeometryTool = ActsTrackingGeometryToolCfg(configFlags) 
  result.merge(acc) 
  Acts_ActsSurfaceMappingTool = CompFactory.FaserActsSurfaceMappingTool
  ActsSurfaceMappingTool = Acts_ActsSurfaceMappingTool(name)
  ActsSurfaceMappingTool.TrackingGeometryTool = actsTrackingGeometryTool

  from AthenaCommon.Constants import INFO
  ActsSurfaceMappingTool.OutputLevel = INFO

  result.addPublicTool(ActsSurfaceMappingTool, primary=True)
  return result

def ActsVolumeMappingToolCfg(configFlags, name = "ActsVolumeMappingTool" ) :
  result=ComponentAccumulator()
    
  acc, actsTrackingGeometryTool = ActsTrackingGeometryToolCfg(configFlags) 
  result.merge(acc)

  Acts_ActsVolumeMappingTool = CompFactory.FaserActsVolumeMappingTool
  FaserActsVolumeMappingTool = Acts_ActsVolumeMappingTool(name)
  FaserActsVolumeMappingTool.TrackingGeometryTool = actsTrackingGeometryTool

  from AthenaCommon.Constants import INFO
  FaserActsVolumeMappingTool.OutputLevel = INFO

  result.addPublicTool(FaserActsVolumeMappingTool, primary=True)
  return result

def ActsMaterialJsonWriterToolCfg(name= "ActsMaterialJsonWriterTool", **kwargs) :
  result=ComponentAccumulator()
    
  Acts_ActsMaterialJsonWriterTool = CompFactory.FaserActsMaterialJsonWriterTool
  ActsMaterialJsonWriterTool = Acts_ActsMaterialJsonWriterTool(name, **kwargs)

  from AthenaCommon.Constants import INFO
  ActsMaterialJsonWriterTool.OutputLevel = INFO

  result.addPublicTool(ActsMaterialJsonWriterTool, primary=True)
  return result

def ActsObjWriterToolCfg(name= "ActsObjWriterTool", **kwargs) :
  result=ComponentAccumulator()
    
  Acts_ActsObjWriterTool = FaserActsObjWriterTool
  ActsObjWriterTool = Acts_ActsObjWriterTool(name, **kwargs)

  from AthenaCommon.Constants import INFO
  ActsObjWriterTool.OutputLevel = INFO

  result.addPublicTool(ActsObjWriterTool, primary=True)
  return result
