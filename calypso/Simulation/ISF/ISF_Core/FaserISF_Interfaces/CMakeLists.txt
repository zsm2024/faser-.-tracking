################################################################################
# Package: FaserISF_Interfaces
################################################################################

# Declare the package name:
atlas_subdir( FaserISF_Interfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( FaserISF_InterfacesLib
                   src/*.cxx
                   PUBLIC_HEADERS FaserISF_Interfaces
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel FaserDetDescr GeoPrimitives GaudiKernel GeneratorObjects FaserISF_Event ISF_Event StoreGateLib SGtests TrackRecordLib )

