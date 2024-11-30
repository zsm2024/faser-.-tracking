/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

// $Id: $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODFaserLHC/versions/FaserLHCData_v1.h"

namespace xAOD {

  FaserLHCData_v1::FaserLHCData_v1() 
    : SG::AuxElement() {
  }

  /////////////////////////////////////////////////////////////////////////////
  //
  //              Implementation for the start time functions
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, unsigned int,
					fillNumber, set_fillNumber )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, std::string,
					machineMode, set_machineMode )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, std::string, 
					beamMode, set_beamMode )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, int,
					beamType1, set_beamType1 )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, int,
					beamType2, set_beamType2 )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, float, 
					betaStar, set_betaStar )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, float, 
					crossingAngle, set_crossingAngle )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, bool, 
					stableBeams, set_stableBeams )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, std::string, 
					injectionScheme, set_injectionScheme )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, unsigned int, 
					numBunchBeam1, set_numBunchBeam1 )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, unsigned int, 
					numBunchBeam2, set_numBunchBeam2 )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, unsigned int, 
					numBunchColliding, set_numBunchColliding )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, int, 
					distanceToCollidingBCID, set_distanceToCollidingBCID )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, int, 
					distanceToUnpairedB1, set_distanceToUnpairedB1 )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, int, 
					distanceToUnpairedB2, set_distanceToUnpairedB2 )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, int, 
					distanceToInboundB1, set_distanceToInboundB1 )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, unsigned int, 
					distanceToTrainStart, set_distanceToTrainStart )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserLHCData_v1, unsigned int, 
					distanceToPreviousColliding, set_distanceToPreviousColliding )



} // namespace xAOD

namespace xAOD {

  std::ostream& operator<<(std::ostream& s, const xAOD::FaserLHCData_v1& td) {
    s << "xAODFaserLHCData: fill=" << td.fillNumber()
      << " beamMode=" << td.beamMode()
      << " betaStar=" << td.betaStar()
      << " distanceToCollidingBCID=" << td.distanceToCollidingBCID()
      << std::endl;

    return s;
  }

} // namespace xAOD
