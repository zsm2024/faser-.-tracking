################################################################################
# Package: ScintReadoutGeometry
################################################################################

# Declare the package name:
atlas_subdir( ScintReadoutGeometry )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( GeoModel )

# Component(s) in the package:
atlas_add_library( ScintReadoutGeometry
                   src/*.cxx
                   PUBLIC_HEADERS ScintReadoutGeometry
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaKernel CxxUtils FaserDetDescr GeoModelFaserUtilities GeoPrimitives Identifier GaudiKernel ScintIdentifier TrkDetElementBase TrkSurfaces TrkEventPrimitives StoreGateLib SGtests AthenaBaseComps DetDescrConditions
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities IdDictDetDescr )

