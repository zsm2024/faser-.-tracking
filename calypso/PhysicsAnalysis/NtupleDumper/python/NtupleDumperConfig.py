"""
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
from FaserActsGeometry.ActsGeometryConfig import ActsTrackingGeometryToolCfg
from WaveformConditionsTools.WaveformCableMappingConfig import WaveformCableMappingCfg

def NtupleDumperAlgCfg(flags, OutName, **kwargs):
    # Initialize GeoModel
    from FaserGeoModel.FaserGeoModelConfig import FaserGeometryCfg
    acc = FaserGeometryCfg(flags)

    acc.merge(MagneticFieldSvcCfg(flags))
    #acc.merge(ActsTrackingGeometrySvcCfg(flags))
    #acc.merge(FaserActsAlignmentCondAlgCfg(flags))
    acc.merge(WaveformCableMappingCfg(flags, **kwargs))

    result, actsTrackingGeometryTool = ActsTrackingGeometryToolCfg(flags)
    acc.merge(result)

    actsExtrapolationTool = CompFactory.FaserActsExtrapolationTool("FaserActsExtrapolationTool")
    actsExtrapolationTool.MaxSteps = 10000
    actsExtrapolationTool.TrackingGeometryTool = actsTrackingGeometryTool 
    
    NtupleDumperAlg = CompFactory.NtupleDumperAlg("NtupleDumperAlg",**kwargs)
    NtupleDumperAlg.ExtrapolationTool = actsExtrapolationTool
    acc.addEventAlgo(NtupleDumperAlg)

    thistSvc = CompFactory.THistSvc()
    thistSvc.Output += [f"HIST2 DATAFILE='{OutName}' OPT='RECREATE'"]
    acc.addService(thistSvc)

    return acc

