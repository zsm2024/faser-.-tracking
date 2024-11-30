################################################################################
# Package: WaveRecTools
################################################################################

# Declare the package name:
atlas_subdir( WaveRecTools )

# External dependencies:
find_package( ROOT )

# Component(s) in the package:
atlas_add_library( WaveRecToolsLib
                   WaveRecTools/*.h src/*.cxx src/*.h
                   PUBLIC_HEADERS WaveRecTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GeoPrimitives 
		   WaveformConditionsToolsLib WaveRawEvent xAODFaserWaveform Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES}
		   )

atlas_add_component( WaveRecTools
		     src/components/*.cxx 
		     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} 
		     WaveformConditionsToolsLib AthenaBaseComps GaudiKernel 
		     WaveRecToolsLib)


