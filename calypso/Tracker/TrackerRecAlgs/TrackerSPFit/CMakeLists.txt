################################################################################
# Package: TrackerSPFit
################################################################################

# Declare the package name:
atlas_subdir( TrackerSPFit)

# External dependencies:
find_package( Eigen )

find_package( ROOT COMPONENTS Core Tree MathCore)

atlas_add_component( TrackerSPFit
                     src/components/*.cxx src/*.cxx src/*.h
		     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
		     LINK_LIBRARIES ${EIGEN_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives Identifier GaudiKernel TrackerReadoutGeometry TrackerPrepRawData TrackerSpacePoint AthenaMonitoringKernelLib FaserDetDescr xAODEventInfo TrkEventPrimitives TrackerIdentifier FaserSiSpacePointToolLib TrkSpacePoint )

atlas_install_python_modules( python/*.py )
