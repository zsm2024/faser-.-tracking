################################################################################
# Package: G4FaserTools
################################################################################

# Declare the package name:
atlas_subdir( G4FaserTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Geant4 )
find_package( TBB )
find_package( XercesC )

# Component(s) in the package:
#atlas_add_library( G4FaserToolsLib
#                   src/*.cxx
#                   src/*.cc
#                   PUBLIC_HEADERS G4FaserTools
#                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
#                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
#                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
#                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps G4AtlasInterfaces
#                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel G4PhysicsLists )

#atlas_add_component( G4FaserTools
#                     src/components/*.cxx
#                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
#                     LINK_LIBRARIES ${Boost_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps G4AtlasInterfaces GaudiKernel G4PhysicsLists G4FaserToolsLib )


# Install files from the package:
atlas_install_python_modules( python/*.py )
#atlas_install_scripts( test/*.py )

