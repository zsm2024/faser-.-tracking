# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
# from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def VetoNuGeometryCfg( flags ):
    from FaserGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc = acc.getPrimary()

    GeometryDBSvc=CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("ScintGeometryDBSvc"))

    VetoNuDetectorTool = CompFactory.VetoNuDetectorTool
    vetoNuDetectorTool = VetoNuDetectorTool()

    vetoNuDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    geoModelSvc.DetectorTools += [ vetoNuDetectorTool ]

    return acc
