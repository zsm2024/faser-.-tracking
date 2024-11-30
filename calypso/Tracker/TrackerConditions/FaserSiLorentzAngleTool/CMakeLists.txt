################################################################################
# Package: FaserSiLorentzAngleTool
################################################################################

# Declare the package name:
atlas_subdir( FaserSiLorentzAngleTool )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( FaserSiLorentzAngleTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GeoPrimitives GaudiKernel FaserSiPropertiesToolLib MagFieldInterfaces AthenaPoolUtilities Identifier TrackerIdentifier TrackerReadoutGeometry MagFieldElements MagFieldConditions )

# Run tests:
#atlas_add_test( TestSCTLorentzAngle
#                SCRIPT athena.py --threads=5 SiLorentzAngleTool/testSCTLorentzAngle.py 
#                PROPERTIES TIMEOUT 300
#                ENVIRONMENT THREADS=5 )

#atlas_add_test( SiLorentzAngleConfig_test
#                SCRIPT test/SiLorentzAngleConfig_test.py
#                PROPERTIES TIMEOUT 30 )

# Install files from the package:
#atlas_install_headers( FaserSiLorentzAngleTool )
atlas_install_python_modules( python/*.py )
#atlas_install_joboptions( share/*.py )
