
# Declare the package name:
atlas_subdir( FaserActsGeometry )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Boost )
find_package( nlohmann_json )

find_package( Acts COMPONENTS Core )

atlas_add_library( FaserActsGeometryLib 
                        src/FaserActsSurfaceMappingTool.cxx
                        src/FaserActsMaterialJsonWriterTool.cxx
                        src/FaserActsMaterialMapping.cxx
                        src/FaserActsAlignmentStore.cxx
                        src/FaserActsDetectorElement.cxx
                        src/FaserActsLayerBuilder.cxx
                        src/CuboidVolumeBuilder.cxx
                        src/util/*.cxx
                        PUBLIC_HEADERS FaserActsGeometry
                        INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS}
                        LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${BOOST_LIBRARIES} nlohmann_json::nlohmann_json
                        TrackerIdentifier 
                        TrackerReadoutGeometry
                        ActsInteropLib
                        FaserActsGeometryInterfacesLib
                        AthenaKernel								
                        ActsCore
                        GeoModelFaserUtilities
                        ActsGeometryLib
                        ActsGeometryInterfacesLib
                        MagFieldInterfaces 
                        MagFieldElements 
                        MagFieldConditions
                        GeneratorObjects 
                        TrackerIdentifier 
                        TrkGeometry
		  )


# Component(s) in the package:
atlas_add_component( FaserActsGeometry
                         src/FaserActsSurfaceMappingTool.cxx
                         src/FaserActsMaterialMapping.cxx
                         src/FaserActsMaterialJsonWriterTool.cxx
                         src/FaserActsTrackingGeometrySvc.cxx
                         src/FaserActsTrackingGeometryTool.cxx
                         src/FaserActsWriteTrackingGeometry.cxx
                         src/FaserActsObjWriterTool.cxx
                         src/FaserActsExtrapolationAlg.cxx
                         src/FaserActsExtrapolationTool.cxx
                         src/FaserActsPropStepRootWriterSvc.cxx
                         src/FaserActsMaterialTrackWriterSvc.cxx
                         src/FaserActsAlignmentCondAlg.cxx
                         src/NominalAlignmentCondAlg.cxx
                         src/FaserActsVolumeMappingTool.cxx
                         src/FaserActsGeometryBoundaryTestAlg.h
                         src/FaserActsGeometryBoundaryTestAlg.cxx
                         src/components/*.cxx
                         PUBLIC_HEADERS FaserActsGeometry
                         INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS}
                         LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${BOOST_LIBRARIES}
                         EventInfo
                         AthenaBaseComps
                         GaudiKernel
                         FaserActsGeometryLib
                         ActsInteropLib
                         FaserActsGeometryInterfacesLib
                         ActsCore
                         ActsGeometryLib
                         ActsGeometryInterfacesLib
                         GeoModelFaserUtilities
                         MagFieldInterfaces
                         MagFieldElements
                         MagFieldConditions
                         GeneratorObjects 
                         TrackerIdentifier 
                         TrkGeometry

)

# Install files from the package:
# atlas_install_headers( FaserActsGeometry )
atlas_install_python_modules( python/*.py )
atlas_install_scripts( test/*.py )

atlas_add_test( FaserActsGeometryBoundary_test
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/FaserActsGeometryBoundary_test.py
                PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                PROPERTIES TIMEOUT 300 )
