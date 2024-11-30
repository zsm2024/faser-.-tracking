################################################################################
# Package: G4FaserUserActions
################################################################################

# Declare the package name:
atlas_subdir( G4FaserUserActions )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( TBB )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library(   G4FaserUserActions
                     src/*.cxx
                     src/components/*.cxx
                     OBJECT
                     NO_PUBLIC_HEADERS
                     PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                     PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel G4AtlasInterfaces G4AtlasToolsLib EventInfo TrackerSimEvent ScintSimEvent FaserMCTruth FaserMCTruthBaseLib TruthUtils SimHelpers PathResolver G4DebuggingHelperLib)

# Install files from the package:
atlas_install_python_modules( python/*.py )
#atlas_install_joboptions( share/*.py )
#atlas_install_runtime( share/*.dat )
