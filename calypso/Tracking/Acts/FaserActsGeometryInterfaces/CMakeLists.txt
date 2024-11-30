
# Declare the package name:
atlas_subdir( FaserActsGeometryInterfaces )

# External dependencies:
find_package( Eigen )
#set( Acts_DIR /home/tboeckh/Documents/acts/run )
#find_package( Acts REQUIRED COMPONENTS Core PATHS /home/tboeckh/Documents/acts/run NO_DEFAULT_PATH )
find_package( Acts COMPONENTS Core )
find_package( nlohmann_json )
# Component(s) in the package:

atlas_add_library( FaserActsGeometryInterfacesLib
                   FaserActsGeometryInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS FaserActsGeometryInterfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} nlohmann_json::nlohmann_json
                   AthenaKernel
                   ActsInteropLib
                   ActsCore)
