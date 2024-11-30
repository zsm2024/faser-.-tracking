# Copyright (C) 2020 CERN for the benefit of the FASER collaboration

# Declare the package name.
atlas_subdir( xAODFaserTriggerAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODFaserTriggerAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODFaserTrigger/FaserTriggerData.h xAODFaserTrigger/FaserTriggerDataAux.h
   TYPES_WITH_NAMESPACE xAOD::FaserTriggerData xAOD::FaserTriggerDataAux
   CNV_PFX xAOD
   LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODFaserTrigger )


