# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS and FASER collaborations

# Declare the package name:
atlas_subdir( TrackerRIO_OnTrack )

# Component(s) in the package:
atlas_add_library( TrackerRIO_OnTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrackerRIO_OnTrack
                   LINK_LIBRARIES AthLinks AthenaKernel CxxUtils Identifier GaudiKernel TrackerPrepRawData TrkEventPrimitives TrkRIO_OnTrack
                   PRIVATE_LINK_LIBRARIES TrackerIdentifier TrackerReadoutGeometry TrkSurfaces )

atlas_add_dictionary( TrackerRIO_OnTrackDict
                      TrackerRIO_OnTrack/TrackerRIO_OnTrackDict.h
                      TrackerRIO_OnTrack/selection.xml
                      LINK_LIBRARIES TrackerRIO_OnTrack )
