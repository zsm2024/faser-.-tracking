################################################################################
# Package: G4FaserAlg
################################################################################

# Declare the package name:
atlas_subdir( G4FaserAlg )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( Eigen )

# G4FaserAlgLib library

atlas_add_library( G4FaserAlgLib
                     src/*.cxx
                     OBJECT
                     PUBLIC_HEADERS G4FaserAlg
                     NO_PUBLIC_HEADERS
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel CxxUtils GaudiKernel G4AtlasInterfaces SGTools StoreGateLib EventInfo GeneratorObjects GeoModelInterfaces HepMC_InterfacesLib FaserISF_InterfacesLib ISF_InterfacesLib FaserMCTruthBaseLib )

# Component(s) in the package:
atlas_add_library(   G4FaserAlg
                     src/components/*.cxx
                     OBJECT
                     NO_PUBLIC_HEADERS
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     PRIVATE_LINK_LIBRARIES ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel CxxUtils GaudiKernel G4AtlasInterfaces G4FaserAlgLib SGTools StoreGateLib EventInfo GeneratorObjects FaserMCTruthBaseLib)

atlas_add_test( G4FaserAlgConfig_TestFaser
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/G4FaserAlgConfig_Test.py GeoModel.FaserVersion="'FASER-01'" IOVDb.GlobalTag="'OFLCOND-FASER-01'" Output.HITSFileName='faser.HITS.pool.root'
                PROPERTIES TIMEOUT 300 
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

atlas_add_test( G4FaserAlgConfig_TestFaserNu03
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/G4FaserAlgConfig_Test.py GeoModel.FaserVersion="'FASERNU-03'" IOVDb.GlobalTag="'OFLCOND-FASER-02'" Output.HITSFileName='faserNu03.HITS.pool.root'
                PROPERTIES TIMEOUT 300 
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

atlas_add_test( G4FaserAlgConfig_TestFaserNu03_NewField
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/G4FaserAlgConfig_Test.py GeoModel.FaserVersion="'FASERNU-03'" IOVDb.GlobalTag="'OFLCOND-FASER-03'" Output.HITSFileName='faserNu03_NewField.HITS.pool.root'
                PROPERTIES TIMEOUT 300 
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

atlas_add_test( G4FaserAlgConfig_TestFaserNu04
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/G4FaserAlgConfig_Test.py GeoModel.FaserVersion="'FASERNU-04'" IOVDb.GlobalTag="'OFLCOND-FASER-03'" Output.HITSFileName='faserNu04.HITS.pool.root'
                PROPERTIES TIMEOUT 300 
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

atlas_add_test( G4FaserAlgConfig_TestTestbeam00
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/G4FaserAlgConfig_Test.py GeoModel.FaserVersion="'FASER-TB00'" IOVDb.GlobalTag="'OFLCOND-FASER-TB00'" Output.HITSFileName='tb00.HITS.pool.root'
                PROPERTIES TIMEOUT 300 
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

atlas_add_test( G4FaserAlgConfig_TestTestbeam01
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/G4FaserAlgConfig_Test.py GeoModel.FaserVersion="'FASER-TB01'" IOVDb.GlobalTag="'OFLCOND-FASER-TB00'" Output.HITSFileName='tb01.HITS.pool.root'
                PROPERTIES TIMEOUT 300 
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR})


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )
