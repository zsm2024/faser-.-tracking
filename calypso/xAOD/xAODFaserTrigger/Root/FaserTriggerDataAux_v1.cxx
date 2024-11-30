/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

// $Id: $

// Local include(s):
#include "xAODFaserTrigger/versions/FaserTriggerDataAux_v1.h"

namespace xAOD {

  FaserTriggerDataAux_v1::FaserTriggerDataAux_v1()
    : AuxInfoBase(),
      header(0), eventId(0), orbitId(0), bcid(0), 
      inputBitsNextClk(0), inputBits(0), tbp(0), tap(0) {
    AUX_VARIABLE( header );
    AUX_VARIABLE( eventId );
    AUX_VARIABLE( orbitId );
    AUX_VARIABLE( bcid );
    AUX_VARIABLE( inputBitsNextClk );
    AUX_VARIABLE( inputBits );
    AUX_VARIABLE( tbp );
    AUX_VARIABLE( tap );
  }

} // namespace xAOD
