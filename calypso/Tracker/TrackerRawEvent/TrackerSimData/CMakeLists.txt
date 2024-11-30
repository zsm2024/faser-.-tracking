################################################################################
# Package: TrackerSimData
################################################################################

# Declare the package name:
atlas_subdir( TrackerSimData )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrackerSimData
                   src/TrackerSimData.cxx
                   src/TrackerSimDataCollection.cxx
                   PUBLIC_HEADERS TrackerSimData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel Identifier GeneratorObjects
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( TrackerSimDataDict
                      TrackerSimData/TrackerSimDataDict.h
                      TrackerSimData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} Identifier GeneratorObjects TrackerSimData )

