// Dear emacs, this is -*- c++ -*-                                              

/*                                                                              
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration            
*/

#ifndef XAODFASERWAVEFORM_VERSIONS_WAVEFORMHITAUXCONTAINER_V1_H
#define XAODFASERWAVEFORM_VERSIONS_WAVEFORMHITAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

  /// Auxiliary container for WaveformHit containers

  class WaveformHitAuxContainer_v1 : public AuxContainerBase {

  public:
    /// Default constructor
    WaveformHitAuxContainer_v1();
    /// Destructor
    ~WaveformHitAuxContainer_v1() {}

  private:
    /// @name Basic variables
    ///@ {
    std::vector<unsigned int> channel;
    std::vector<unsigned int> id32;
    std::vector<float> localtime;
    std::vector<float> peak;
    std::vector<float> width;
    std::vector<float> integral;
    std::vector<float> bcid_time;

    std::vector<float> raw_peak;
    std::vector<float> raw_integral;

    std::vector<float> baseline_mean;
    std::vector<float> baseline_rms;

    std::vector<unsigned int> hit_status;

    std::vector<float> mean;
    std::vector<float> alpha;
    std::vector<float> nval;

    std::vector<std::vector<float>> time_vector;
    std::vector<std::vector<float>> wave_vector;

    ///@}

  }; // class WaveformHitAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::WaveformHitAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODFASERWAVEFORM_VERSIONS_WAVEFORMHITAUXCONTAINER_V1_H
