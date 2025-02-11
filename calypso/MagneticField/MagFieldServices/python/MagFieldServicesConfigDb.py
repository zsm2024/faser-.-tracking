# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# database entries for https://twiki.cern.ch/twiki/bin/view/AtlasComputing/ConfiguredFactory#Factory_functions_vs_derived_cla
# Valerio Ippolito - Harvard University
from AthenaCommon.CfgGetter import addService, addAlgorithm

addService('MagFieldServices.MagFieldServicesConfig.GetFieldSvc', 'FaserFieldSvc')
addAlgorithm('MagFieldServices.MagFieldServicesConfig.GetFieldMapCondAlg', 'FaserFieldMapCondAlg')
addAlgorithm('MagFieldServices.MagFieldServicesConfig.GetFieldCacheCondAlg', 'FaserFieldCacheCondAlg')
