################################################################################
# Package: FaserGeoMaterial2G4
################################################################################

# Declare the package name:
atlas_subdir( FaserGeoMaterial2G4 )

# External dependencies:
find_package( Geant4 )
find_package( GeoModel COMPONENTS GeoModelKernel)

# Component(s) in the package:
atlas_add_library( FaserGeoMaterial2G4
                   src/*.cxx
                   OBJECT
                   PUBLIC_HEADERS FaserGeoMaterial2G4
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES}  ${GEOMODEL_LIBRARIES} AthenaBaseComps
                   PRIVATE_LINK_LIBRARIES CxxUtils GeoModelFaserUtilities )

