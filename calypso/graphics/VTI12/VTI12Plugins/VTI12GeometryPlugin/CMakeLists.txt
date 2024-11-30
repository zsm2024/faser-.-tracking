################################################################################
# Package: VTI12GeometryPlugin
################################################################################

# Declare the package name:
atlas_subdir( VTI12GeometryPlugin )

# External dependencies:
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Build the library.
atlas_add_library( VTI12GeometryPlugin
   VTI12GeometryPlugin/*.h src/*.cxx src/*.qrc 
   PUBLIC_HEADERS VTI12GeometryPlugin
   LINK_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets 
   PRIVATE_LINK_LIBRARIES VP1Base VTI12GeometrySystems VTI12GuideLineSystems )
