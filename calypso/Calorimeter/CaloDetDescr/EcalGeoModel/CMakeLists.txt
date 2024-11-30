################################################################################
# Package: EcalGeoModel
################################################################################

# Option to chose geo with or without tyvek paper
option( ECAL_GEO_TYVEK "Apply Tyvek in Ecal geometry" ON )

# Declare the package name:
atlas_subdir( EcalGeoModel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( GeoModel COMPONENTS GeoModelKernel GeoModelDBManager GDMLtoGM )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( EcalGeoModel
		     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${GEOMODEL_INCLUDE_DIR} ${XERCESC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${GEOMODEL_LIBRARIES} ${XERCESC_LIBRARIES} AthenaKernel GeoModelFaserUtilities GaudiKernel 
                                    CaloGeoModelUtils CaloReadoutGeometry SGTools StoreGateLib SGtests AthenaPoolUtilities DetDescrConditions Identifier FaserCaloIdentifier PathResolver )

atlas_add_test( EcalGMConfig_test
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/EcalGMConfig_test.py
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                PROPERTIES TIMEOUT 300 )

# Decide which ecal gdml file to use based on option; tyvek density reduced in EcalTyvek04.gdml (June 2023)
if (ECAL_GEO_TYVEK)
   add_custom_command (OUTPUT ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/Ecal.gdml 
                       COMMAND mkdir -p ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/
		                 COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/data/EcalTyvek04.gdml ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/Ecal.gdml )
   add_custom_command (OUTPUT ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/Ecal04.gdml 
                       COMMAND mkdir -p ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/
                       COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/data/EcalTyvek04.gdml ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/Ecal04.gdml )
   add_custom_target (make_ecal_gdml ALL DEPENDS  ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/Ecal.gdml)
   add_custom_target (make_ecal_gdml04 ALL DEPENDS  ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/Ecal04.gdml)
   get_filename_component( _realpath ${CMAKE_CURRENT_SOURCE_DIR}/data/EcalTyvek.gdml REALPATH )
   get_filename_component( _realpath04 ${CMAKE_CURRENT_SOURCE_DIR}/data/EcalTyvek04.gdml REALPATH )
else() 
   add_custom_command (OUTPUT ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/Ecal.gdml 
                       COMMAND mkdir -p ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/   
		                 COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/data/EcalNoTyvek.gdml ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/Ecal.gdml )
   add_custom_command (OUTPUT ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/Ecal04.gdml 
                       COMMAND mkdir -p ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/   
		                 COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/data/EcalNoTyvek.gdml ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/Ecal04.gdml )
   add_custom_target (make_ecal_gdml ALL DEPENDS  ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/Ecal.gdml)
   add_custom_target (make_ecal_gdml04 ALL DEPENDS  ${CMAKE_XML_OUTPUT_DIRECTORY}/EcalGeoModel/Ecal04.gdml)
   get_filename_component( _realpath ${CMAKE_CURRENT_SOURCE_DIR}/data/EcalNoTyvek.gdml REALPATH )
   get_filename_component( _realpath04 ${CMAKE_CURRENT_SOURCE_DIR}/data/EcalNoTyvek.gdml REALPATH )
endif()

install(FILES ${_realpath} DESTINATION ${CMAKE_INSTALL_PREFIX}/XML/EcalGeoModel RENAME Ecal.gdml)
install(FILES ${_realpath04} DESTINATION ${CMAKE_INSTALL_PREFIX}/XML/EcalGeoModel RENAME Ecal04.gdml)
unset( _realpath )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )
# Not needed as done with symlink above
#atlas_install_xmls( data/*.gdml )
