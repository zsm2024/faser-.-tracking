
# Declare the package name:
atlas_subdir( ActsInterop )

# External dependencies:

#set( Acts_DIR /home/tboeckh/Documents/acts/run )
#find_package( Acts REQUIRED COMPONENTS Core PATHS /home/tboeckh/Documents/acts/run NO_DEFAULT_PATH )
find_package( Acts COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( ActsInteropLib
                   src/*.cxx
                   PUBLIC_HEADERS ActsInterop
                   LINK_LIBRARIES 
                   AthenaBaseComps
                   Identifier
                   FaserDetDescr
                   TrackerIdentifier 
                   TrackerReadoutGeometry
                   ActsCore)


