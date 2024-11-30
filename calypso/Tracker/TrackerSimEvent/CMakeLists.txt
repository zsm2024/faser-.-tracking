################################################################################
# Package: TrackerSimEvent
################################################################################

# Declare the package name:
atlas_subdir( TrackerSimEvent )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrackerSimEvent
                   src/*.cxx src/*.h
                   PUBLIC_HEADERS TrackerSimEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthAllocators AthenaKernel CxxUtils GeneratorObjects HitManagement StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrackerIdentifier )

atlas_add_dictionary( TrackerSimEventDict
                      TrackerSimEvent/TrackerSimEventDict.h
                      TrackerSimEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthAllocators CxxUtils GeneratorObjects HitManagement StoreGateLib SGtests TrackerIdentifier TrackerSimEvent )

