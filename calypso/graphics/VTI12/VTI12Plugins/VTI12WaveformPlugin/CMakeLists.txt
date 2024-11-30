################################################################################
# Package: VTI12WaveformPlugin
################################################################################

# Declare the package name:
atlas_subdir( VTI12WaveformPlugin )

# External dependencies:
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Build the library.
atlas_add_library( VTI12WaveformPlugin
   VTI12WaveformPlugin/*.h src/*.cxx src/*.qrc 
   PUBLIC_HEADERS VTI12WaveformPlugin
   LINK_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets 
   PRIVATE_LINK_LIBRARIES VP1Base VTI12WaveformSystems )
