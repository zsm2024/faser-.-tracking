###############################################################################
# Package: WaveformConditionsTools
################################################################################

# Declare the package name:
atlas_subdir( WaveformConditionsTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component ( WaveformConditionsTools
                      src/components/*.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel WaveformConditionsToolsLib Identifier ScintIdentifier FaserCaloIdentifier GaudiKernel AthenaBaseComps AthenaPoolUtilities StoreGateLib xAODEventInfo )


atlas_add_library( WaveformConditionsToolsLib
                  src/*.cxx
                  PUBLIC_HEADERS WaveformConditionsTools
                  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                  LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel Identifier ScintIdentifier FaserCaloIdentifier  GaudiKernel AthenaBaseComps AthenaPoolUtilities StoreGateLib xAODEventInfo )

# Install files from the package:
atlas_install_python_modules( python/*.py )

