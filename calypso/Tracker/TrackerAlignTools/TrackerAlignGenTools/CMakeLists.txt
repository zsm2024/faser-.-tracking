################################################################################
# Package: TrackerAlignGenTools
################################################################################

# Declare the package name:
atlas_subdir( TrackerAlignGenTools )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_library ( TrackerAlignGenToolsLib
                    src/*.cxx
                    PUBLIC_HEADERS TrackerAlignGenTools
                    INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                    LINK_LIBRARIES ${CORAL_LIBRARIES} ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaPoolUtilities AtlasHepMCLib GeoPrimitives Identifier EventPrimitives AthenaKernel AthContainers FaserDetDescr DetDescrConditions GaudiKernel TrackerIdentifier TrackerReadoutGeometry RegistrationServicesLib )

atlas_add_component( TrackerAlignGenTools
                     src/components/*.cxx
                     LINK_LIBRARIES TrackerAlignGenToolsLib )

# Install files from the package:
# obsolete
# atlas_install_headers( TrackerAlignGenTools )

