################################################################################
# Package: FaserSiSpacePointTool
################################################################################

# Declare the package name:
atlas_subdir( TruthSeededTrackFinderTool)

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TruthSeededTrackFinderToolLib
                   TruthSeededTrackFinderTool/*.h src/*.cxx src/*.h
		               PUBLIC_HEADERS TruthSeededTrackFinderTool
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives TrackerPrepRawData
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrackerIdentifier TrackerReadoutGeometry TrackerSpacePoint )

atlas_add_component( TruthSeededTrackFinderTool
                     src/components/*.cxx 
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
		                 LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives TrackerPrepRawData GaudiKernel 
                                    TrackerIdentifier TrackerReadoutGeometry TrackerSpacePoint TruthSeededTrackFinderToolLib )

