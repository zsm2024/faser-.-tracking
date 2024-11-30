################################################################################
# Package: WaveRecAlgs
################################################################################

# Declare the package name:
atlas_subdir( WaveRecAlgs )

# Component(s) in the package:
atlas_add_component( WaveRecAlgs
                     src/*.cxx src/*.h
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib WaveRawEvent xAODFaserWaveform WaveRecToolsLib ScintSimEvent FaserCaloSimEvent)

atlas_install_python_modules( python/*.py )

