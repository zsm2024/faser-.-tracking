################################################################################
# Package: TrackerSegmentFIt
################################################################################

# Declare the package name:
atlas_subdir( TrackerSegmentFit )

# External dependencies
find_package( Eigen )
find_package( Boost )

# Component(s) in the package:
atlas_add_component( TrackerSegmentFit
                    src/*.cxx src/*.h
                    src/components/*.cxx
                    INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                    LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthViews StoreGateLib SGtests Identifier GaudiKernel TrackerRawData TrackerPrepRawData FaserDetDescr TrackerIdentifier TrackerReadoutGeometry xAODFaserTrigger 
                                                      TrkTrack TrkEventPrimitives TrackerRIO_OnTrack TrkRIO_OnTrack TrackerEventTPCnv )

atlas_install_python_modules( python/*.py )

atlas_install_scripts( test/*.py )
