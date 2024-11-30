###############################################################################
# Package: LHCDataAlgs
################################################################################

# Declare the package name:
atlas_subdir( LHCDataAlgs )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_component( LHCDataAlgs
                     src/*.h
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib xAODFaserLHC LHCDataToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
