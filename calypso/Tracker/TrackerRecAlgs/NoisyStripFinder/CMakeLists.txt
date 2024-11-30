################################################################################
# Package: NoisyStripFinder
################################################################################

# Declare the package name:
atlas_subdir( NoisyStripFinder )

# Component(s) in the package:
atlas_add_component( NoisyStripFinder
                     src/*.cxx src/*.h
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests Identifier GaudiKernel TrackerRawData TrackerPrepRawData FaserDetDescr TrackerIdentifier 
                                    TrackerReadoutGeometry AthViews FaserSCT_ConditionsData xAODFaserTrigger )

# Install files from the package:
#atlas_install_headers( TrackerPrepRawDataFormation )

atlas_install_python_modules( python/*.py )

atlas_install_scripts( share/*.py share/*.sh )

