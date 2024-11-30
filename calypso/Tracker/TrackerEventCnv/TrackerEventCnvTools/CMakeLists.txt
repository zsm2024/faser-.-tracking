# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackerEventCnvTools )

# Component(s) in the package:
atlas_add_component( TrackerEventCnvTools
                     src/*.cxx TrackerEventCnvTools/*h
                     src/components/*.cxx
		     LINK_LIBRARIES AthenaBaseComps FaserDetDescr IdDictDetDescr Identifier TrackerIdentifier TrackerReadoutGeometry TrackerPrepRawData TrackerSpacePoint TrackerRIO_OnTrack StoreGateLib TrkPrepRawData TrkRIO_OnTrack TrkEventCnvToolsLib )
