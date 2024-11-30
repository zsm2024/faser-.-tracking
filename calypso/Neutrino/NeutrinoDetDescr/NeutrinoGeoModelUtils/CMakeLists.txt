################################################################################
# Package: NeutrinoGeoModelUtils
################################################################################

# Declare the package name:
atlas_subdir( NeutrinoGeoModelUtils )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( GeoModel )

# Component(s) in the package:
atlas_add_library( NeutrinoGeoModelUtils
                   src/*.cxx
                   PUBLIC_HEADERS NeutrinoGeoModelUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaKernel GaudiKernel StoreGateLib SGtests GeoPrimitives GeometryDBSvcLib
                   PRIVATE_LINK_LIBRARIES GeoModelFaserUtilities )

