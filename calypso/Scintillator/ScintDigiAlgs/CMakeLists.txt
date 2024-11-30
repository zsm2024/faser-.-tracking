################################################################################
# Package: ScintDigiAlgs
################################################################################

# Declare the package name:
atlas_subdir( ScintDigiAlgs )

# Component(s) in the package:
atlas_add_component( ScintDigiAlgs
                     src/*.cxx src/*.h
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps Identifier StoreGateLib 
		     ScintIdentifier ScintReadoutGeometry ScintSimEvent 
		     WaveRawEvent WaveformConditionsToolsLib WaveDigiToolsLib)

atlas_install_python_modules( python/*.py )

