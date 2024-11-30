/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/**
 * @file ClockReconstructionTool.cxx
 * Implementation file for the ClockReconstructionTool.cxx
 * @ author E. Torrence, 2021
 **/

#include "ClockReconstructionTool.h"

#include "TVirtualFFT.h"

#include <vector>

// Constructor
ClockReconstructionTool::ClockReconstructionTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

// Initialization
StatusCode
ClockReconstructionTool::initialize() {
  ATH_MSG_INFO( name() << "::initalize()" );

  return StatusCode::SUCCESS;
}

// Reconstruction step
StatusCode
ClockReconstructionTool::reconstruct(const RawWaveform& raw_wave,
				     xAOD::WaveformClock* clockdata) const {

  ATH_MSG_DEBUG("Clock reconstruct called ");

  // Check the output object
  if (!clockdata) {
    ATH_MSG_ERROR("WaveformClock passed to reconstruct() is null!");
    return StatusCode::FAILURE;
  }

  // Invalid value until we know we are OK
  clockdata->set_frequency(-1.);

  // Can we determine the actual BCID we triggered in?
  //clockdata->setTriggerTime(raw_wave.trigger_time_tag());
  //ATH_MSG_DEBUG("Trigger time: " << raw_wave.trigger_time_tag());

  // Digitized clock data, sampled at 500 MHz (2 ns)
  auto counts = raw_wave.adc_counts();

  // Check that we have some minimal amount of data to work with
  if (int(counts.size()) <= m_minimumSamples.value()) {
    ATH_MSG_WARNING("Found clock waveform with length " << counts.size() << "! Not enough data to continue!");
    return StatusCode::SUCCESS;
  }

  // Need a double array for FFT
  int N = counts.size();
  std::vector<double> wave(N);
  wave.assign(counts.begin(), counts.end());

  ATH_MSG_DEBUG("Created double array with length " << wave.size() );
  ATH_MSG_DEBUG("First 10 elements:");

  for (int i=0; i < std::min(10, N); i++)
    ATH_MSG_DEBUG(" " << i << " " << wave[i]);

  // delta_nu = 1/T where T is the total waveform length
  // Multiplying (freqmult * i) gives the frequency of point i in the FFT
  double freqmult = 1./(0.002 * N); // 2ns per point, so this is in MHz

  TVirtualFFT *fftr2c = TVirtualFFT::FFT(1, &N, "R2C");
  fftr2c->SetPoints(&wave[0]);
  fftr2c->Transform();

  // Get the coefficients
  std::vector<double> re_full(N);
  std::vector<double> im_full(N);
  std::vector<double> magnitude(N/2+1);  // From fftw manual, output array is N/2+1 long 
  fftr2c->GetPointsComplex(&re_full[0], &im_full[0]);

  // Normalize the magnitude (just using the positive complex frequencies)
  unsigned int i=0;
  magnitude[i] = sqrt(pow(re_full[i], 2) + pow(im_full[i], 2))/N;
  for(i=1; i<magnitude.size(); i++) 
    magnitude[i] = 2*sqrt(pow(re_full[i], 2) + pow(im_full[i], 2))/N;

  // First, look at the DC offset
  ATH_MSG_DEBUG("DC offset: " << magnitude[0]);

  // Second, find max value (should be primary line at 40 MHz
  unsigned int imax = max_element(magnitude.begin()+1, magnitude.end()) - magnitude.begin();

  if (((int(imax)-1) < 0) || ((imax+1) >= magnitude.size())) {
    ATH_MSG_WARNING("Found maximum frequency amplitude at postion " << imax << "!");
  } else {
    ATH_MSG_DEBUG("Magnitude array at peak:");
    for(i = imax-1; i <= imax+1; i++)
      ATH_MSG_DEBUG("Index: " << i << " Freq: " << i*freqmult << " Mag: " << magnitude[i]);
  }

  // Store results (amplitides in mV)
  clockdata->set_dc_offset(raw_wave.mv_per_bit()*magnitude[0]);
  clockdata->set_amplitude(raw_wave.mv_per_bit()*magnitude[imax]);
  clockdata->set_frequency(imax * freqmult);
  clockdata->set_phase(atan2(im_full[imax], re_full[imax])); // Not a bug, atan2(y,x)!

  ATH_MSG_DEBUG("Before correcting for finite resolution:");
  ATH_MSG_DEBUG(*clockdata);

  // Correct for the windowing to find the exact peak frequency
  // Following: https://indico.cern.ch/event/132526/contributions/128902/attachments/99707/142376/Meeting1-06-11_FFT_corrections_for_tune_measurements.pdf

  double dm;  // Fraction of integer frequency where real peak sits
  if (magnitude[imax+1] >= magnitude[imax-1]) { // Past ipeak by dm
    dm = magnitude[imax+1] / (magnitude[imax] + magnitude[imax + 1]);
  } else { // Before ipeak by dm
    dm = -magnitude[imax-1] / (magnitude[imax-1] + magnitude[imax]);
  }

  ATH_MSG_DEBUG("Found shift in frequency index: " << dm);

  // Improved values

  // Not a bug, atan2(y,x)!
  double phase = atan2(im_full[imax], re_full[imax]) - dm * M_PI;
  // Fix any overflows caused by adding dm
  if (phase < -M_PI) phase += (2*M_PI);
  if (phase > +M_PI) phase -= (2*M_PI);

  clockdata->set_frequency( (imax+dm) * freqmult );
  clockdata->set_phase (phase);
  clockdata->set_amplitude( raw_wave.mv_per_bit() * 2*M_PI*dm*magnitude[imax] / sin(M_PI * dm) );

  ATH_MSG_DEBUG("After correcting for finite resolution:");
  ATH_MSG_DEBUG(*clockdata);

  delete fftr2c;

  // Check the result for sanity
  if (clockdata->amplitude() < m_amplitude_min || clockdata->frequency() < m_frequency_min || clockdata->frequency() > m_frequency_max) {
    ATH_MSG_WARNING("Clock reconstructed with frequency = " << clockdata->frequency() << " amplitude = " << clockdata->amplitude() << "!");
  }

  if (m_checkResult) checkResult(raw_wave, clockdata);

  return StatusCode::SUCCESS;
}

void
ClockReconstructionTool::checkResult(const RawWaveform& raw_wave,
				     xAOD::WaveformClock* clockdata) const {

  // Go through each element in raw_wave and make sure time in clockdata matches

  float time;
  for (unsigned int i=0; i<raw_wave.adc_counts().size(); i++) {
    time = 2.*i; // Time in ns

    float dt = clockdata->time_from_clock(time);

    // Is raw_wave HI or LO?
    bool hi = raw_wave.adc_counts()[i] > clockdata->dc_offset();

    // Check for mismatch
    if (((dt > 0.05) && (dt < 12.45) && !hi) || ((dt > 12.55) && (dt < 24.95) && hi) ) 
      ATH_MSG_WARNING("Clock Time: " << time << " Time to edge: "  << dt << " found raw data: " << hi);
  }

}
