################################################################################
# Package: WriteAlignment
################################################################################

# Declare the package name:
atlas_subdir( WriteAlignment )

# External dependencies:
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( WriteAlignment
                     src/WriteAlignmentAlg.cxx
                     src/components/WriteAlignment_entries.cxx
                     INCLUDE_DIRS ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaBaseComps GeoModelUtilities ScintReadoutGeometry TrackerReadoutGeometry TrackerAlignGenToolsLib )

# atlas_add_test( WriteAlignmentCheck
#                 SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/python/WriteAlignmentConfig.py
#                 PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#                 PROPERTIES TIMEOUT 300 )

# Install files from the package:
#atlas_install_headers( WriteAlignment )
#atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/*.sh )
