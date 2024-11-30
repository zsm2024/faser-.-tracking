################################################################################
# Package: FaserISF_Event
################################################################################

# Declare the package name:
atlas_subdir( FaserISF_Event )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( FaserISF_Event
                   src/*.cxx
                   PUBLIC_HEADERS FaserISF_Event
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ISF_Event TestTools AthenaBaseComps FaserDetDescr GeoPrimitives GeneratorObjects GaudiKernel )

#atlas_add_test( FaserISFParticle_test
#                SOURCES
#                test/ISFParticle_test.cxx
#                INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
#                LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ISF_Event TestTools AthenaBaseComps FaserDetDescr GeoPrimitives GeneratorObjects GaudiKernel FaserISF_Event 
#                WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
