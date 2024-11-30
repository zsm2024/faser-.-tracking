# $Id: CMakeLists.txt 728682 2016-03-09 15:17:26Z krasznaa $
################################################################################
# Package: VTI12TrackSystems
################################################################################

# Declare the package name:
atlas_subdir( VTI12TrackSystems )

# External dependencies:
find_package( CLHEP )
find_package( Coin3D )
find_package( Qt5 COMPONENTS Core Gui Widgets  )
find_package( GeoModel )

# Generate UI files automatically:
# Note: add the "Widgets" component to "find_package( Qt5 ...)" if you have UI files, otherwise UIC, even if CMAKE_AUTOUIC is set to ON, is not run
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VTI12TrackSystems VTI12TrackSystems/*.h src/*.cxx
   PUBLIC_HEADERS VTI12TrackSystems
   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES ${GEOMODEL_LIBRARIES} GeoPrimitives Identifier 
   #xAODTracking
   GaudiKernel GeneratorObjects TrkDetDescrInterfaces TrkSurfaces
   TrkEventPrimitives TrkParameters VP1Base VTI12Utils StoreGateLib SGtests
   Qt5::Core Qt5::Gui TrackRecordLib
   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${CLHEP_LIBRARIES}
   AtlasHepMCLib AthContainers FaserDetDescr EventPrimitives
   NeutrinoIdentifier NeutrinoSimEvent NeutrinoReadoutGeometry
   ScintIdentifier ScintSimEvent ScintReadoutGeometry
   TrackerIdentifier TrackerReadoutGeometry TrackerSimEvent
   FaserCaloIdentifier CaloReadoutGeometry FaserCaloSimEvent
   TrackerPrepRawData TrackerRIO_OnTrack
   Particle
   TrkDetDescrUtils TrkDetElementBase TrkVolumes TrkCompetingRIOsOnTrack
   TrkMaterialOnTrack TrkMeasurementBase TrkPrepRawData
   TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkTrack
   TrkTrackSummary TrkExInterfaces TrkFitterInterfaces VP1HEPVis
   )
