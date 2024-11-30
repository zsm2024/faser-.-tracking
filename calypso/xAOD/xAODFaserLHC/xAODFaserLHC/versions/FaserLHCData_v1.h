// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERLHC_VERSIONS_FASERLHCDATA_V1_H
#define XAODFASERLHC_VERSIONS_FASERLHCDATA_V1_H

// System include(s):
extern "C" {
#   include "stdint.h"
}

// Core EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

  /// Class describing the Faser LHC information
  ///
  ///
  class FaserLHCData_v1 : public SG::AuxElement {

  public:
    /// Default constructor
    FaserLHCData_v1();

    /// @name Access LHC elements
    /// @{

    /// First batch comes from ATLAS /LHC/DCS/FILLSTATE folder
    /// which is a copy of information sent to ATLAS by LHC over DIP
    /// This is from the FASER COOL folder /LHC/BeamData

    /// LHC fill number
    unsigned int fillNumber() const;
    void set_fillNumber(unsigned int value);

    /// LHC machine mode (i.e. PROTON PHYSICS)
    std::string machineMode() const; 
    void set_machineMode(std::string value);

    /// LHC Beam Mode (i.e. STABLE BEAMS)
    std::string beamMode() const; 
    void set_beamMode(std::string value);

    /// LHC Beam type (charge of particle, 1-proton, 82-lead)
    int beamType1() const;
    void set_beamType1(int value);

    int beamType2() const;
    void set_beamType2(int value);

    // Beta* at IP1, in cm
    float betaStar() const;
    void set_betaStar(float value);

    // Crossing angle at IP1, in uRad
    float crossingAngle() const;
    void set_crossingAngle(float value);

    // LHC stable beam flag
    bool stableBeams() const;
    void set_stableBeams(bool value);

    // Injection scheme
    std::string injectionScheme() const;
    void set_injectionScheme(std::string value);

    // Second batch is information is related to the BCID structure
    // This comes from the LPC filling scheme (nominal values)
    // and is stored in /LHC/BCIDData

    /// Nominal bunches (derived from DIP "Circulating Bunch Configuration")
    unsigned int numBunchBeam1() const;
    void set_numBunchBeam1(unsigned int value);
    unsigned int numBunchBeam2() const;
    void set_numBunchBeam2(unsigned int value);
    unsigned int numBunchColliding() const;
    void set_numBunchColliding(unsigned int value);

    // Number of BCIDs difference between current event BCID (from EventInfo)
    // and a colliding BCID in the nominal bunch pattern

    int distanceToCollidingBCID() const;
    void set_distanceToCollidingBCID(int value);

    int distanceToUnpairedB1() const;
    void set_distanceToUnpairedB1(int value);

    int distanceToUnpairedB2() const;
    void set_distanceToUnpairedB2(int value);

    int distanceToInboundB1() const;
    void set_distanceToInboundB1(int value);

    /// Distance of current BCID to start of previous bunch train
    unsigned int distanceToTrainStart() const;
    void set_distanceToTrainStart(unsigned int value);

    /// Distance of current BCID frmo previous colliding BCID (0 if this is colliding)
    unsigned int distanceToPreviousColliding() const;
    void set_distanceToPreviousColliding(unsigned int value);

    /// Simple boolean functions to access useful values
    /// In most cases, these check that the distance above is zero
    bool isStable() const { return this->stableBeams(); }
    bool isColliding() const { return this->distanceToCollidingBCID() == 0; }
    bool isUnpairedBeam1() const { return this->distanceToUnpairedB1() == 0; }
    bool isUnpairedBeam2() const { return this->distanceToUnpairedB2() == 0; }
    bool isInboundBeam1() const { return this->distanceToInboundB1() == 0; }

    /// @}


  }; // class FaserLHCData_v1

  std::ostream& operator<<(std::ostream& s, const xAOD::FaserLHCData_v1& td);

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::FaserLHCData_v1, SG::AuxElement );

#endif // XAODFASERLHC_VERSIONS_FASERLHCDATA_V1_H
