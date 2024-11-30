# Copyright (C) 2020 CERN for the benefit of the FASER collaboration

# Declare the package name:
atlas_subdir( TrackerByteStream )

# Component(s) in the package:

atlas_add_component( TrackerByteStream
                     src/*.cxx src/*.h
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel GaudiKernel StoreGateLib FaserByteStreamCnvSvcBaseLib FaserEventStorageLib TrackerRawData TrackerReadoutGeometry TrackerIdentifier EventFormats FaserSCT_ConditionsToolsLib
                     PRIVATE_LINK_LIBRARIES AthenaBaseComps )


atlas_install_python_modules( python/*.py )
