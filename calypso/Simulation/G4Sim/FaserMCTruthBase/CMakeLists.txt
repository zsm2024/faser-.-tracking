################################################################################
# Package: FaserMCTruthBase
################################################################################

# Declare the package name:
atlas_subdir( FaserMCTruthBase )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( FaserMCTruthBaseLib
                   src/*.cxx
                   OBJECT
                   PUBLIC_HEADERS FaserMCTruthBase
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaKernel GaudiKernel StoreGateLib G4AtlasToolsLib CxxUtils
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps
                   G4AtlasInterfaces FaserMCTruth SimHelpers FaserISF_InterfacesLib ISF_InterfacesLib 
                   FaserDetDescr FaserISF_Geant4Event
                 )


# atlas_add_library(FaserMCTruthBase
#                  src/components/*.cxx
#                  OBJECT
#                  NO_PUBLIC_HEADERS
#                  PRIVATE_LINK_LIBRARIES FaserMCTruthBaseLib)


#atlas_add_component( FaserMCTruthBase
#                     src/components/*.cxx
#                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
#                     LINK_LIBRARIES ${GEANT4_LIBRARIES}
#                     ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES}
#                     AtlasHepMCLib AthenaKernel GaudiKernel
#                     AthenaBaseComps StoreGateLib SGtests
#                     G4AtlasInterfaces G4AtlasToolsLib FaserMCTruth
#                     SimHelpers ISF_InterfacesLib FaserISF_InterfacesLib
#                     FaserDetDescr FaserMCTruthBaseLib
#                     FaserISF_Geant4Event )

# Install files from the package:
#atlas_install_python_modules( python/*.py )
