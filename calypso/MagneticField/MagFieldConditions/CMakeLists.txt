################################################################################
# Package: MagFieldConditions
################################################################################

# Declare the package name:
atlas_subdir( MagFieldConditions )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MagFieldConditions
                   src/*.cxx
                   PUBLIC_HEADERS MagFieldConditions
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES SGTools GaudiKernel StoreGateLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} MagFieldElements)

