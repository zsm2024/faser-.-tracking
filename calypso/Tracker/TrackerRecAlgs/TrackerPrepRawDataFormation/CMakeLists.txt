################################################################################
# Package: TrackerPrepRawDataFormation
################################################################################

# Declare the package name:
atlas_subdir( TrackerPrepRawDataFormation )

# Component(s) in the package:
atlas_add_component( TrackerPrepRawDataFormation
                     src/*.cxx src/*.h
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests Identifier GaudiKernel TrackerRawData TrackerPrepRawData FaserSiClusterizationToolLib FaserDetDescr TrackerIdentifier 
                                    TrkSpacePoint TrackerReadoutGeometry AthViews FaserSCT_ConditionsData InDetConditionsSummaryService )

# Install files from the package:
#atlas_install_headers( TrackerPrepRawDataFormation )

atlas_install_python_modules( python/*.py )

atlas_install_scripts( test/*.py )
