# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from CalypsoConfiguration.AutoConfigFlags import GetFileMD, DetDescrInfo

def createGeoModelConfigFlags():
    gcf=AthConfigFlags()
    gcf.addFlag("GeoModel.FaserVersion", lambda prevFlags : ((prevFlags.Input.Files and
                                         prevFlags.Input.Files != ["_FASER_GENERIC_INPUTFILE_NAME_"] and
                                         GetFileMD(prevFlags.Input.Files).get("GeoFaser",None))
                                         or "FASER-01"))
    gcf.addFlag("GeoModel.GeoExportFile","")
    gcf.addFlag("GeoModel.Align.Disable", False)          # Hack to avoid loading alignment when we want to create it from scratch


    return gcf

def modifyGeoModelConfigFlags(flags):
    # print("modifyGeoModelConfigFlags called for FaserVersion: ", flags.GeoModel.FaserVersion)
    flags.GeoModel.Run = lambda prevFlags : DetDescrInfo(prevFlags.GeoModel.FaserVersion)['Common']['Run']
    flags.GeoModel.Align.Dynamic = False
    # print(DetDescrInfo(flags.GeoModel.FaserVersion)['Common']['Run'])
    return flags
 