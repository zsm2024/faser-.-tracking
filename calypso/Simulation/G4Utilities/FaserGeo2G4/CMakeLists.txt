################################################################################
# Package: FaserGeo2G4
################################################################################

# Declare the package name:
atlas_subdir( FaserGeo2G4 )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS MathCore RIO Core Tree Hist pthread )
find_package( XercesC )
find_package( GeoModel COMPONENTS GeoModelKernel)

# Component(s) in the package:
atlas_add_library( FaserGeo2G4Lib
                   src/*.cxx
                   OBJECT
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${GEOMODEL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel G4FaserInterfaces G4AtlasInterfaces G4AtlasToolsLib GeoModelFaserUtilities GeoPrimitives
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES}  CxxUtils GeoSpecialShapes  SimHelpers FaserGeoMaterial2G4 AthenaKernel GeoModelInterfaces StoreGateLib )

atlas_add_library( FaserGeo2G4
                     src/components/*.cxx
                     OBJECT
                     NO_PUBLIC_HEADERS
                     PRIVATE_LINK_LIBRARIES FaserGeo2G4Lib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )


