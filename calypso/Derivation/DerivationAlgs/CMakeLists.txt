################################################################################
# Package: DerivationAlgs
################################################################################

# Declare the package name:
atlas_subdir( DerivationAlgs )

atlas_add_component( DerivationAlgs
		     src/*.cxx src/*.h
		     src/components/*.cxx 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel StoreGateLib DerivationToolsLib)

atlas_install_python_modules( python/*.py )


