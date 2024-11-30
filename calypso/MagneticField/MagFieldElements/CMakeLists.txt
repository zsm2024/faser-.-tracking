################################################################################
# Package: MagFieldElements
################################################################################

# Declare the package name:
atlas_subdir( MagFieldElements )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MagFieldElements
                   src/*.cxx
                   PUBLIC_HEADERS MagFieldElements
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
		           LINK_LIBRARIES EventPrimitives GaudiKernel CxxUtils
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver)

# Install files from the package:
# obsolete
# atlas_install_headers( MagFieldElements )

