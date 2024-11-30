################################################################################
# Package: FaserSiClusterizationTool
################################################################################

# Declare the package name:
atlas_subdir( FaserSiClusterizationTool )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_library( FaserSiClusterizationToolLib
   FaserSiClusterizationTool/*.h src/*.cxx src/*.h
   PUBLIC_HEADERS FaserSiClusterizationTool
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives
   Identifier EventPrimitives GaudiKernel TrackerSimData TrackerIdentifier
   TrackerReadoutGeometry TrackerRawData TrackerPrepRawData
   TrkParameters CxxUtils FaserSCT_ConditionsToolsLib
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaPoolUtilities FileCatalog FaserDetDescr
   TrkSurfaces TrkEventPrimitives )

atlas_add_component( FaserSiClusterizationTool
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel FaserSiClusterizationToolLib )

# Install files from the package:
#atlas_install_joboptions( share/*.py )
