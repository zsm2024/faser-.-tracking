# Copyright (C) 2020 CERN for the benefit of the FASER collaboration

# Declare the package name:
atlas_subdir( WaveformDataAccessExample )

# Component(s) in the package:

atlas_add_component( WaveformDataAccessExample
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps WaveRawEvent )

atlas_install_python_modules( python/*.py )

# Test(s) in the package:
atlas_add_test( WaveformDataAccessTest
   SCRIPT python/WaveformDataAccessExampleConfig.py ${CMAKE_CURRENT_SOURCE_DIR}
   PROPERTIES TIMEOUT 300 )

