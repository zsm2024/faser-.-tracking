# Copyright (C) 2020 CERN for the benefit of the FASER collaboration

# Declare the package name.
atlas_subdir( xAODFaserLHC )

# External dependencies.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODFaserLHC
   xAODFaserLHC/*.h xAODFaserLHC/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODFaserLHC
   LINK_LIBRARIES xAODCore )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODFaserLHC/selection.xml
   OUTPUT _selectionFile
   OBJECTS "xAOD::FaserLHCData_v1" "xAOD::FaserLHCDataAux_v1")

atlas_add_dictionary( xAODFaserLHCDict
   xAODFaserLHC/xAODFaserLHCDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODFaserLHC
   EXTRA_FILES Root/dict/*.cxx )

# Understand what this does...
atlas_generate_cliddb( xAODFaserLHC )
