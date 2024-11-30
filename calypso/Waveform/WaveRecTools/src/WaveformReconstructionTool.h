/*
   Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/** @file WaveformReconstructionTool.h
 *  Header file for WaveformReconstructionTool.h
 *
 */
#ifndef WAVERECTOOLS_WAVEFORMRECONSTRUCTIONTOOL_H
#define WAVERECTOOLS_WAVEFORMRECONSTRUCTIONTOOL_H

//Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "WaveRecTools/IWaveformReconstructionTool.h"

#include "WaveRawEvent/RawWaveform.h"
#include "WaveRawEvent/RawWaveformContainer.h"
#include "xAODFaserWaveform/WaveformHit.h"

#include "WaveformBaselineData.h"
#include "WaveformFitResult.h"

// Tool classes
#include "WaveformConditionsTools/IWaveformTimingTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

//STL
#include <string>
#include <vector>

class WaveformReconstructionTool: public extends<AthAlgTool, IWaveformReconstructionTool> {
 public:

  /// Normal constructor for an AlgTool; 'properties' are also declared here
  WaveformReconstructionTool(const std::string& type, 
			     const std::string& name, const IInterface* parent);

  /// Retrieve the necessary services in initialize
  StatusCode initialize();

  /// Reconstruct primary hits from waveform (in trigger window)
  virtual StatusCode reconstructPrimary(const RawWaveform& wave,
					xAOD::WaveformHitContainer* hitContainer) const;

  /// Reconstruct primary hits from waveform (in trigger window)
  virtual StatusCode reconstructSecondary(const RawWaveform& wave,
					xAOD::WaveformHitContainer* hitContainer) const;

  /// Set local hit times from LHC clock
  virtual StatusCode setLocalTime(const xAOD::WaveformClock* clock,
				  xAOD::WaveformHitContainer* container) const;


 private:

  //
  // Baseline Estimation Parameters
  BooleanProperty m_useSimpleBaseline{this, "UseSimpleBaseline", false};

  ToolHandle<IWaveformTimingTool> m_timingTool
    {this, "WaveformTimingTool", "WaveformTimingTool"};

  // Minimum number of samples needed to calculate simple baseline
  // Just average these first n values
  IntegerProperty m_samplesForBaselineAverage{this, "SamplesForBaselineAverage", 40};

 
  //
  // Parameters for advanced baseline determination
  // Parameters of initial histogram to find most likely ADC reading
  // Range and bins to use, ratio should be an integer (bin width)
  IntegerProperty m_baselineRange{this, "BaselineRange", 16000};
  IntegerProperty m_baselineRangeBins{this, "BaselineRangeBins", 320};

  // Range of samples to use to find baseline (to avoid signal contamination)
  // In samples (2 ns intervals) so 350 -> 700 ns in digitizer window
  IntegerProperty m_baselineSampleLo{this, "BaselineSampleLo", 0};
  // Change this from 350 -> 75 for 2023 data, allows shorter readout
  IntegerProperty m_baselineSampleHi{this, "BaselineSampleHi", 75};

  // 
  // Parameters for the Gaussian fit to the baseline peak (in counts)
  // Range is total range to use to find truncated mean and width
  IntegerProperty m_baselineFitRange{this, "BaselineFitRange", 200};
  // Fit window is value (in sigma) of trucated width to use in final fit
  FloatProperty m_baselineFitWindow{this, "BaselineFitWindow", 2.};

  //
  // Peak threshold (in sigma of baseline RMS)
  // Primary threshold is requirement to try a fit for the in-time window
  // Secondary threshold is requirement to produce a secondary hit
  // from a local maximum
  FloatProperty m_primaryPeakThreshold{this, "PrimaryPeakThreshold", 5.};
  FloatProperty m_secondaryPeakThreshold{this, "SecondaryPeakThreshold", 10.};
 
  //
  // Window to define fitting range, in samples (2ns/sample)
  // Make this longer by default, from 120 to 150 ns
  IntegerProperty m_windowStart{this, "FitWindowStart", -20};  
  IntegerProperty m_windowWidth{this, "FitWindowWidth", 75};

  //
  // Remove overflow values from CB fit
  BooleanProperty m_removeOverflow{this, "RemoveOverflow", true};

  //
  // Fraction of peak to set local hit time
  FloatProperty m_timingPeakFraction{this, "TimingPeakFraction", 0.4};

  //
  // When looking for secondary hits with a primary found above threshold
  // should we look before or after the primary hit?
  BooleanProperty m_findSecondaryBefore{this, "FindSecondaryBefore", true};
  BooleanProperty m_findSecondaryAfter{this, "FindSecondaryAfter", false};

  // Reco algorithms
  // Fill hit with raw data from waveform
  void fillRawHitValues(const RawWaveform& wave,
			int lo_edge, int hi_edge,
			xAOD::WaveformHit* hit) const;

  // Perform fits to WaveformHit data
  void reconstructHit(xAOD::WaveformHit* hit) const;

  // Baseline algorithms
  WaveformBaselineData& findSimpleBaseline(const RawWaveform& wave) const;
  WaveformBaselineData& findAdvancedBaseline(const RawWaveform& wave) const;
  WaveformBaselineData& findBaseline(const RawWaveform& wave, 
				     xAOD::WaveformHit* hit) const;


  // Find peak in wave, return index to peak position, or -1 if 
  // peak isn't greater than threshold
  int findPeak(WaveformBaselineData& baseline, float threshold, 
		std::vector<float>& wave) const;

  // Get estimate from waveform data itself
  WaveformFitResult& findRawHitValues(const std::vector<float> time, 
				      const std::vector<float> wave) const;

  // Fit windowed data to Gaussian (to get initial estimate of parameters
  WaveformFitResult& fitGaussian(const WaveformFitResult& raw,
				 const std::vector<float> time, 
				 const std::vector<float> wave) const;

  // Find overflows and remove points from arrays
  bool findOverflow(float baseline, 
		    std::vector<float>& time, std::vector<float>& wave) const;

  // Fit windowed data to CrystalBall function
  WaveformFitResult& fitCBall(const WaveformFitResult& gfit, 
			      const std::vector<float> time, 
			      const std::vector<float> wave) const;

};

#endif // WAVERECTOOLS_WAVEFORMRECONSTRUCTIONTOOL_H
