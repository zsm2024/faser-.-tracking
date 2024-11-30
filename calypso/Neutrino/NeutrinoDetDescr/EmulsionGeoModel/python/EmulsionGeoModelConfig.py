# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
# from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def EmulsionGeometryCfg( flags ):
    from FaserGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc = acc.getPrimary()

    if flags.Detector.GeometryEmulsion:
        GeometryDBSvc = CompFactory.GeometryDBSvc
        acc.addService(GeometryDBSvc("NeutrinoGeometryDBSvc"))

        EmulsionDetectorTool = CompFactory.EmulsionDetectorTool
        emulsionDetectorTool = EmulsionDetectorTool()

        emulsionDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
        geoModelSvc.DetectorTools += [ emulsionDetectorTool ]

    return acc
