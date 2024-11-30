# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
# from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def EcalGeometryCfg( flags ):
    from FaserGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc = acc.getPrimary()

    GeometryDBSvc = CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("CaloGeometryDBSvc"))

    EcalDetectorTool = CompFactory.EcalDetectorTool
    ecalDetectorTool = EcalDetectorTool()

    ecalDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    geoModelSvc.DetectorTools += [ ecalDetectorTool ]

    return acc
