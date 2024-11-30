################################################################################
# Package: CaloRecTools
################################################################################

# Declare the package name:
atlas_subdir( CaloRecTools )

# External dependencies:
find_package( ROOT )

# Component(s) in the package:
atlas_add_library( CaloRecToolsLib
                   CaloRecTools/*.h src/*.cxx src/*.h
                   PUBLIC_HEADERS CaloRecTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES xAODFaserWaveform xAODFaserCalorimeter
		   AthenaBaseComps AthenaPoolUtilities AthenaKernel GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES}
       		   )

atlas_add_component( CaloRecTools
		     src/components/*.cxx 
		     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} 
		     AthenaBaseComps AthenaPoolUtilities AthenaKernel GaudiKernel 
		     CaloRecToolsLib)

# Install files from the package:
atlas_install_python_modules( python/*.py )


