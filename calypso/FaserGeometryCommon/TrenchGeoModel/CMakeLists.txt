################################################################################
# Package: TrenchGeoModel
################################################################################

# Declare the package name:
atlas_subdir( TrenchGeoModel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( GeoModel COMPONENTS GeoModelKernel GeoModelDBManager GDMLtoGM )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( TrenchGeoModel
		             src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIR} ${XERCESC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${GEOMODEL_LIBRARIES} ${XERCESC_LIBRARIES} AthenaKernel GeoModelFaserUtilities GaudiKernel 
                                    StoreGateLib GeoModelInterfaces RDBAccessSvcLib PathResolver)

add_custom_command (OUTPUT ${CMAKE_XML_OUTPUT_DIRECTORY}/TrenchGeoModel/Trench.gdml 
                    COMMAND mkdir -p ${CMAKE_XML_OUTPUT_DIRECTORY}/TrenchGeoModel/   
		       COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/data/Trench.gdml ${CMAKE_XML_OUTPUT_DIRECTORY}/TrenchGeoModel/Trench.gdml )
add_custom_target (make_cavern_gdml ALL DEPENDS  ${CMAKE_XML_OUTPUT_DIRECTORY}/TrenchGeoModel/Trench.gdml)
get_filename_component( _realpath ${CMAKE_CURRENT_SOURCE_DIR}/data/Trench.gdml REALPATH )

install(FILES ${_realpath} DESTINATION ${CMAKE_INSTALL_PREFIX}/XML/TrenchGeoModel RENAME Trench.gdml)
unset( _realpath )

# Install files from the package:
#atlas_install_python_modules( python/*.py )
#atlas_install_scripts( test/*.py )
# Not needed as done with symlink above
#atlas_install_xmls( data/*.gdml )
