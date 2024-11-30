# Copyright (C) 2020 CERN for the benefit of the FASER collaboration

# Declare the package name:
atlas_subdir( FaserTriggerByteStream )

# Component(s) in the package:
atlas_add_library( FaserTriggerByteStreamLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES AthenaKernel GaudiKernel StoreGateLib FaserByteStreamCnvSvcBaseLib FaserEventStorageLib
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps xAODFaserTrigger  )

atlas_add_component( FaserTriggerByteStream
                     src/components/*.cxx
                     LINK_LIBRARIES FaserTriggerByteStreamLib xAODFaserTrigger )

#atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
