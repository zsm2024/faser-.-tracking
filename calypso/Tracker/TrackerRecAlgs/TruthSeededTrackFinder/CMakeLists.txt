################################################################################
# Package: TruthSeededTrackFinder
#############################################

# Declare the package name:
atlas_subdir( TruthSeededTrackFinder )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
#atlas_add_library( TruthSeededTrackFinderLib
#                   src/*.cxx src/*.h
#		   PUBLIC_HEADERS TruthTrackSeeds
#                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
#		   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives Identifier GaudiKernel TrackerReadoutGeometry TrackerRawData TrackerSimData GeneratorObjects TrackerSimEvent TrackerSpacePoint TrackerPrepRawData VxVertex TrackerSpacePointMakerToolLib BeamSpotConditionsData AthenaMonitoringKernelLib
#                   PRIVATE_LINK_LIBRARIES FaserDetDescr xAODEventInfo TrackerIdentifier )

 atlas_add_component( TruthSeededTrackFinder
                     src/components/*.cxx src/*.cxx src/*.h
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
		     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives Identifier GaudiKernel TrackerReadoutGeometry TrackerRawData TrackerSimData GeneratorObjects TrackerSimEvent 
                            TrackerSpacePoint FaserDetDescr xAODEventInfo TrackerIdentifier TrackerPrepRawData AthenaMonitoringKernelLib)

atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )
