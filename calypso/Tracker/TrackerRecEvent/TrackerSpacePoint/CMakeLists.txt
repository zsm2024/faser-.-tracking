################################################################################
# Package: TrackerSpacePoint
################################################################################

# Declare the package name:
atlas_subdir( TrackerSpacePoint )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrackerSpacePoint
		   src/*.cxx src/*.h 
                   PUBLIC_HEADERS TrackerSpacePoint
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
		   LINK_LIBRARIES Identifier TrkDetElementBase TrkSpacePoint AthLinks TrackerPrepRawData
		   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives TrkSurfaces TrkEventPrimitives TrkPrepRawData )

atlas_add_dictionary( TrackerSpacePointDict
		   TrackerSpacePoint/TrackerSpacePointDict.h
		   TrackerSpacePoint/selection.xml
		   LINK_LIBRARIES TrackerSpacePoint)

