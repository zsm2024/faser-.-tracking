# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep, Project
from IOVDbSvc.IOVDbSvcConfig import addFolders #, addFoldersSplitOnline

def FaserSCT_GeometryCfg( flags ):
    from FaserGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc=acc.getPrimary()
    
    GeometryDBSvc = CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("TrackerGeometryDBSvc"))
    
    # from RDBAccessSvc.RDBAccessSvcConf import RDBAccessSvc
    # acc.addService(RDBAccessSvc("RDBAccessSvc"))

    # from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    # acc.addService(DBReplicaSvc("DBReplicaSvc"))

    FaserSCT_DetectorTool = CompFactory.FaserSCT_DetectorTool
    sctDetectorTool = FaserSCT_DetectorTool()

    sctDetectorTool.useDynamicAlignFolders = flags.GeoModel.Align.Dynamic
    geoModelSvc.DetectorTools += [ sctDetectorTool ]
    
    if flags.GeoModel.Align.Disable:
        return acc

    # Don't specify db= in addFolders calls below
    # dbname set globally to OFLP200 for MC and CONDBR3 for data
    # Now have proper data and MC DBs for the alignment
    if flags.GeoModel.Align.Dynamic:
        # acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL1/ID","/Indet/AlignL1/ID",className="CondAttrListCollection"))
        # acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL2/SCT","/Indet/AlignL2/SCT",className="CondAttrListCollection"))
        # acc.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/AlignL3","/Indet/AlignL3",className="AlignableTransformContainer"))
        print("FaserSCT dynamic align flag is not supported!")
    else:
        if flags.Common.Project != Project.AthSimulation and (flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay):
            acc.merge(addFolders(flags,"/Tracker/Align", "SCT_OFL", className="AlignableTransformContainer"))
        else:
            acc.merge(addFolders(flags, "/Tracker/Align", "SCT_OFL"))

    if flags.Common.Project != Project.AthSimulation: # Protection for AthSimulation builds
        if flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay:
            FaserSCT_AlignCondAlg = CompFactory.FaserSCT_AlignCondAlg
            sctAlignCondAlg = FaserSCT_AlignCondAlg(name = "FaserSCT_AlignCondAlg",
                                                    UseDynamicAlignFolders = flags.GeoModel.Align.Dynamic)
            acc.addCondAlgo(sctAlignCondAlg)
            FaserSCT_DetectorElementCondAlg = CompFactory.FaserSCT_DetectorElementCondAlg
            sctDetectorElementCondAlg = FaserSCT_DetectorElementCondAlg(name = "FaserSCT_DetectorElementCondAlg")
            acc.addCondAlgo(sctDetectorElementCondAlg)
    return acc
