# Copyright (C) 2020 CERN for the benefit of the FASER collaboration

# Declare the package name.
atlas_subdir( xAODFaserLHCAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODFaserLHCAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODFaserLHC/FaserLHCData.h xAODFaserLHC/FaserLHCDataAux.h
   TYPES_WITH_NAMESPACE xAOD::FaserLHCData xAOD::FaserLHCDataAux
   CNV_PFX xAOD
   LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODFaserLHC )


