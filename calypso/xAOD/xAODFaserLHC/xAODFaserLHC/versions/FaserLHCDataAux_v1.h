// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERLHC_VERSIONS_FASERLHCDATAAUX_V1_H
#define XAODFASERLHC_VERSIONS_FASERLHCDATAAUX_V1_H

// System include(s):
extern "C" {
#   include "stdint.h"
}

// xAOD include(s):
#include "xAODCore/AuxInfoBase.h"

namespace xAOD {

  /// Class holding the data handled by FaserLHCData_v1
  ///
  /// This class holds the payload of the TLB raw data
  ///
  /// @author Eric Torrence <torrence@uoregon.edu>
  ///
  /// $Revision:  $
  /// $Date: $

  class FaserLHCDataAux_v1 : public AuxInfoBase {

  public:
    /// Default constructor
    FaserLHCDataAux_v1();

  private:
    /// @name LHCData payload
    /// @{
    unsigned int fillNumber;
    std::string machineMode;
    std::string beamMode;
    int beamType1;
    int beamType2;

    float betaStar;
    float crossingAngle;

    bool stableBeams;
    std::string injectionScheme;

    unsigned int numBunchBeam1;
    unsigned int numBunchBeam2;
    unsigned int numBunchColliding;

    int distanceToCollidingBCID;
    int distanceToUnpairedB1;
    int distanceToUnpairedB2;
    int distanceToInboundB1;

    unsigned int distanceToTrainStart;
    unsigned int distanceToPreviousColliding;

    /// @}

  }; // class FaserLHCDataAuxContainer_v1

} // namespace xAOD

// Set up the StoreGate inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::FaserLHCDataAux_v1, xAOD::AuxInfoBase );

#endif // XAODFASERLHC_VERSIONS_FASERLHCDATAAUX_V1_H


