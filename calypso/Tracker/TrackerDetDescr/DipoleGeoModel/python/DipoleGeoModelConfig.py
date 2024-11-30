# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

def DipoleGeometryCfg( flags ):
    from FaserGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc=acc.getPrimary()
    
    GeometryDBSvc = CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("TrackerGeometryDBSvc"))
    
    # from RDBAccessSvc.RDBAccessSvcConf import RDBAccessSvc
    # acc.addService(RDBAccessSvc("RDBAccessSvc"))

    # from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    # acc.addService(DBReplicaSvc("DBReplicaSvc"))

    DipoleTool = CompFactory.DipoleTool
    dipoleTool = DipoleTool()

    geoModelSvc.DetectorTools += [ dipoleTool ]
    
    return acc
