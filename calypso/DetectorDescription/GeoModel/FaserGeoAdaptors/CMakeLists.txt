################################################################################
# Package: FaserGeoAdaptors
################################################################################

# Declare the package name:
atlas_subdir( FaserGeoAdaptors )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( FaserGeoAdaptors
                   src/*.cxx
                   PUBLIC_HEADERS FaserGeoAdaptors
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} Identifier NeutrinoIdentifier NeutrinoReadoutGeometry NeutrinoSimEvent 
                                                                FaserCaloIdentifier CaloReadoutGeometry FaserCaloSimEvent 
                                                                ScintIdentifier ScintReadoutGeometry ScintSimEvent 
                                                                TrackerIdentifier TrackerReadoutGeometry TrackerSimEvent StoreGateLib SGtests )

