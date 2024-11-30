# Copyright (C) 2020 CERN for the benefit of the FASER collaboration

# Declare the package name.
atlas_subdir( xAODFaserWaveform )

# External dependencies.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODFaserWaveform
   xAODFaserWaveform/*.h xAODFaserWaveform/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODFaserWaveform
   LINK_LIBRARIES Identifier xAODCore )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODFaserWaveform/selection.xml
   OUTPUT _selectionFile
   OBJECTS "xAOD::WaveformClock_v1"
   CONTAINERS "xAOD::WaveformHitContainer_v1")

atlas_add_dictionary( xAODFaserWaveformDict
   xAODFaserWaveform/xAODFaserWaveformDict.h
   ${_selectionFile}
   LINK_LIBRARIES Identifier xAODCore xAODFaserWaveform
   EXTRA_FILES Root/dict/*.cxx )

