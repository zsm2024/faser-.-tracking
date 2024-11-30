# Copyright (C) 2020 CERN for the benefit of the FASER collaboration

# Declare the package name:
atlas_subdir( WaveByteStream )

# Component(s) in the package:
atlas_add_component( WaveByteStream
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel GaudiKernel FaserByteStreamCnvSvcBaseLib FaserEventStorageLib WaveRawEvent WaveformConditionsToolsLib
                     PRIVATE_LINK_LIBRARIES AthenaBaseComps )

atlas_install_python_modules( python/*.py )
