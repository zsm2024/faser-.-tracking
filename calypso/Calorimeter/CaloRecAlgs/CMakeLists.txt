################################################################################
# Package: CaloRecAlgs
################################################################################

# Declare the package name:
atlas_subdir( CaloRecAlgs )

# Component(s) in the package:
atlas_add_component( CaloRecAlgs
                     src/*.cxx src/*.h
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel GaudiKernel AthenaBaseComps AthenaPoolUtilities StoreGateLib xAODFaserCalorimeter xAODFaserWaveform CaloRecToolsLib)

atlas_install_python_modules( python/*.py )

