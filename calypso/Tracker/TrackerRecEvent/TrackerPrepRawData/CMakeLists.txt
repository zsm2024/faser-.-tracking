################################################################################
# Package: TrackerPrepRawData
################################################################################

# Declare the package name:
atlas_subdir( TrackerPrepRawData )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrackerPrepRawData
                   src/*.cxx src/*.h
                   PUBLIC_HEADERS TrackerPrepRawData
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthLinks CxxUtils AthenaKernel GeoPrimitives Identifier EventPrimitives GaudiKernel TrackerReadoutGeometry TrkSurfaces TrkEventPrimitives TrkPrepRawData TrackerRawData EventContainers TrkSpacePoint 
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( TrackerPrepRawDataDict
                      TrackerPrepRawData/TrackerPrepRawDataDict.h
                      TrackerPrepRawData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthLinks CxxUtils AthenaKernel GeoPrimitives Identifier EventPrimitives GaudiKernel TrackerReadoutGeometry TrkSurfaces TrkEventPrimitives TrkPrepRawData TrackerRawData TrackerPrepRawData EventContainers )

