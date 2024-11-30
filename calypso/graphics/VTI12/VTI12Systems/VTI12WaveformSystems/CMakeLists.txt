################################################################################
# Package: VTI12WaveformSystems
################################################################################

# Declare the package name:
atlas_subdir( VTI12WaveformSystems )

# External dependencies:
find_package( Coin3D )
find_package( Qt5 COMPONENTS Core Gui Widgets Charts )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VTI12WaveformSystems VTI12WaveformSystems/*.h src/*.cxx
   PUBLIC_HEADERS VTI12WaveformSystems
   INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${QT5_INCLUDE_DIRS}
   LINK_LIBRARIES ${COIN3D_LIBRARIES} GeoPrimitives VP1Base VTI12Utils WaveRawEvent xAODFaserWaveform
   Qt5::Core Qt5::Gui Qt5::Charts
   PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
   PRIVATE_LINK_LIBRARIES GaudiKernel VP1HEPVis )
