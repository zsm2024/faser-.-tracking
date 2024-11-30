################################################################################
# Package: FaserSCT_ConditionsData
################################################################################

option( INSTALL_CONDB "Generate and install a conditions database" OFF )
# Declare the package name:
atlas_subdir( FaserSCT_ConditionsData )

# Component(s) in the package:
atlas_add_library( FaserSCT_ConditionsData
                   FaserSCT_ConditionsData/*.h
                   src/*.cxx
                   PUBLIC_HEADERS FaserSCT_ConditionsData
                   LINK_LIBRARIES AthenaPoolUtilities Identifier )

if (INSTALL_CONDB)
   add_custom_command (
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ALLP200.db
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/SCT_Conditions.py
      COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/data/SCT_Conditions.py
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      )

   add_custom_target( ALLP200 ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ALLP200.db )

   # Install the generated file:
   install( FILES ${CMAKE_CURRENT_BINARY_DIR}/ALLP200.db
            DESTINATION ${CMAKE_INSTALL_DATADIR}/sqlite200 
         )
endif()