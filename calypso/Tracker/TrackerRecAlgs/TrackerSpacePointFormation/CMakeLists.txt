################################################################################
# Package: TrackerSpacePointFormation
################################################################################

# Declare the package name:
atlas_subdir( TrackerSpacePointFormation )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
#atlas_add_library( TrackerSpacePointFormationLib
#                   src/*.cxx src/*.h
#                   PUBLIC_HEADERS TrackerSpacePointMakerAlg
#                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
#                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives Identifier GaudiKernel TrackerReadoutGeometry TrackerPrepRawData TrackerSpacePoint AthenaMonitoringKernelLib
#                   PRIVATE_LINK_LIBRARIES FaserDetDescr xAODEventInfo TrackerIdentifier )

atlas_add_component( TrackerSpacePointFormation
                     src/components/*.cxx src/*.cxx src/*.h
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
		             LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives Identifier GaudiKernel TrackerReadoutGeometry TrackerPrepRawData TrackerSpacePoint AthenaMonitoringKernelLib FaserDetDescr xAODEventInfo TrackerIdentifier FaserSiSpacePointToolLib TrkSpacePoint )

atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )
