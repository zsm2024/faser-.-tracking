# Copyright (C) 2020 CERN for the benefit of the FASER collaboration

# Declare the package name.
atlas_subdir( xAODFaserCalorimeter )

# External dependencies.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODFaserCalorimeter
   xAODFaserCalorimeter/*.h xAODFaserCalorimeter/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODFaserCalorimeter
   LINK_LIBRARIES Identifier xAODCore xAODFaserWaveform )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODFaserCalorimeter/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::CalorimeterHitContainer_v1")

atlas_add_dictionary( xAODFaserCalorimeterDict
   xAODFaserCalorimeter/xAODFaserCalorimeterDict.h
   ${_selectionFile}
   LINK_LIBRARIES Identifier xAODCore xAODFaserCalorimeter
   EXTRA_FILES Root/dict/*.cxx )

