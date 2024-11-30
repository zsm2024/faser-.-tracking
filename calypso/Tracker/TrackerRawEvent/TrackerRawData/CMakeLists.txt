################################################################################
# Package: TrackerRawData
################################################################################

# Declare the package name:
atlas_subdir( TrackerRawData )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrackerRawData
                   src/*.cxx
                   PUBLIC_HEADERS TrackerRawData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
		   LINK_LIBRARIES AthenaKernel AthContainers AthLinks Identifier EventContainers TrackerReadoutGeometry GeoPrimitives
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( TrackerRawDataDict
                      TrackerRawData/TrackerRawDataCLASS_DEF.h
                      TrackerRawData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks Identifier GaudiKernel TrackerRawData EventContainers )

