# Copyright (C) 2020 CERN for the benefit of the FASER collaboration

# Declare the package name.
atlas_subdir( xAODFaserWaveformAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODFaserWaveformAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODFaserWaveform/WaveformClock.h xAODFaserWaveform/WaveformClockAuxInfo.h xAODFaserWaveform/WaveformHitContainer.h xAODFaserWaveform/WaveformHitAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::WaveformClock xAOD::WaveformClockAuxInfo xAOD::WaveformHitContainer xAOD::WaveformHitAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODFaserWaveform )


