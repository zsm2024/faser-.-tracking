################################################################################
# Package: CaloGeoModelUtils
################################################################################

# Declare the package name:
atlas_subdir( CaloGeoModelUtils )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( GeoModel )

# Component(s) in the package:
atlas_add_library( CaloGeoModelUtils
                   src/*.cxx
                   PUBLIC_HEADERS CaloGeoModelUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaKernel GaudiKernel RDBAccessSvcLib StoreGateLib SGtests GeoModelFaserUtilities GeoModelInterfaces GeoPrimitives GeometryDBSvcLib
                   PRIVATE_LINK_LIBRARIES GeoModelFaserUtilities )

