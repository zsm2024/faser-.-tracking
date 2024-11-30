// Dear emacs, this is -*- c++ -*-                                              

/*                                                                              
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration            
*/

#ifndef XAODFASERCALORIMETER_VERSIONS_CALORIMETERHITAUXCONTAINER_V1_H
#define XAODFASERCALORIMETER_VERSIONS_CALORIMETERHITAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODFaserWaveform/WaveformHitContainer.h"

namespace xAOD {

  /// Auxiliary container for CalorimeterHit containers

  class CalorimeterHitAuxContainer_v1 : public AuxContainerBase {

  public:
    /// Default constructor
    CalorimeterHitAuxContainer_v1();
    /// Destructor
    ~CalorimeterHitAuxContainer_v1() {}

  private:
    /// @name Basic variables
    ///@ {
    std::vector<unsigned int> channel;
    std::vector<float> Nmip;
    std::vector<float> E_dep;
    std::vector<float> E_EM;
    std::vector<float> fit_to_raw_ratio;

    typedef std::vector< ElementLink< WaveformHitContainer > > WaveformHitLink_t;
    std::vector< WaveformHitLink_t > WaveformLink;
    //std::vector< WaveformHitLink_t > caloLinks;
    //std::vector< WaveformHitLink_t > preshowerLinks;

    ///@}

  }; // class CalorimeterHitAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CalorimeterHitAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODFASERCALORIMETER_VERSIONS_CALORIMETERHITAUXCONTAINER_V1_H
