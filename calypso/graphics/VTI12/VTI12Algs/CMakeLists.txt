################################################################################
# Package: VTI12Algs
################################################################################

# Declare the package name:
atlas_subdir( VTI12Algs )

# "Externals" dependencies - They are needed at runtime, only
find_package( libxkbcommon ) 
find_package( glib ) 
find_package( libffi )

# Component(s) in the package:
atlas_add_component( VTI12Algs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel xAODEventInfo PathResolver EventInfo
                     PRIVATE_LINK_LIBRARIES VTI12Gui VP1UtilsBase )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/* )
atlas_install_scripts( share/vti12 )
#atlas_install_scripts( share/clear-bash-hash-table-vp1 )

#----------------------------------------------------------
# CONFIGURING THE ENVIRONMENT VARIABLES FOR THE PACKAGE   #
#
#message( "CMAKE_BINARY_DIR path:" ${CMAKE_BINARY_DIR} )
#message( "ATLAS_PLATFORM path:" ${ATLAS_PLATFORM} )
# set a variable to be used in the 'Config' script. 
#   The variable will be substituted with the path when file is 'configured' 
#   and copied from 'source' to 'binary'
set( BUILD_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM} )
# Configure the environment setup module. 
#   The 'configure_file' command here below copies the input file in the 'source' dir
#   to the output file in 'binary' dir, e.g. in ../build/graphics/VP1/VP1Algs/
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/VTI12AlgsEnvironmentConfig.cmake.in  
    ${CMAKE_CURRENT_BINARY_DIR}/VTI12AlgsEnvironmentConfig.cmake   
    @ONLY )
# Set the folder where CMake can find the 'Config' file for the dummy package
set( VTI12AlgsEnvironment_DIR ${CMAKE_CURRENT_BINARY_DIR}  
    CACHE PATH "Location of VTI12AlgsEnvironmentConfig.cmake" 
    FORCE 
    )
# And now "find" it! It will set the env vars defined in the package's 'Config' file.
find_package( VTI12AlgsEnvironment )
# message( "VP1PLUGINPATH has been set to: " $ENV{VP1PLUGINPATH} )


