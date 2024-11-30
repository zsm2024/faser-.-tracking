# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool
# add tools and algorithms to the ConfiguredFactory, see:
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConfiguredFactory
addTool("FaserSCT_GeoModel.FaserSCT_GeoModelConfig.FasergetSCT_DetectorTool","FaserSCT_DetectorTool")
