# $Id: CMakeLists.txt 720646 2016-01-29 14:01:10Z krasznaa $
#
# Build configuration for package AtlasAuthentication.
#

# Declare the package's name:
atlas_subdir( FaserAuthentication )

# If we are in release rebuilding mode, stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Install the local file(s) of the package:
atlas_install_xmls( data/*.xml )
atlas_install_runtime( data/dbreplica.config )
#atlas_install_scripts( share/setupLocalDBReplica_CERN.sh )

# Find the ATLAS externals area:
if( NOT SITEROOT )
   if( ENV{SITEROOT} )
      set( SITEROOT $ENV{SITEROOT} )
   else()
      set( SITEROOT "/afs/cern.ch" )
   endif()
endif()
if( NOT FASER_EXTERNAL )
   if( ENV{FASER_EXTERNAL} )
      set( FASER_EXTERNAL $ENV{FASER_EXTERNAL} )
   else()
      set( FASER_EXTERNAL "${SITEROOT}/faser/offline/external" )
   endif()
endif()

# Pick up authentication.xml:
set( FaserAuthentication_native_version "v18" )
set( FaserAuthentication_home
   "${FASER_EXTERNAL}/FaserAuth/${FaserAuthentication_native_version}" )
# Apparently not needed, because we don't have it...
#atlas_install_xmls( ${FaserAuthentication_home}/authentication.xml )


# Configure the environment setup module:
configure_file(
   ${CMAKE_CURRENT_SOURCE_DIR}/FaserAuthenticationEnvironmentConfig.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/FaserAuthenticationEnvironmentConfig.cmake
   @ONLY )

# And now "find" it:
set( FaserAuthenticationEnvironment_DIR ${CMAKE_CURRENT_BINARY_DIR}
   CACHE PATH "Location of FaserAuthenticationEnvironmentConfig.cmake" )
find_package( FaserAuthenticationEnvironment )
