// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERTRIGGER_VERSIONS_FASERTRIGGERDATA_V1_H
#define XAODFASERTRIGGER_VERSIONS_FASERTRIGGERDATA_V1_H

// System include(s):
extern "C" {
#   include "stdint.h"
}

// Core EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

  /// Class describing the Faser Trigger Logic Board (TLB) raw data
  ///
  ///
  class FaserTriggerData_v1 : public SG::AuxElement {

  public:
    /// Default constructor
    FaserTriggerData_v1();

    /// @name Access TLB elements
    /// @{

    /// The first raw word
    uint32_t header() const;
    void setHeader(uint32_t value);
 
    /// Event ID
    uint32_t eventId() const;
    void setEventId(uint32_t value);

    /// Orbit ID
    uint32_t orbitId() const;
    void setOrbitId(uint32_t value);

    /// Bunch Crossing
    uint32_t bcid() const;
    void setBcid(uint32_t value);

    /// Input bits
    uint8_t inputBitsNextClk() const;
    void setInputBitsNextClk(uint8_t value);

    uint8_t inputBits() const;
    void setInputBits(uint8_t value);

    /// Trigger counts (before prescale, after prescale)
    uint8_t tbp() const;
    void setTbp(uint8_t value);

    uint8_t tap() const;
    void setTap(uint8_t value);

    /// @}

    /// @name Set function
    /// @{

    void setTriggerData( const uint32_t* data, size_t size );

    /// @}

  }; // class FaserTriggerData_v1

  std::ostream& operator<<(std::ostream& s, const xAOD::FaserTriggerData_v1& td);

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::FaserTriggerData_v1, SG::AuxElement );

#endif // XAODFASERTRIGGER_VERSIONS_FASERTRIGGERDATA_V1_H
