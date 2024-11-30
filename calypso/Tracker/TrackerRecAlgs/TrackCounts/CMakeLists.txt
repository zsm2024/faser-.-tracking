################################################################################
# Package: TrackerClusterFIt
################################################################################

# Declare the package name:
atlas_subdir( TrackCounts )

# Component(s) in the package:
atlas_add_component( TrackCounts
        src/*.cxx src/*.h
        src/components/*.cxx
        LINK_LIBRARIES AthenaBaseComps StoreGateLib TrkTrack TrackerIdentifier TrackerPrepRawData TrkRIO_OnTrack TrackerRIO_OnTrack
        )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )
