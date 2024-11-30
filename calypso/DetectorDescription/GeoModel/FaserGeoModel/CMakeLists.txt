###############################
# Package: FaserGeoModel
###############################
option( INSTALL_GEOMDB "Generate and install a geometry database" OFF )

# Declare package name
atlas_subdir( FaserGeoModel )

if (INSTALL_GEOMDB)
add_custom_command (
   OUTPUT ${CMAKE_BINARY_DIR}/data/geomDB/geomDB_sqlite
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/geomDB.sql
   COMMAND mkdir -p ${CMAKE_BINARY_DIR}/data/geomDB
   COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/data/geomDB.sql | sqlite3 ${CMAKE_BINARY_DIR}/data/geomDB/geomDB_sqlite
   )

   add_custom_target( geomDB ALL DEPENDS ${CMAKE_BINARY_DIR}/data/geomDB/geomDB_sqlite )

   # Install the generated file:
   install( FILES ${CMAKE_BINARY_DIR}/data/geomDB/geomDB_sqlite
            DESTINATION ${CMAKE_INSTALL_DATADIR}/geomDB/ )
endif()

# Install python files from the package:
atlas_install_python_modules( python/*.py )

# Install xml files from the package:
atlas_install_xmls( data/*.xml data/*.dtd )

atlas_install_scripts( test/*.py )

atlas_add_test( EVNT_InputGeo_test
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/FaserGeometryConfig_EVNT_test.py
                PROPERTIES TIMEOUT 300 
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )
