# Copyright (C) 2020 CERN for the benefit of the FASER collaboration

# Declare the package name:
atlas_subdir( TriggerDataAccessExample )

# Component(s) in the package:

atlas_add_component( TriggerDataAccessExample
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODFaserTrigger )

atlas_install_python_modules( python/*.py )

# Test(s) in the package:
atlas_add_test( TriggerDataAccessTest
   SCRIPT python/TriggerDataAccessExampleConfig.py ${CMAKE_CURRENT_SOURCE_DIR}
   PROPERTIES TIMEOUT 300 )

