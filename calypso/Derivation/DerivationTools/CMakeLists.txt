################################################################################
# Package: DerivationTools
################################################################################

# Declare the package name:
atlas_subdir( DerivationTools )

# External dependencies:
find_package( ROOT )

# Component(s) in the package:
atlas_add_library( DerivationToolsLib
                   DerivationTools/*.h src/*.cxx src/*.h
                   PUBLIC_HEADERS DerivationTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel xAODFaserTrigger
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES}
		   )

atlas_add_component( DerivationTools
		     src/components/*.cxx 
		     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel xAODEventInfo xAODFaserTrigger DerivationToolsLib)

