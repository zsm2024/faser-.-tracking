################################################################################
# Package: VTI12Utils
################################################################################
# Author: Thomas Kittelmann
# Author: Riccardo Maria BIANCHI <rbianchi@cern.ch>
################################################################################

# Declare the package name:
atlas_subdir( VTI12Utils )

# External dependencies:
find_package( CLHEP ) # TODO: to be removed when fully migrated to Eigen-based GeoTrf
find_package( Coin3D )
find_package( Eigen )
find_package( HepPDT )
find_package( Qt5 COMPONENTS Core REQUIRED)
find_package( GeoModel )

# Generate MOC files automatically:
set( CMAKE_AUTOMOC ON )

# Component(s) in the package:
atlas_add_library( VTI12Utils VTI12Utils/*.h src/*.cxx src/*.cpp
   PUBLIC_HEADERS VTI12Utils
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS}
   ${EIGEN_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODEL_LIBRARIES} EventPrimitives
   GaudiKernel VP1Base StoreGateLib SGtests AthDSoCallBacks 
   GeoPrimitives Qt5::Core
   PRIVATE_LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${COIN3D_LIBRARIES}
   ${EIGEN_LIBRARIES} CxxUtils FaserDetDescr
   GeoModelFaserUtilities Identifier
   ScintIdentifier ScintReadoutGeometry
   TrackerIdentifier TrackerReadoutGeometry
   FaserCaloIdentifier CaloReadoutGeometry
   NeutrinoIdentifier NeutrinoReadoutGeometry
   #InDetRIO_OnTrack 
   TrkSurfaces TrkRIO_OnTrack VP1HEPVis )
