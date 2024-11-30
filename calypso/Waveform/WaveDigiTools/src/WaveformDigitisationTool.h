/*
   Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/** @file WaveformDigitisationTool.h
 *  Header file for WaveformDigitisationTool.h
 *
 */
#ifndef WAVEDIGITOOLS_WAVEFORMDIGITISATIONTOOL_H
#define WAVEDIGITOOLS_WAVEFORMDIGITISATIONTOOL_H

//Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "WaveDigiTools/IWaveformDigitisationTool.h"
#include "WaveformConditionsTools/IWaveformDigiConditionsTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"

// Data classes
#include "ScintSimEvent/ScintHitCollection.h"

// Helpers
#include "ScintIdentifier/VetoID.h"
#include "ScintIdentifier/VetoNuID.h"
#include "ScintIdentifier/TriggerID.h"
#include "ScintIdentifier/PreshowerID.h"
#include "FaserCaloIdentifier/EcalID.h"

// Detector Managers
#include "ScintReadoutGeometry/VetoDetectorManager.h"
#include "ScintReadoutGeometry/VetoNuDetectorManager.h"
#include "ScintReadoutGeometry/TriggerDetectorManager.h"
#include "ScintReadoutGeometry/PreshowerDetectorManager.h"
#include "CaloReadoutGeometry/EcalDetectorManager.h"

// ROOT
#include "TF1.h"

//STL

class WaveformDigitisationTool: public extends<AthAlgTool, IWaveformDigitisationTool> {
 public:

  /// Normal constructor for an AlgTool; 'properties' are also declared here
 WaveformDigitisationTool(const std::string& type, 
			  const std::string& name, const IInterface* parent);

  /// Retrieve the necessary services in initialize
  StatusCode initialize();

  /// Evaluate time kernel over samples  
  std::vector<float> evaluate_timekernel(TF1* kernel) const;

  /// Generate random baseline 
  float generate_baseline(float mean, float rms) const;

  /// Fill EvsT histograms
  void fill_evst_hist(EvstHistMap& map, const Identifier& id, float time, float energy) const;

  void clear_evst_hist(EvstHistMap& map) const;

  /// Waveform digitization parameters
  FloatProperty m_digitizerPeriod { this, "DigitizerPeriod", 2.0, "Duration of one sample (in ns)" };
  IntegerProperty m_digitizerSamples { this, "DigitizerSamples", 600, "Number of digitizer samples" };

  /// This is used to set the time scale over which the hit energy 
  /// is histogramed before convoluting with the time kernel
  IntegerProperty m_overSamples { this, "OverSamples", 20, "Oversampling of hit times compared to digitizer period" };

  /// Digitizer samples to start before t0 (time = preSamples * digiPeriod)
  IntegerProperty  m_preSamples { this, "PreSamples", 10, "Presamples for energy histogram" };

  /// Access functions for parameters
  unsigned int digitizer_samples() const { return m_digitizerSamples; }
  float digitizer_period() const { return m_digitizerPeriod; }
  unsigned int over_samples() const { return m_overSamples; }
  unsigned int pre_samples() const {return m_preSamples; }

  ///
  /// New functions to handle entire digitization process

  //template <class HitCollection>
  //std::map<Identifier, std::vector<uint16_t>>& generateWaveforms(
  //		      const EventContext& ctx, 
  //		      const ToolHandle<IWaveformDigiConditionsTool>&, 
  //		      const HitCollection*) const;

  /// Simple (old-style) digitization
  std::map<Identifier, std::vector<uint16_t>>& generate_scint_waveforms(
		      const EventContext& ctx, 
		      const ToolHandle<IWaveformDigiConditionsTool>&, 
		      const ScintHitCollection*) const;

  /// Less simple (new-style) digitization with timing
  std::map<Identifier, std::vector<uint16_t>>& generate_scint_timing_waveforms(
		      const EventContext& ctx, 
		      const ToolHandle<IWaveformDigiConditionsTool>&, 
		      const ScintHitCollection*) const;

  /// Simple (old-style) digitization
  std::map<Identifier, std::vector<uint16_t>>& generate_calo_waveforms(
		      const EventContext& ctx, 
		      const ToolHandle<IWaveformDigiConditionsTool>&, 
		      const CaloHitCollection*) const;

  /// Less simple (new-style) digitization with timing
  std::map<Identifier, std::vector<uint16_t>>& generate_calo_timing_waveforms(
		      const EventContext& ctx, 
		      const ToolHandle<IWaveformDigiConditionsTool>&, 
		      const CaloHitCollection*) const;

 private:

  template <class ID, class DM>
    std::map<Identifier, float> 
    create_tof_map(const ID* idHelper, const DM* detman) const;

  std::map<Identifier, float> 
    create_calo_tof_map(const EcalID* idHelper, const CaloDD::EcalDetectorManager* detman) const;

  TF1* create_kernel(const EventContext& ctx, 
		     const ToolHandle<IWaveformDigiConditionsTool>& digiCondTool) const;

  void convolve_waveforms(
    const EventContext& ctx,
    const ToolHandle<IWaveformDigiConditionsTool>& digiCondTool,
    const EvstHistMap& evst_hist,
    std::map<Identifier, std::vector<uint16_t>>& waveforms) const;

 private:

  /// ID helpers
  const VetoID* m_vetoID{nullptr};
  const VetoNuID* m_vetoNuID{nullptr};
  const TriggerID* m_triggerID{nullptr};
  const PreshowerID* m_preshowerID{nullptr};
  const EcalID* m_ecalID{nullptr};

  /// Detector manager helpers
  const ScintDD::VetoDetectorManager* m_vetoDetMan{nullptr};
  const ScintDD::VetoNuDetectorManager* m_vetoNuDetMan{nullptr};
  const ScintDD::TriggerDetectorManager* m_triggerDetMan{nullptr};
  const ScintDD::PreshowerDetectorManager* m_preshowerDetMan{nullptr};
  const CaloDD::EcalDetectorManager* m_caloDetMan{nullptr};
};

#endif // WAVEDIGITOOLS_WAVEFORMDIGITISATIONTOOL_H
