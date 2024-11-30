################################################################################
# Package: VetoNuGeoModel
################################################################################

# Declare the package name:
atlas_subdir( VetoNuGeoModel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( GeoModel )

# Component(s) in the package:
atlas_add_component( VetoNuGeoModel
        		     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaKernel GeoModelFaserUtilities GaudiKernel SGTools StoreGateLib SGtests AthenaPoolUtilities DetDescrConditions FaserDetDescr ScintGeoModelUtils ScintReadoutGeometry ScintIdentifier Identifier )

atlas_add_test( VetoNuGMConfig_test
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/VetoNuGMConfig_test.py
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                PROPERTIES TIMEOUT 300 )


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )
