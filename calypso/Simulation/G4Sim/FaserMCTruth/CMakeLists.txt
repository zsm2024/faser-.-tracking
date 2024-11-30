################################################################################
# Package: FaserMCTruth
################################################################################

# Declare the package name:
atlas_subdir( FaserMCTruth )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( FaserMCTruth
                   src/*.cxx
                   OBJECT
                   PUBLIC_HEADERS FaserMCTruth
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaKernel GeneratorObjects TruthUtils
                   PRIVATE_LINK_LIBRARIES ISF_Event FaserISF_Event SimHelpers )
