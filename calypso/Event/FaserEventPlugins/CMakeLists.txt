################################################################################
# Package: FaserEventsPlugins
################################################################################

# Declare the package name:
atlas_subdir( FaserEventPlugins )

# External dependencies:
find_package( tdaq-common COMPONENTS ers EventStorage )
find_package( Boost COMPONENTS system )
find_package( Xrootd COMPONENTS Posix PosixPreload )
find_package( Davix )

# Make sure that libraries are linked correctly:
atlas_disable_as_needed()

atlas_add_library( fReadXRootD
   src/fReadXRootD.h src/fReadXRootD.cxx
   NO_PUBLIC_HEADERS
   PRIVATE_INCLUDE_DIRS FaserEventStorage ${TDAQ-COMMON_INCLUDE_DIRS} 
   ${XROOTD_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES FaserEventStorageLib ${TDAQ-COMMON_LIBRARIES} 
   ${XROOTD_LIBRARIES} ${Boost_LIBRARIES}
   PRIVATE_DEFINITIONS -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
   -D_FILE_OFFSET_BITS=64 )

atlas_add_library( fReadPlain 
   src/fReadPlain.h src/fReadPlain.cxx 
   NO_PUBLIC_HEADERS
   PRIVATE_INCLUDE_DIRS FaserEventStorage ${TDAQ-COMMON_INCLUDE_DIRS} 
   ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES FaserEventStorageLib ${TDAQ-COMMON_LIBRARIES} 
   ${Boost_LIBRARIES})

atlas_add_library( fReadDavix
   src/fReadDavix.h src/fReadDavix.cxx
   NO_PUBLIC_HEADERS
   PRIVATE_INCLUDE_DIRS FaserEventStorage ${TDAQ-COMMON_INCLUDE_DIRS}
   ${DAVIX_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES FaserEventStorageLib ${TDAQ-COMMON_LIBRARIES} 
   ${DAVIX_LIBRARIES} ${Boost_LIBRARIES} )


