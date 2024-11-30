# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def InitializeGeometryParameters(dbGeomCursor):
   """Read version name, layout and fasernu from FaserCommon table.

   dbGeomCursor: FaserGeoDBInterface instance
   """

   dbId, dbCommon, dbParam = dbGeomCursor.GetCurrentLeafContent("FaserCommon")

   params = {"Run" : "UNDEFINED",
             "TrackerType" : "UNDEFINED",
             "FaserNuType" : "UNDEFINED"}

   if len(dbId)>0:
      key = dbId[0]
      if "CONFIG" in dbParam :
         params["Run"] = dbCommon[key][dbParam.index("CONFIG")]
      if "TRACKERTYPE" in dbParam :
         params["TrackerType"] = dbCommon[key][dbParam.index("TRACKERTYPE")]
      if "FASERNUTYPE" in dbParam :
         params["FaserNuType"] = dbCommon[key][dbParam.index("FASERNUTYPE")]

   return params
