// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERWAVEFORM_VERSIONS_WAVEFORMHIT_V1_H
#define XAODFASERWAVEFORM_VERSIONS_WAVEFORMHIT_V1_H

// System include(s):
extern "C" {
#   include "stdint.h"
}

#include <vector>

// Core EDM include(s):
#include "AthContainers/AuxElement.h"
#include "Identifier/Identifier.h"

namespace xAOD {

  // Meaning of status bits
  enum WaveformStatus {
    THRESHOLD_FAILED=0,  // No hit found over threshold
    SECONDARY,           // Second or more hit found in raw waveform
    WAVE_OVERFLOW,       // Overflows removed from fit
    BASELINE_FAILED,     // Baseline determination failed
    GFIT_FAILED,         // Gaussian fit failed
    CBFIT_FAILED,        // CrystalBall fit failed
    CLOCK_INVALID,
    WAVEFORM_MISSING,    // Input waveform data missing
    WAVEFORM_INVALID     // Input waveform data length mismatch
  };

  // Cllss describing pulses in the waveform digitizer
  class WaveformHit_v1 : public SG::AuxElement {

  public:
    /// Defaullt constructor
    WaveformHit_v1();

    /// @name Access WaveformHit elements
    /// @{

    /// Waveform digitizer channel
    unsigned int channel() const;
    void set_channel(unsigned int value);

    /// 32-bit version of channel identifier
    /// From Identifier::get_identifier32()::get_compact()
    unsigned int id32() const;
    void set_id32(unsigned int value);

    /// Interface for proper identifier
    Identifier identify() const {
      return Identifier(this->id32());
    }
    void set_identifier(const Identifier& id) {
      set_id32(id.get_identifier32().get_compact());
    }

    /// All values are in units of ns and mV

    /// Best results
    float localtime() const;
    void set_localtime(float value);

    float peak() const;
    void set_peak(float value);

    float width() const;
    void set_width(float value);

    float integral() const;
    void set_integral(float value);

    /// Time from previous clock edge
    float bcid_time() const;
    void set_bcid_time(float value);

    /// Time with respect to nominal trigger time (including known offsets)
    float trigger_time() const;
    void set_trigger_time(float value);

    /// Raw values from waveform
    float raw_peak() const;
    void set_raw_peak(float value);

    float raw_integral() const;
    void set_raw_integral(float value);

    /// Bsaeline mean
    float baseline_mean() const;
    void set_baseline_mean(float value);

    /// Baseline rms
    float baseline_rms() const;
    void set_baseline_rms(float value);

    /// Status word
    unsigned int hit_status() const;
    void set_hit_status(unsigned int value);

    /// Other fit results
    float mean() const;
    void set_mean(float value);

    /// Crystal Ball fit parameters
    float alpha() const;
    void set_alpha(float value);

    float nval() const;
    void set_nval(float value);

    /// Raw time and waveform data (in ns and mV)
    std::vector<float> time_vector() const;
    void set_time_vector(std::vector<float> value);

    std::vector<float> wave_vector() const;
    void set_wave_vector(std::vector<float> value);

    /// Status bit access functions
    void set_status_bit(WaveformStatus bit) {
      this->set_hit_status(this->hit_status() | (1<<bit));
    }
    bool status_bit(WaveformStatus bit) const {
      return (this->hit_status() & (1<<bit));
    }

    bool threshold() const {
      return !(this->status_bit(xAOD::WaveformStatus::THRESHOLD_FAILED));
    }

    bool overflow() const {
      return (this->status_bit(xAOD::WaveformStatus::WAVE_OVERFLOW));
    }

    bool secondary() const {
      return this->status_bit(xAOD::WaveformStatus::SECONDARY);
    }

    /// @}

  }; // class WaveformHit_v1

  std::ostream& operator<<(std::ostream& s, const xAOD::WaveformHit_v1& hit);
}

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::WaveformHit_v1, SG::AuxElement );


#endif // XAODFASERWAVEFORM_VERSIONS_WAVEFORMHIT_V1_H
