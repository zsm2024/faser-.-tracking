################################################################################
# Package: VTI12FaserPlugin
################################################################################

# Declare the package name:
atlas_subdir( VTI12FaserPlugin )

# External dependencies:
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Build the library.
atlas_add_library( VTI12FaserPlugin
   VTI12FaserPlugin/*.h src/*.cxx src/*.qrc 
   PUBLIC_HEADERS VTI12FaserPlugin
   LINK_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets
   PRIVATE_LINK_LIBRARIES VP1Base VTI12GuideLineSystems VTI12GeometrySystems VTI12SimHitSystems VTI12RawDataSystems VTI12PRDSystems VTI12TrackSystems)
