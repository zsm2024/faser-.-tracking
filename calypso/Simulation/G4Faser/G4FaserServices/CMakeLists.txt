################################################################################
# Package: G4FaserServices
################################################################################

# Declare the package name:
atlas_subdir( G4FaserServices )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( TBB )
find_package( XercesC)
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( G4FaserServices
                   src/*.cxx
                   src/components/*.cxx
                   OBJECT
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}  ${ROOT_INCLUDE_DIRS} 
                   PRIVATE_LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} ${ROOT_LIBRARIES}
		     GaudiKernel AthenaBaseComps 
		     G4AtlasInterfaces G4AtlasToolsLib G4PhysicsLists PathResolver MagFieldElements MagFieldInterfaces)
set_target_properties( G4FaserServices PROPERTIES INTERPROCEDURAL_OPTIMIZATION ${ATLAS_GEANT4_USE_LTO} )

# Component(s) in the package:
#atlas_add_component( G4FaserServices
#                     src/*.cxx
#                     src/components/*.cxx
#                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
#                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} GaudiKernel AthenaBaseComps G4AtlasInterfaces G4AtlasToolsLib G4PhysicsLists MagFieldInterfaces )


# Install files from the package:
atlas_install_python_modules( python/*.py )
#atlas_install_scripts( test/*.py )
