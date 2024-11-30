################################################################################
# Package: WaveDigiTools
################################################################################

# Declare the package name:
atlas_subdir( WaveDigiTools )

# External dependencies:
find_package( ROOT )

# Component(s) in the package:
atlas_add_library( WaveDigiToolsLib
                   WaveDigiTools/*.h src/*.cxx src/*.h
                   PUBLIC_HEADERS WaveDigiTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GeoPrimitives 
		   WaveRawEvent Identifier ScintSimEvent ScintReadoutGeometry
		   FaserCaloSimEvent CaloReadoutGeometry
		   WaveformConditionsToolsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES}
		   )

atlas_add_component( WaveDigiTools
		     src/components/*.cxx 
		     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} 
		     AthenaBaseComps GaudiKernel WaveDigiToolsLib)

