################################################################################
# Package: FaserSiPropertiesTool
################################################################################

# Declare the package name:
atlas_subdir( FaserSiPropertiesTool )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( FaserSiPropertiesToolLib
                   src/*.cxx
                   PUBLIC_HEADERS FaserSiPropertiesTool
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel TrackerReadoutGeometry FaserSCT_ConditionsData StoreGateLib SGtests InDetConditionsSummaryService
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} Identifier TrackerIdentifier )

atlas_add_component( FaserSiPropertiesTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} FaserSiPropertiesToolLib InDetConditionsSummaryService)

# Run tests:
#atlas_add_test( TestSCTProperties
#                SCRIPT athena.py --threads=5 SiPropertiesTool/testSCTProperties.py
#                PROPERTIES TIMEOUT 300
#                ENVIRONMENT THREADS=5 )

#atlas_add_test( SiPropertiesConfig_test
#                SCRIPT test/SiPropertiesConfig_test.py
#                PROPERTIES TIMEOUT 300 )

# Install files from the package:
atlas_install_python_modules( python/*.py )
#atlas_install_joboptions( share/*.py )

