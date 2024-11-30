################################################################################
# Package: FaserCaloSimEvent
################################################################################

# Declare the package name:
atlas_subdir( FaserCaloSimEvent )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( FaserCaloSimEvent
                   src/*.cxx
                   PUBLIC_HEADERS FaserCaloSimEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthAllocators AthenaKernel CxxUtils GeneratorObjects HitManagement StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} FaserCaloIdentifier Identifier)

atlas_add_dictionary( FaserCaloSimEventDict
                      FaserCaloSimEvent/CaloSimEventDict.h
                      FaserCaloSimEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthAllocators CxxUtils GeneratorObjects HitManagement StoreGateLib SGtests FaserCaloIdentifier FaserCaloSimEvent Identifier)

