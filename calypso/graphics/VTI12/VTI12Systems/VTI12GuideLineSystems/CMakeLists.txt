# $Id: CMakeLists.txt 728676 2016-03-09 15:11:49Z krasznaa $
################################################################################
# Package: VTI12GuideLineSystems
################################################################################

# Declare the package name:
atlas_subdir( VTI12GuideLineSystems )

# External dependencies:
find_package( Coin3D )
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VTI12GuideLineSystems VTI12GuideLineSystems/*.h src/*.cxx
   PUBLIC_HEADERS VTI12GuideLineSystems
   INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
   LINK_LIBRARIES ${COIN3D_LIBRARIES} GeoPrimitives VP1Base VTI12Utils
   Qt5::Core Qt5::Gui
   PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
   PRIVATE_LINK_LIBRARIES GaudiKernel VP1HEPVis )
