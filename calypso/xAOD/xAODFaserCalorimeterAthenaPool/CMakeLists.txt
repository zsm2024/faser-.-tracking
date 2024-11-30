# Copyright (C) 2020 CERN for the benefit of the FASER collaboration

# Declare the package name.
atlas_subdir( xAODFaserCalorimeterAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODFaserCalorimeterAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODFaserCalorimeter/CalorimeterHitContainer.h xAODFaserCalorimeter/CalorimeterHitAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::CalorimeterHitContainer xAOD::CalorimeterHitAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODFaserCalorimeter xAODFaserWaveform )


