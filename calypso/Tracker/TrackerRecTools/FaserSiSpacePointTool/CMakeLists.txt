################################################################################
# Package: FaserSiSpacePointTool
################################################################################

# Declare the package name:
atlas_subdir( FaserSiSpacePointTool )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( FaserSiSpacePointToolLib
                   FaserSiSpacePointTool/*.h src/*.cxx src/*.h
                   PUBLIC_HEADERS FaserSiSpacePointTool
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives TrackerPrepRawData
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrackerIdentifier TrackerReadoutGeometry TrackerSpacePoint )

 atlas_add_component( FaserSiSpacePointTool
                     src/components/*.cxx 
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives TrackerPrepRawData GaudiKernel TrackerIdentifier TrackerReadoutGeometry TrackerSpacePoint FaserSiSpacePointToolLib )

