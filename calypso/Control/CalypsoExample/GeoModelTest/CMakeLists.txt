################################################################################
# Package: GeoModelTest
################################################################################

# Declare the package name:
atlas_subdir( GeoModelTest )

# External dependencies:
find_package( GeoModel )

# Component(s) in the package:
atlas_add_component( GeoModelTest
                     src/GeoModelTestAlg.cxx
                     src/components/GeoModelTest_entries.cxx
                     INCLUDE_DIRS ${GEOMODEL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEOMODEL_LIBRARIES} AthenaBaseComps GeoModelFaserUtilities ScintReadoutGeometry TrackerReadoutGeometry MagFieldInterfaces MagFieldElements MagFieldConditions )

atlas_add_test( Faser01GeoCheck
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/python/Faser01TestConfig.py
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                PROPERTIES TIMEOUT 300 )

atlas_add_test( Faser02GeoCheck
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/python/Faser02TestConfig.py
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                PROPERTIES TIMEOUT 300 )

atlas_add_test( Faser03GeoCheck
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/python/Faser03TestConfig.py
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                PROPERTIES TIMEOUT 300 )

atlas_add_test( Faser04GeoCheck
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/python/Faser04TestConfig.py
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                PROPERTIES TIMEOUT 300 )

atlas_add_test( TestBeam00GeoCheck
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/python/TestBeam00TestConfig.py
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                PROPERTIES TIMEOUT 300 )

atlas_add_test( TestBeam01GeoCheck
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/python/TestBeam01TestConfig.py
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                PROPERTIES TIMEOUT 300 )
                
# Install files from the package:
#atlas_install_headers( GeoModelTest )
#atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
#atlas_install_scripts( scripts/*.sh )
