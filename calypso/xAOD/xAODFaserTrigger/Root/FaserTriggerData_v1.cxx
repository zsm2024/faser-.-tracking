/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

// $Id: $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODFaserTrigger/versions/FaserTriggerData_v1.h"
#include <bitset>

namespace xAOD {

  FaserTriggerData_v1::FaserTriggerData_v1() 
    : SG::AuxElement() {
  }

  /////////////////////////////////////////////////////////////////////////////
  //
  //              Implementation for the start time functions
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserTriggerData_v1, uint32_t, 
					header, setHeader )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserTriggerData_v1, uint32_t, 
					eventId, setEventId )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserTriggerData_v1, uint32_t, 
					orbitId, setOrbitId )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserTriggerData_v1, uint32_t, 
					bcid, setBcid )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserTriggerData_v1, uint8_t, 
					inputBitsNextClk, setInputBitsNextClk )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserTriggerData_v1, uint8_t, 
					inputBits, setInputBits )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserTriggerData_v1, uint8_t, 
					tbp, setTbp )

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( FaserTriggerData_v1, uint8_t, 
					tap, setTap )

} // namespace xAOD

namespace xAOD {

  std::ostream& operator<<(std::ostream& s, const xAOD::FaserTriggerData_v1& td) {
    s << "xAODFaserTriggerData: eventID=" << td.eventId() 
      << " orbitID=" << td.orbitId() 
      << " BCID=" << td.bcid() 
      << " Input Bits (next clk)=" << std::bitset<8>(td.inputBits()) << " (" 
      << std::bitset<8>(td.inputBitsNextClk()) << ")"
      << " TBP/TAP=" << std::bitset<6>(td.tbp()) << " / " 
      << std::bitset<6>(td.tap())  
      << std::endl;

    return s;
  }

} // namespace xAOD
