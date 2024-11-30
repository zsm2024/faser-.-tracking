################################################################################
# Package: FaserEventStorage
################################################################################

# Declare the package name:
atlas_subdir( FaserEventStorage )

# External dependencies:
# Apparently not needed...
# find_package( FaserEventPlugins COMPONENTS fReadXRootD fReadPlain fReadDavix )
find_package( tdaq-common COMPONENTS ers EventStorage )
find_package( Boost COMPONENTS system )

atlas_add_library( FaserEventStorageLib
  FaserEventStorage/*.h
  src/*.h src/*.cxx
  PUBLIC_HEADERS FaserEventStorage
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
  LINK_LIBRARIES ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} EventFormats )

