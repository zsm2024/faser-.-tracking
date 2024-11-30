/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

// Local include(s):
#include "xAODFaserCalorimeter/versions/CalorimeterHitAuxContainer_v1.h"

namespace xAOD {

  CalorimeterHitAuxContainer_v1::CalorimeterHitAuxContainer_v1() 
    : AuxContainerBase() {

    AUX_VARIABLE(channel);
    AUX_VARIABLE(Nmip);
    AUX_VARIABLE(E_dep);
    AUX_VARIABLE(E_EM);
    AUX_VARIABLE(fit_to_raw_ratio);

    AUX_VARIABLE(WaveformLink);
  }

} // namespace xAOD

