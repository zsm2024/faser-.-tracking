################################################################################
# Package: PreshowerG4_SD
################################################################################

# Declare the package name:
atlas_subdir( PreshowerG4_SD )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( GeoModel COMPONENTS GeoModelKernel GeoModelRead GeoModelDBManager )

# Component(s) in the package:
atlas_add_library( PreshowerG4_SDLib
                   src/*.cxx
                   OBJECT
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${GEANT4_LIBRARIES} ${GEOMODEL_LIBRARIES} G4AtlasToolsLib ScintSimEvent FaserMCTruth StoreGateLib GeoModelInterfaces GeoPrimitives )

atlas_add_library( PreshowerG4_SD
                   src/components/*.cxx
                   OBJECT
                   NO_PUBLIC_HEADERS
                   PRIVATE_LINK_LIBRARIES PreshowerG4_SDLib )

# Component(s) in the package:
# atlas_add_component( PreshowerG4_SD
#                      src/*.cxx
#                      src/components/*.cxx
#                      INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
#                      LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} StoreGateLib SGtests GaudiKernel ScintSimEvent G4AtlasToolsLib FaserMCTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py )

