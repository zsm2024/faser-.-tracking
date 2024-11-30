"""
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
# from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
from FaserSCT_GeoModel.FaserSCT_GeoModelConfig import FaserSCT_GeometryCfg
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg


FaserActsExtrapolationAlg,FaserActsExtrapolationTool,FaserActsTrackingGeometrySvc,FaserActsTrackingGeometryTool,FaserActsAlignmentCondAlg = CompFactory.getComps("FaserActsExtrapolationAlg","FaserActsExtrapolationTool","FaserActsTrackingGeometrySvc","FaserActsTrackingGeometryTool","FaserActsAlignmentCondAlg")

#def FaserActsTrackingGeometrySvcCfg(flags, **kwargs):
#    acc = ComponentAccumulator()
#    acc.addService(FaserActsTrackingGeometrySvc(name = "FaserActsTrackingGeometrySvc", **kwargs))
#    return acc 


def FaserActsAlignmentCondAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    acc.addCondAlgo(CompFactory.FaserActsAlignmentCondAlg(name = "FaserActsAlignmentCondAlg", **kwargs))
    #acc.addCondAlgo(CompFactory.NominalAlignmentCondAlg(name = "NominalAlignmentCondAlg", **kwargs))
    return acc


def FaserActsExtrapolationAlgBasicCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    actsExtrapolationAlg=FaserActsExtrapolationAlg(**kwargs)
    actsExtrapolationTool=FaserActsExtrapolationTool("FaserActsExtrapolationTool")
    actsExtrapolationTool.FieldMode="FASER"
    #actsExtrapolationTool.FieldMode="Constant"
    from FaserActsGeometry.ActsGeometryConfig import ActsTrackingGeometryToolCfg
    result, actsTrackingGeometryTool = ActsTrackingGeometryToolCfg(flags) 
    actsExtrapolationTool.TrackingGeometryTool=actsTrackingGeometryTool
    actsExtrapolationAlg.ExtrapolationTool=actsExtrapolationTool
    actsExtrapolationAlg.EtaRange=[1, 10]
    actsExtrapolationAlg.PtRange=[0.1, 100]
    actsExtrapolationAlg.NParticlesPerEvent=int(1e4)
    acc.merge(result)
    acc.addEventAlgo(actsExtrapolationAlg)
    return acc


def FaserActsExtrapolationAlg_OutputCfg(flags):                                                                                                    
    acc = ComponentAccumulator()
    return acc


def FaserActsExtrapolationAlgCfg(flags, **kwargs):
    # Initialize GeoModel
    #acc = FaserGeometryCfg(flags)
    acc = FaserSCT_GeometryCfg(flags)

    # Initialize field service
    acc.merge(MagneticFieldSvcCfg(flags))

    #acc.merge(FaserActsTrackingGeometrySvcCfg(flags, **kwargs))
    #acc.merge(FaserActsAlignmentCondAlgCfg(flags))
    acc.merge(FaserActsExtrapolationAlgBasicCfg(flags, **kwargs))
    acc.merge(FaserActsExtrapolationAlg_OutputCfg(flags))
    return acc
