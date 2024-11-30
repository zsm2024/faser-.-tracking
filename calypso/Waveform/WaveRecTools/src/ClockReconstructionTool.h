/*
   Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/** @file ClockReconstructionTool.h
 *  Header file for ClockReconstructionTool.h
 *
 */
#ifndef WAVERECTOOLS_CLOCKRECONSTRUCTIONTOOL_H
#define WAVERECTOOLS_CLOCKRECONSTRUCTIONTOOL_H

//Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "WaveRecTools/IClockReconstructionTool.h"

#include "WaveRawEvent/RawWaveform.h"
#include "xAODFaserWaveform/WaveformClock.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

//STL
#include <string>
#include <vector>

class ClockReconstructionTool: public extends<AthAlgTool, IClockReconstructionTool> {
 public:

  /// Normal constructor for an AlgTool; 'properties' are also declared here
  ClockReconstructionTool(const std::string& type, 
			  const std::string& name, const IInterface* parent);

  /// Retrieve the necessary services in initialize
  StatusCode initialize();

  /// Reconstruct hits from clock  
  virtual StatusCode reconstruct(const RawWaveform& wave,
				 xAOD::WaveformClock* clockdata) const;

 private:

  //
  // Parameters

  /// Minimum samples in the input waveform array to try FFT
  IntegerProperty m_minimumSamples{this, "MinimumSamples", 40};

  /// Check reconstructed clock against waveform
  BooleanProperty m_checkResult{this, "CheckResult", false};

  void checkResult(const RawWaveform& raw_wave,
		   xAOD::WaveformClock* clockdata) const;

  // Limits to print warnings (amplitude in mV)
  FloatProperty m_amplitude_min{this, "AmplitudeMin", 500.};
  FloatProperty m_frequency_min{this, "FrequencyMin", 40.0};
  FloatProperty m_frequency_max{this, "FrequencyMax", 40.1};

};

#endif // WAVERECTOOLS_CLOCKRECONSTRUCTIONTOOL_H
