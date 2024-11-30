// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERTRIGGER_VERSIONS_FASERTRIGGERDATAAUX_V1_H
#define XAODFASERTRIGGER_VERSIONS_FASERTRIGGERDATAAUX_V1_H

// System include(s):
extern "C" {
#   include "stdint.h"
}

// xAOD include(s):
#include "xAODCore/AuxInfoBase.h"

namespace xAOD {

  /// Class holding the data handled by FaserTriggerData_v1
  ///
  /// This class holds the payload of the TLB raw data
  ///
  /// @author Eric Torrence <torrence@uoregon.edu>
  ///
  /// $Revision:  $
  /// $Date: $

  class FaserTriggerDataAux_v1 : public AuxInfoBase {

  public:
    /// Default constructor
    FaserTriggerDataAux_v1();

  private:
    /// @name TriggerLogicBoard payload
    /// @{
    uint32_t header;
    uint32_t eventId;
    uint32_t orbitId;
    uint32_t bcid;
    uint8_t inputBitsNextClk;
    uint8_t inputBits;
    uint8_t tbp;
    uint8_t tap;
    /// @}

  }; // class FaserTriggerDataAuxContainer_v1

} // namespace xAOD

// Set up the StoreGate inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::FaserTriggerDataAux_v1, xAOD::AuxInfoBase );

#endif // XAODFASERTRIGGER_VERSIONS_FASERTRIGGERDATAAUX_V1_H


