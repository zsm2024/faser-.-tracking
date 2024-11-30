###############################################################################
# Package: FaserSCT_ConditionsTools
################################################################################

# Declare the package name:
atlas_subdir( FaserSCT_ConditionsTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( GTest )
find_package( GMock )

# Component(s) in the package:
atlas_add_component ( FaserSCT_ConditionsTools
                      FaserSCT_ConditionsTools/*.h
                      src/components/*.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel FaserSCT_ConditionsToolsLib Identifier GeoModelUtilities GeoModelFaserUtilities GaudiKernel AthenaBaseComps StoreGateLib SGtests xAODEventInfo FaserSCT_ConditionsData InDetByteStreamErrors TrackerIdentifier TrackerReadoutGeometry FaserSiPropertiesToolLib InDetConditionsSummaryService )


atlas_add_library( FaserSCT_ConditionsToolsLib
                  FaserSCT_ConditionsTools/*.h
                  src/*.h
                  src/*.cxx
                  PUBLIC_HEADERS FaserSCT_ConditionsTools
                  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                  LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel Identifier GeoModelUtilities GeoModelFaserUtilities GaudiKernel AthenaBaseComps StoreGateLib SGtests xAODEventInfo FaserSCT_ConditionsData FaserSiPropertiesToolLib InDetByteStreamErrors TrackerIdentifier TrackerReadoutGeometry SCT_CablingLib EventContainers InDetConditionsSummaryService )

# Add unit tests
#atlas_add_test( SCT_RODVetoTool_test
#                SOURCES test/SCT_RODVetoTool_test.cxx
#                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}  ${GMOCK_INCLUDE_DIRS}
#                LINK_LIBRARIES TestTools SCT_ConditionsToolsLib ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES}
#                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

#atlas_add_test( SCT_ConditionsConfig_test
#                SCRIPT test/SCT_ConditionsConfig_test.py
#                PROPERTIES TIMEOUT 300 )

# Install files from the package:
#atlas_install_headers( FaserSCT_ConditionsTools )
atlas_install_python_modules( python/*.py )
#atlas_install_scripts( share/*.py )

