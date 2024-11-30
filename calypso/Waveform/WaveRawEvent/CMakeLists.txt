################################################################################
# Package: WaveRawEvent
################################################################################

# Declare the package name:
atlas_subdir( WaveRawEvent )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( WaveRawEvent
                   src/*.cxx
                   PUBLIC_HEADERS WaveRawEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthAllocators AthenaKernel Identifier CxxUtils StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ScintIdentifier EventFormats )

atlas_add_dictionary( WaveRawEventDict
                      WaveRawEvent/WaveRawEventDict.h
                      WaveRawEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthAllocators Identifier CxxUtils StoreGateLib ScintIdentifier WaveRawEvent )

