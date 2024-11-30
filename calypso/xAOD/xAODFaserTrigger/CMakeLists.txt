# Copyright (C) 2020 CERN for the benefit of the FASER collaboration
# CMAKE file based on Event/xAOD/xAODTrigger/CMakeLists.txt

# Declare the package name.
atlas_subdir( xAODFaserTrigger )

# External dependencies.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODFaserTrigger
   xAODFaserTrigger/*.h xAODFaserTrigger/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODFaserTrigger
   LINK_LIBRARIES xAODCore )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODFaserTrigger/selection.xml
   OUTPUT _selectionFile
   OBJECTS "xAOD::FaserTriggerData_v1" "xAOD::FaserTriggerDataAux_v1")

atlas_add_dictionary( xAODFaserTriggerDict
   xAODFaserTrigger/xAODFaserTriggerDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODFaserTrigger
   EXTRA_FILES Root/dict/*.cxx )

# Understand what this does...
atlas_generate_cliddb( xAODFaserTrigger )
