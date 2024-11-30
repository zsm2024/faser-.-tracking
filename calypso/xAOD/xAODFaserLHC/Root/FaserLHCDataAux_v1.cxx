/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

// $Id: $

// Local include(s):
#include "xAODFaserLHC/versions/FaserLHCDataAux_v1.h"

namespace xAOD {

  FaserLHCDataAux_v1::FaserLHCDataAux_v1()
    : AuxInfoBase(),
      fillNumber(0), 
      machineMode(""),
      beamMode(""),
      beamType1(0),
      beamType2(0),
      betaStar(0),
      crossingAngle(0),
      stableBeams(false),
      injectionScheme(""),
      numBunchBeam1(0),
      numBunchBeam2(0),
      numBunchColliding(0),
      distanceToCollidingBCID(0),
      distanceToUnpairedB1(0),
      distanceToUnpairedB2(0),
      distanceToInboundB1(0),
      distanceToTrainStart(0),
      distanceToPreviousColliding(0)
  {
    AUX_VARIABLE( fillNumber );
    AUX_VARIABLE( machineMode );
    AUX_VARIABLE( beamMode );
    AUX_VARIABLE( beamType1 );
    AUX_VARIABLE( beamType2 );
    AUX_VARIABLE( betaStar );
    AUX_VARIABLE( crossingAngle );
    AUX_VARIABLE( stableBeams );
    AUX_VARIABLE( injectionScheme );
    AUX_VARIABLE( numBunchBeam1 );
    AUX_VARIABLE( numBunchBeam2 );
    AUX_VARIABLE( numBunchColliding );
    AUX_VARIABLE( distanceToCollidingBCID );
    AUX_VARIABLE( distanceToUnpairedB1 );
    AUX_VARIABLE( distanceToUnpairedB2 );
    AUX_VARIABLE( distanceToInboundB1 );
    AUX_VARIABLE( distanceToTrainStart );
    AUX_VARIABLE( distanceToPreviousColliding );

  }

} // namespace xAOD
