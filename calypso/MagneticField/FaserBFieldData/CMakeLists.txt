################################################################################
# Package: FaserBFieldData
################################################################################
option( INSTALL_BFIELD "Generate and install a field map" OFF )

# Declare the package name:
atlas_subdir( FaserBFieldData )

find_package( ROOT REQUIRED COMPONENTS Core )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/share/doTranslation.C.in ${CMAKE_CURRENT_BINARY_DIR}/share/doTranslation.C)

if (INSTALL_BFIELD)
   add_custom_command (
      OUTPUT ${CMAKE_BINARY_DIR}/data/MagneticFieldMaps/FaserFieldTable.root
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/FieldTable.csv ${CMAKE_CURRENT_SOURCE_DIR}/share/*.C ${CMAKE_CURRENT_SOURCE_DIR}/share/*.in
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/share
      COMMAND mkdir -p ${CMAKE_BINARY_DIR}/data/MagneticFieldMaps
      COMMAND ${ROOTSYS}/bin/root -l -b -q -e '.x doTranslation.C'
      )

   add_custom_target( FieldTable ALL DEPENDS ${CMAKE_BINARY_DIR}/data/MagneticFieldMaps/FaserFieldTable.root )

   # Install the generated file:
   install( FILES ${CMAKE_BINARY_DIR}/data/MagneticFieldMaps/FaserFieldTable.root
            DESTINATION ${CMAKE_INSTALL_DATADIR}/MagneticFieldMaps )
endif()

