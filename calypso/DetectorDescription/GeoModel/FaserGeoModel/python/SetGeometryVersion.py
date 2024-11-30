# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

from AthenaCommon.JobProperties import jobproperties
#from AthenaCommon.DetFlags    import DetFlags

DDversion    = jobproperties.Global.DetDescrVersion()

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc

GeoModelSvc = GeoModelSvc()
DetDescrCnvSvc = DetDescrCnvSvc()

GeoModelSvc.FaserVersion = DDversion

# Set the SupportedGeometry flag
from AthenaCommon.AppMgr import release_metadata
rel_metadata = release_metadata()
relversion = rel_metadata['release'].split('.')
if len(relversion) < 3:
    relversion = rel_metadata['base release'].split('.')

print("SetGeometryVersion.py obtained major release version ", relversion[0])
GeoModelSvc.SupportedGeometry = int(relversion[0])
