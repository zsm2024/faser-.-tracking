# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file: AtlasGeoModel/python/GeoModelInit.py
## @brief: Encapsulate GeoModel configuration
 
def _setupGeoModel():
    from AthenaCommon.JobProperties import jobproperties
    from AthenaCommon.AppMgr import theApp
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
 
    import DetDescrCnvSvc.DetStoreConfig
    svcMgr.DetDescrCnvSvc.IdDictFromRDB = True

    # Conditions DB setup and TagInfo
    from IOVDbSvc.CondDB import conddb
    import EventInfoMgt.EventInfoMgtInit
 
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    geoModelSvc = GeoModelSvc()
    svcMgr += geoModelSvc
    theApp.CreateSvc += [ "GeoModelSvc" ]

    DDversion    = jobproperties.Global.DetDescrVersion()

    # Set up detector tools here


    if "FASERNU" in DDversion and not hasattr(svcMgr,'NeutrinoGeometryDBSvc'):
        from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
        svcMgr+=GeometryDBSvc("NeutrinoGeometryDBSvc")

    if not hasattr(svcMgr,'ScintGeometryDBSvc'):
        from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
        svcMgr+=GeometryDBSvc("ScintGeometryDBSvc")

    if not hasattr(svcMgr, 'TrackerGeometryDBSvc'):
        from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
        svcMgr+=GeometryDBSvc("TrackerGeometryDBSvc")

    if not hasattr(svcMgr, 'CaloGeometryDBSvc'):
        from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
        svcMgr+=GeometryDBSvc("CaloGeometryDBSvc")

    # from AthenaCommon import CfgGetter
    if "FASERNU" in DDversion:
        from EmulsionGeoModel.EmulsionGeoModelConf import EmulsionDetectorTool
        emulsionDetectorTool = EmulsionDetectorTool(DetectorName = "Emulsion", Alignable = True, RDBAccessSvc = "RDBAccessSvc", GeometryDBSvc = "NeutrinoGeometryDBSvc", GeoDbTagSvc = "GeoDbTagSvc")
        geoModelSvc.DetectorTools += [ emulsionDetectorTool ]

    if "FASERNU-03" in DDversion or "FASERNU-04" in DDversion:
        from VetoNuGeoModel.VetoNuGeoModelConf import VetoNuDetectorTool
        vetoNuDetectorTool = VetoNuDetectorTool( DetectorName = "VetoNu",
                                            Alignable = True,
                                            RDBAccessSvc = "RDBAccessSvc",
                                            GeometryDBSvc = "ScintGeometryDBSvc",
                                            GeoDbTagSvc = "GeoDbTagSvc")

        geoModelSvc.DetectorTools += [ vetoNuDetectorTool ]

    from VetoGeoModel.VetoGeoModelConf import VetoDetectorTool
    vetoDetectorTool = VetoDetectorTool( DetectorName = "Veto",
                                         Alignable = True,
                                         RDBAccessSvc = "RDBAccessSvc",
                                         GeometryDBSvc = "ScintGeometryDBSvc",
                                         GeoDbTagSvc = "GeoDbTagSvc")

    geoModelSvc.DetectorTools += [ vetoDetectorTool ]

    from TriggerGeoModel.TriggerGeoModelConf import TriggerDetectorTool
    triggerDetectorTool = TriggerDetectorTool( DetectorName = "Trigger",
                                               Alignable = True,
                                               RDBAccessSvc = "RDBAccessSvc",
                                               GeometryDBSvc = "ScintGeometryDBSvc",
                                               GeoDbTagSvc = "GeoDbTagSvc")

    geoModelSvc.DetectorTools += [ triggerDetectorTool ]

    from PreshowerGeoModel.PreshowerGeoModelConf import PreshowerDetectorTool
    preshowerDetectorTool = PreshowerDetectorTool( DetectorName = "Preshower",
                                                   Alignable = True,
                                                   RDBAccessSvc = "RDBAccessSvc",
                                                   GeometryDBSvc = "ScintGeometryDBSvc",
                                                   GeoDbTagSvc = "GeoDbTagSvc")

    geoModelSvc.DetectorTools += [ preshowerDetectorTool ]

    from FaserSCT_GeoModel.FaserSCT_GeoModelConf import FaserSCT_DetectorTool
    faserSCTDetectorTool = FaserSCT_DetectorTool( DetectorName = "SCT",
                                                  Alignable = True,
                                                  RDBAccessSvc = "RDBAccessSvc",
                                                  GeometryDBSvc = "TrackerGeometryDBSvc",
                                                  GeoDbTagSvc = "GeoDbTagSvc")

    geoModelSvc.DetectorTools += [ faserSCTDetectorTool ]

# Deal with SCT alignment conditions folders and algorithms

    conddb.addFolderSplitOnline("SCT","/Tracker/Onl/Align","/Tracker/Align",className="AlignableTransformContainer")
    # print("Override Alignment dbname to OFLP200, fix this when alignment available in CONDBR3")
    # conddb.addFolder("/Tracker/Align", "SCT_OFL",className="AlignableTransformContainer")
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    if not hasattr(condSeq, "FaserSCT_AlignCondAlg"):
        from FaserSCT_ConditionsAlgorithms.FaserSCT_ConditionsAlgorithmsConf import FaserSCT_AlignCondAlg
        condSeq += FaserSCT_AlignCondAlg(name = "FaserSCT_AlignCondAlg",
                                        UseDynamicAlignFolders =  False)
    if not hasattr(condSeq, "FaserSCT_DetectorElementCondAlg"):
        from FaserSCT_ConditionsAlgorithms.FaserSCT_ConditionsAlgorithmsConf import FaserSCT_DetectorElementCondAlg
        condSeq += FaserSCT_DetectorElementCondAlg(name = "FaserSCT_DetectorElementCondAlg")


    from DipoleGeoModel.DipoleGeoModelConf import DipoleTool
    dipoleTool = DipoleTool( RDBAccessSvc = "RDBAccessSvc",
                             GeometryDBSvc = "TrackerGeometryDBSvc",
                             GeoDbTagSvc = "GeoDbTagSvc")
    geoModelSvc.DetectorTools += [ dipoleTool ]

    from EcalGeoModel.EcalGeoModelConf import EcalDetectorTool
    ecalDetectorTool = EcalDetectorTool( DetectorName = "Ecal",
                                         Alignable = True,
                                         RDBAccessSvc = "RDBAccessSvc",
                                         GeometryDBSvc = "CaloGeometryDBSvc",
                                         GeoDbTagSvc = "GeoDbTagSvc")

    geoModelSvc.DetectorTools += [ ecalDetectorTool ]

    if "FASERNU-03" in DDversion or "FASERNU-04" in DDversion:
        from TrenchGeoModel.TrenchGeoModelConf import TrenchDetectorTool
        trenchDetectorTool = TrenchDetectorTool( )

        geoModelSvc.DetectorTools += [ trenchDetectorTool ]


    pass

## setup GeoModel at module import
_setupGeoModel()

## clean-up: avoid running multiple times this method
del _setupGeoModel
