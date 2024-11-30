"""
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
#from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg

FaserActsMaterialJsonWriterTool,FaserActsWriteTrackingGeometry,FaserActsTrackingGeometrySvc,FaserActsTrackingGeometryTool,FaserActsObjWriterTool,FaserActsAlignmentCondAlg = CompFactory.getComps("FaserActsMaterialJsonWriterTool","FaserActsWriteTrackingGeometry","FaserActsTrackingGeometrySvc","FaserActsTrackingGeometryTool","FaserActsObjWriterTool","FaserActsAlignmentCondAlg")

#def FaserActsTrackingGeometrySvcCfg(flags, **kwargs):
#    acc = ComponentAccumulator()
#    acc.addService(FaserActsTrackingGeometrySvc(name = "FaserActsTrackingGeometrySvc", **kwargs))
#    return acc 


def FaserActsAlignmentCondAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    acc.addCondAlgo(CompFactory.FaserActsAlignmentCondAlg(name = "FaserActsAlignmentCondAlg", **kwargs))
    #acc.addCondAlgo(CompFactory.NominalAlignmentCondAlg(name = "NominalAlignmentCondAlg", **kwargs))
    return acc


def FaserActsWriteTrackingGeometryBasicCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    actsWriteTrackingGeometry=FaserActsWriteTrackingGeometry(**kwargs)
    from FaserActsGeometry.ActsGeometryConfig import ActsTrackingGeometryToolCfg
    result, actsTrackingGeometryTool = ActsTrackingGeometryToolCfg(flags) 
    acc.merge(result)
    actsWriteTrackingGeometry.TrackingGeometryTool=actsTrackingGeometryTool
    actsWriteTrackingGeometry.ObjWriterTool=FaserActsObjWriterTool("FaserActsObjWriterTool",OutputDirectory="./", SubDetectors=["Station_0","Station_1","Station_2","Station_3"])
    actsWriteTrackingGeometry.MaterialJsonWriterTool= FaserActsMaterialJsonWriterTool(OutputFile = "geometry-maps.json",
       processSensitives = False,
       processnonmaterial = False)

    acc.addEventAlgo(actsWriteTrackingGeometry)
    return acc


def FaserActsWriteTrackingGeometry_OutputCfg(flags):                                                                                                    
    acc = ComponentAccumulator()
    return acc


def FaserActsWriteTrackingGeometryCfg(flags, **kwargs):
    #acc = FaserGeometryCfg(flags)
    acc = FaserSCT_GeometryCfg(flags)
    #acc.merge(FaserActsTrackingGeometrySvcCfg(flags, **kwargs))
    #acc.merge(FaserActsAlignmentCondAlgCfg(flags))
    acc.merge(FaserActsWriteTrackingGeometryBasicCfg(flags, **kwargs))
    acc.merge(FaserActsWriteTrackingGeometry_OutputCfg(flags))
    return acc
