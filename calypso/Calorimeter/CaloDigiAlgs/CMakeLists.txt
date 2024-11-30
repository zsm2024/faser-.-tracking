################################################################################
# Package: CaloDigiAlgs
################################################################################

# Declare the package name:
atlas_subdir( CaloDigiAlgs )

# Component(s) in the package:
atlas_add_component( CaloDigiAlgs
                     src/*.cxx src/*.h
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps Identifier FaserCaloIdentifier  
		     WaveformConditionsToolsLib StoreGateLib WaveRawEvent 
		     CaloReadoutGeometry FaserCaloSimEvent WaveDigiToolsLib)

atlas_install_python_modules( python/*.py )

