################################################################################
# Package: FaserISF_Services
################################################################################

# Declare the package name:
atlas_subdir( FaserISF_Services )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Geant4 )
find_package( HepPDT )
#find_package( GTest )
#find_package( GMock )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
#find_package( GTest )
#find_package( GMock )

# Component(s) in the package:
atlas_add_library( FaserISF_Services
                     src/*.cxx
                     src/components/*.cxx
                     OBJECT
                     NO_PUBLIC_HEADERS
                     PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS}
                     PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${GEANT4_LIBRARIES} AtlasHepMCLib ${CLHEP_LIBRARIES} ${HEPPDT_LIBRARIES} GaudiKernel BarcodeInterfacesLib ScintSimEvent AthenaBaseComps StoreGateLib SGtests FaserDetDescr GeneratorObjects TrackerSimEvent G4AtlasInterfaces FaserMCTruth SimHelpers FaserISF_Event ISF_Event FaserISF_HepMC_Interfaces ISF_HepMC_Interfaces FaserISF_InterfacesLib ISF_InterfacesLib PmbCxxUtils TruthUtils )

#atlas_add_test( FaserTruthSvc_test
#                SOURCES
#                  test/FaserTruthSvc_test.cxx src/FaserTruthSvc.cxx
#                INCLUDE_DIRS
#                  ${GTEST_INCLUDE_DIRS}
#                  ${GMOCK_INCLUDE_DIRS}
#                  ${ROOT_INCLUDE_DIRS}
#                  ${GEANT4_INCLUDE_DIRS}
#                  ${CLHEP_INCLUDE_DIRS}
#                  ${HEPPDT_INCLUDE_DIRS}
#                LINK_LIBRARIES
#                  ${GTEST_LIBRARIES}
#                  ${GMOCK_LIBRARIES}
#                  ${ROOT_LIBRARIES}
#                  ${GEANT4_LIBRARIES}
#                  ${CLHEP_LIBRARIES}
#                  ${HEPPDT_LIBRARIES}
#                  GaudiKernel
#                  ScintSimEvent
#                  AthenaBaseComps
#                  StoreGateLib
#                  TrackerSimEvent
#                  FaserISF_Event
#                  ISF_Event
#                  FaserISF_Interfaces
#                  ISF_Interfaces
#                  PmbCxxUtils
#                  FaserMCTruth
#                  TruthUtils
#                ENVIRONMENT
#                  "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/test"
#                )

#test FaserISF_ServicesConfigNew
#atlas_add_test( FaserISF_ServicesConfigNew_test
#                SCRIPT test/FaserISF_ServicesConfigNew_test.py
#                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#                PROPERTIES TIMEOUT 300 )



# Needed for the plugin service to see the test components
# defined in the test binary.
#set_target_properties( FaserISF_Services_FaserTruthSvc_test PROPERTIES ENABLE_EXPORTS True )

# Install files from the package:
atlas_install_python_modules( python/*.py )

