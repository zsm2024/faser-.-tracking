################################################################################
# Package: TrackerSeedFinder
#############################################

# Declare the package name:
atlas_subdir( TrackerSeedFinder )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrackerSeedFinderLib
  src/components/*.cxx src/*.cxx TrackerSeedFinder/*.h
  PUBLIC_HEADERS TrackerSeedFinder
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
		   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives Identifier GaudiKernel TrackerReadoutGeometry TrackerRawData TrackerSimData GeneratorObjects TrackerSimEvent TrackerSpacePoint VxVertex FaserDetDescr xAODEventInfo TrackerIdentifier TrackerPrepRawData AthenaMonitoringKernelLib)

atlas_add_component( TrackerSeedFinder
  src/components/*.cxx src/*.cxx TrackerSeedFinder/*.h
		     PUBLIC_HEADERS TrackerSeedFinder
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
		     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives Identifier GaudiKernel TrackerReadoutGeometry TrackerRawData TrackerSimData GeneratorObjects TrackerSimEvent TrackerSpacePoint VxVertex FaserDetDescr xAODEventInfo TrackerIdentifier TrackerPrepRawData AthenaMonitoringKernelLib)

atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )
