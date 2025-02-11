###############################################################################
# Package: FaserSCT_ConditionsAlgorithms
################################################################################

# Declare the package name:
atlas_subdir( FaserSCT_ConditionsAlgorithms )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_component( FaserSCT_ConditionsAlgorithms
                     src/*.h
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests Identifier DetDescrConditions GeoModelFaserUtilities GaudiKernel FaserSCT_ConditionsData AthenaPoolUtilities TrackerIdentifier TrackerReadoutGeometry TrkGeometry FaserSCT_ConditionsToolsLib InDetConditionsSummaryService )

#atlas_add_test( TestCalibChipRead
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testCalibChipRead.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestCalibRead
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testCalibRead.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestConfig
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testConfig.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestDCSConditions
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testDCSConditions.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestLinkMasking
#                SCRIPT share/TestLinkMasking.sh
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestMajority
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testMajority.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestModuleVeto
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testModuleVeto.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestMonRead
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testMonRead.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestParameters
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testParameters.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestReadout
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testReadout.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestRodVeto
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testRodVeto.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestSensors
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testSensors.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestSilicon
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testSilicon.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestStripVeto
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testStripVeto.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestSummary
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testSummary.py
#                PROPERTIES TIMEOUT 600
#                ENVIRONMENT THREADS=5 )
#atlas_add_test( TestTdaqEnabled
#                SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/testTdaqEnabled.py
#                PROPERTIES TIMEOUT 720
#                ENVIRONMENT THREADS=5 )

# Install files from the package:
#atlas_install_joboptions( share/*.py )
#atlas_install_python_modules( python/*.py )
#atlas_install_scripts( share/*.sh )
