// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERCALORIMETER_VERSIONS_CALORIMETERHIT_V1_H
#define XAODFASERCALORIMETER_VERSIONS_CALORIMETERHIT_V1_H

// System include(s):
extern "C" {
#   include "stdint.h"
}

#include <vector>

// Core EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// xAOD include(s):
#include "xAODFaserWaveform/WaveformHit.h"
#include "xAODFaserWaveform/WaveformHitContainer.h"

namespace xAOD {

  // Cllss describing pulses in the calorimeter digitizer
  class CalorimeterHit_v1 : public SG::AuxElement {

  public:
    /// Defaullt constructor
    CalorimeterHit_v1();

    /// @name Access CalorimeterHit elements
    /// @{

    /// Best results
    unsigned int channel() const;
    void set_channel(unsigned int value);

    float Nmip() const;
    void set_Nmip(float value);

    float E_dep() const;
    void set_E_dep(float value);

    float E_EM() const;
    void set_E_EM(float value);

    float fit_to_raw_ratio() const;
    void set_fit_to_raw_ratio(float value);

    // Waveform Hits
    typedef std::vector< ElementLink< xAOD::WaveformHitContainer > > WaveformHitLinks_t;

    // Contributing Calorimeter Waveform Hits
    const WaveformHitLinks_t& WaveformLinks() const;
    void setWaveformLinks( const WaveformHitLinks_t& Waveforms );
    // Remove all waveform hits
    void clearWaveformLinks();
    // Get the pointer to a given waveform hit
    const WaveformHit* Hit( size_t i ) const;
    // Get the number of waveform hits
    size_t nHits() const;
    // Add a waveform hit
    void addHit( const xAOD::WaveformHitContainer*, const xAOD::WaveformHit*);

    // // Contributing Preshower Waveform Hits
    // const WaveformHitLinks_t& preshowerWaveformLinks() const;
    // void setPreshowerWaveformLinks( const WaveformHitLinks_t& preshowerWaveforms );
    // // Remove all waveform hits
    // void clearPreshowerWaveformLinks();
    // // Get the pointer to a given waveform hit
    // const WaveformHit* preshowerHit( size_t i ) const;
    // // Get the number of waveform hits
    // size_t nPreshowerHits() const;
    // // Add a waveform hit
    // void addPreshowerHit( const xAOD::WaveformHitContainer*, const xAOD::WaveformHit*);

    /// @}

  }; // class CalorimeterHit_v1

  std::ostream& operator<<(std::ostream& s, const xAOD::CalorimeterHit_v1& hit);
}

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CalorimeterHit_v1, SG::AuxElement );


#endif // XAODFASERCALORIMETER_VERSIONS_CALORIMETERHIT_V1_H
