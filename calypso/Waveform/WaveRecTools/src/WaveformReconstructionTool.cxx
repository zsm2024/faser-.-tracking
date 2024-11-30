/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/**
 * @file WaveformReconstructionTool.cxx
 * Implementation file for the WaveformReconstructionTool.cxx
 * @ author E. Torrence, 2021
 **/

#include "WaveformReconstructionTool.h"

#include "TH1F.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraph.h"
#include "TError.h"

#include <vector>
#include <tuple>
#include <math.h>

// Constructor
WaveformReconstructionTool::WaveformReconstructionTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

// Initialization
StatusCode
WaveformReconstructionTool::initialize() {
  ATH_MSG_INFO( name() << "::initalize()" );

  gErrorIgnoreLevel = kFatal;  // STFU!

  if (m_useSimpleBaseline.value()) {
    ATH_MSG_INFO("Will use simple baseline estimation");
  } else {
    ATH_MSG_INFO("Will use fit to determine baseline");
  }

  ATH_CHECK( m_timingTool.retrieve() );

  return StatusCode::SUCCESS;
}

//
// Form primary hits using trigger time
//
StatusCode
WaveformReconstructionTool::reconstructPrimary(
	 const RawWaveform& wave,
	 xAOD::WaveformHitContainer* hitContainer) const {

  ATH_MSG_DEBUG(" reconstructPrimary called");

  xAOD::WaveformHit* newhit = new xAOD::WaveformHit();
  hitContainer->push_back(newhit);

  // Set digitizer channel and identifier
  newhit->set_channel(wave.channel());
  newhit->set_identifier(wave.identify());

  // Make sure we have ADC counts
  if (wave.adc_counts().size() == 0) {
    ATH_MSG_WARNING( "Found waveform for channel " << wave.channel() 
		     << " with size " << wave.adc_counts().size() << "!");
	
    newhit->set_status_bit(xAOD::WaveformStatus::WAVEFORM_MISSING);
    return StatusCode::SUCCESS;
  } 

  if (wave.adc_counts().size() != wave.n_samples()) {
    ATH_MSG_WARNING( "Found waveform for channel " << wave.channel() 
		     << " with size " << wave.adc_counts().size() 
		     << " not equal to number of samples " << wave.n_samples());
	
    newhit->set_status_bit(xAOD::WaveformStatus::WAVEFORM_INVALID);
    return StatusCode::SUCCESS;
  }

  // Find the baseline for this waveform
  findBaseline(wave, newhit);

  // Check for problems
  if (newhit->status_bit(xAOD::WaveformStatus::BASELINE_FAILED)) 
    return StatusCode::SUCCESS;

  // Get the nominal trigger time (in ns) from config
  float trigger_time = m_timingTool->nominalTriggerTime();

  // Set range for windowed data in digitizer samples
  float offset = m_timingTool->triggerTimeOffset(wave.channel());

  int lo_edge = int((trigger_time+offset)/2.) + m_windowStart;
  int hi_edge = int((trigger_time+offset)/2.) + m_windowStart + m_windowWidth;

  if (hi_edge >= int(wave.size())) {
    // This likely means we have the wrong digitizer range
    ATH_MSG_WARNING("Found channel " << wave.channel() << " with low edge: " << lo_edge << " hi edge: " << hi_edge << " > wave.size() " << wave.size());
    ATH_MSG_WARNING("  trigger_time + offset: " << (trigger_time+offset) << " => " << int((trigger_time+offset)/2.));
    newhit->set_status_bit(xAOD::WaveformStatus::WAVEFORM_INVALID);
    return StatusCode::SUCCESS;
  }
  
  // Fill raw hit values
  fillRawHitValues(wave, lo_edge, hi_edge, newhit);

  // Check if this is over threshold
  if (newhit->peak() < newhit->baseline_rms() * m_primaryPeakThreshold) {
    ATH_MSG_DEBUG("Primary hit failed threshold");
    newhit->set_status_bit(xAOD::WaveformStatus::THRESHOLD_FAILED);
  } else {
    // Reconstruct hit in this range
    reconstructHit(newhit);
  }

  return StatusCode::SUCCESS;
}

//
// Form primary hits using trigger time
//
StatusCode
WaveformReconstructionTool::reconstructSecondary(
	 const RawWaveform& wave,
	 xAOD::WaveformHitContainer* hitContainer) const {

  ATH_MSG_DEBUG(" reconstructSecondary called");

  // Find existing hit for this channel to get baseline
  xAOD::WaveformHit* primaryHit = NULL;

  for( const auto& hit : *hitContainer) {

    // Use id rather than channel to make sure this works on MC
    if (hit->identify() == wave.identify()) {
      ATH_MSG_DEBUG("Found primary hit in channel "<< hit->channel() 
		    << " with id 0x" << std::hex << hit->identify() << std::dec );
      primaryHit = hit;
      break;
    }
  }

  // Did we find the primary hit for this channel?
  if (!primaryHit) {
    ATH_MSG_ERROR("found no primary hit for channel " << wave.channel() << "!");
    return StatusCode::FAILURE;
  }

  if (primaryHit->status_bit(xAOD::WaveformStatus::WAVEFORM_MISSING)) {
    ATH_MSG_DEBUG("Found primary hit with waveform missing");
    return StatusCode::SUCCESS;
  }

  if (primaryHit->status_bit(xAOD::WaveformStatus::WAVEFORM_INVALID)) {
    ATH_MSG_DEBUG("Found primary hit with waveform invalid");
    return StatusCode::SUCCESS;
  }

  WaveformBaselineData baseline;

  baseline.mean = primaryHit->baseline_mean();
  baseline.rms = primaryHit->baseline_rms();
  
  // Find the secondary peak position
  int ipeak;

  // Is there already a peak in the primary?
  if (primaryHit->threshold()) {

    ATH_MSG_DEBUG("Looking for secondary hit with primary hit above threshold");

    // Look before and after window
    int lo_edge = int(primaryHit->time_vector().front()/2.);
    int hi_edge = int(primaryHit->time_vector().back()/2.);

    std::vector<float> wwave_lo(lo_edge);
    std::vector<float> wwave_hi(wave.adc_counts().size() - hi_edge - 1);

    int ipeak_lo = -1.;
    int ipeak_hi = -1.;

    // Look before
    if (m_findSecondaryBefore) {
      for (int i=0; i<lo_edge; i++) {
	wwave_lo[i] = baseline.mean - wave.mv_per_bit() * wave.adc_counts()[i];
      }

      ipeak_lo = findPeak(baseline, m_secondaryPeakThreshold, wwave_lo);

      if (ipeak_lo < 0) {
	ATH_MSG_DEBUG("No hit found before " << lo_edge);
      } else {
	ATH_MSG_DEBUG("Hit found at " << ipeak_lo << " before " << lo_edge);
      }
    }

    // Look after
    if (m_findSecondaryAfter) {
      for (unsigned int i=(hi_edge+1); i<wave.adc_counts().size(); i++) {
	wwave_hi[(i-(hi_edge+1))] = baseline.mean - wave.mv_per_bit() * wave.adc_counts()[i];
      }

      ipeak_hi = findPeak(baseline, m_secondaryPeakThreshold, wwave_hi);

      // Is this too close to the primary hit?
      if (ipeak_hi < 5) {
	ATH_MSG_DEBUG("Found hit after at " << (ipeak_hi + hi_edge + 1)<< " but too close to edge");
	ipeak_hi = -1;
      }

      if (ipeak_hi < 0) {
	ATH_MSG_DEBUG("No hit found after " << hi_edge);
      } else {
	ATH_MSG_DEBUG("Hit found at " << ipeak_hi << " after " << hi_edge);
      }
    }

    // Nothing found
    if (ipeak_lo < 0 && ipeak_hi < 0)
      return StatusCode::SUCCESS;

    // Both?
    if (ipeak_lo >= 0 && ipeak_hi >= 0) {

      // Pick the largest signal
      if (wwave_lo[ipeak_lo] >= wwave_hi[ipeak_hi]) {
	ipeak = ipeak_lo;
	ATH_MSG_DEBUG("Picked before as " << wwave_lo[ipeak_lo]
		      << " > " << wwave_hi[ipeak_hi]);
      } else {
	ipeak = ipeak_hi + hi_edge + 1;
	ATH_MSG_DEBUG("Picked after as " << wwave_lo[ipeak_lo]
		      << " < " << wwave_hi[ipeak_hi]);
      }

    
    } else if (ipeak_lo > 0) {
      ipeak = ipeak_lo;
      ATH_MSG_DEBUG("Peak before with " << wwave_lo[ipeak_lo]);
    } else {
      ATH_MSG_DEBUG("Peak after with " << wwave_hi[ipeak_hi]);
      ipeak = ipeak_hi+hi_edge+1;
    }

  } else {

    ATH_MSG_DEBUG("Looking for secondary hit without primary hit above threshold");
    std::vector<float> wwave(wave.adc_counts().size());
    for (unsigned int i=0; i<wave.adc_counts().size(); i++) {
      wwave[i] = baseline.mean - wave.mv_per_bit() * wave.adc_counts()[i];
    }

    ipeak = findPeak(baseline, m_secondaryPeakThreshold, wwave);

    // Nothing found
    if (ipeak < 0) 
      return StatusCode::SUCCESS;

    ATH_MSG_DEBUG("Found secondary peak with no primary " << wwave[ipeak]);
  }

  // We seem to have a secondary hit
  xAOD::WaveformHit* newhit = new xAOD::WaveformHit();
  hitContainer->push_back(newhit);

  // Fill values
  newhit->set_channel(wave.channel());
  newhit->set_identifier(wave.identify());
  newhit->set_status_bit(xAOD::WaveformStatus::SECONDARY);
  newhit->set_baseline_mean(baseline.mean);
  newhit->set_baseline_rms(baseline.rms);

  // Set range for windowed data in digitizer samples
  int lo_edge = ipeak + m_windowStart;
  int hi_edge = ipeak + m_windowStart + m_windowWidth;

  // Fill raw hit values
  fillRawHitValues(wave, lo_edge, hi_edge, newhit);

  // Must be over threshold, so reconstruct here
  reconstructHit(newhit);

  return StatusCode::SUCCESS;
}

StatusCode
WaveformReconstructionTool::setLocalTime(const xAOD::WaveformClock* clock,
					 xAOD::WaveformHitContainer* container) const {

  ATH_MSG_DEBUG(" setLocalTime called ");

  // Check the container
  if (!container) {
    ATH_MSG_ERROR("WaveformHitCollection passed to setLocalTime() is null!");
    return StatusCode::FAILURE;
  }

  bool clock_valid;

  //
  // Find time from clock
  if (!clock || (clock->frequency() <= 0.)) {
    clock_valid = false;
  } else {
    clock_valid = true;
  }

  float trigger_time = m_timingTool->nominalTriggerTime();
  float offset;

  // Should actually find the time of the trigger here 
  // and set bcid time offset from that
  // Loop through hits and set local time
  for( const auto& hit : *container) {

    //
    // Find time from clock
    if (clock_valid) {
      hit->set_bcid_time(clock->time_from_clock(hit->localtime()));
    } else {
      hit->set_status_bit(xAOD::WaveformStatus::CLOCK_INVALID);
      hit->set_bcid_time(-1.);
    }

    // Also set time with respect to nominal trigger
    offset = m_timingTool->triggerTimeOffset(hit->channel());
    hit->set_trigger_time(hit->localtime() - (trigger_time + offset));
  }

  return StatusCode::SUCCESS;
}

// Find the baseline
WaveformBaselineData& 
WaveformReconstructionTool::findBaseline(const RawWaveform& raw_wave, 
					 xAOD::WaveformHit* hit) const {

  //
  // Find baseline
  static WaveformBaselineData baseline;

  if (m_useSimpleBaseline.value())
    baseline = findSimpleBaseline(raw_wave);
  else
    baseline = findAdvancedBaseline(raw_wave);

  if (!(baseline.valid)) {
    ATH_MSG_WARNING("Failed to reconstruct baseline!");

    hit->set_baseline_mean(0.);
    hit->set_baseline_rms(-1.);
    hit->set_status_bit(xAOD::WaveformStatus::BASELINE_FAILED);

  } else {
    // Save baseline to hit collection object
    hit->set_baseline_mean(raw_wave.mv_per_bit()*baseline.mean);
    hit->set_baseline_rms(raw_wave.mv_per_bit()*baseline.rms);
    ATH_MSG_DEBUG("Baseline found with mean = " << hit->baseline_mean()
		  << " mV and rms = " << hit->baseline_rms()
		  << " mV");
  }

  return baseline;
}

// Fill the raw hit parameters
void
WaveformReconstructionTool::fillRawHitValues(const RawWaveform& wave,
					   int lo_edge, int hi_edge, 
					   xAOD::WaveformHit* hit) const {

  // First, make sure we don't overflow the waveform range
  if (lo_edge < 0) lo_edge = 0;
  if (hi_edge >= int(wave.size())) hi_edge = wave.size() - 1;
  if (hi_edge <= lo_edge) {
    ATH_MSG_WARNING("Channel " << wave.channel() << " waveform with lo: " << lo_edge << " hi: " << hi_edge << "!");
    return;
  }
  
  ATH_MSG_DEBUG("Fill channel " << wave.channel() 
		<< " waveform from sample " << lo_edge << " to " << hi_edge);

  // Fill hit window with data from wave
  std::vector<float> wtime(hi_edge-lo_edge+1);
  std::vector<float> wwave(hi_edge-lo_edge+1);

  for (int i=lo_edge; i<=hi_edge; i++) {
    unsigned int j = i-lo_edge;
    wtime[j] = 2.*i; // 2ns per sample at 500 MHz
    wwave[j] = hit->baseline_mean() - wave.mv_per_bit() * wave.adc_counts()[i];
  }

  hit->set_time_vector(wtime);
  hit->set_wave_vector(wwave);

  // Set raw values
  WaveformFitResult raw = findRawHitValues(wtime, wwave);
  hit->set_peak(raw.peak);
  hit->set_mean(raw.mean);
  hit->set_width(raw.sigma);
  hit->set_integral(raw.integral);
  hit->set_localtime(raw.mean);
  hit->set_raw_peak(raw.peak);
  hit->set_raw_integral(raw.integral);

}

// Reconstruct a hit from the RawWaveform in the range specified
// Range is in units digitizer samples (not ns)
void
WaveformReconstructionTool::reconstructHit(xAOD::WaveformHit* hit) const {

  // Time and waveform vectors
  // Don't use reference as we may modify this below
  std::vector<float> wtime = hit->time_vector();
  std::vector<float> wwave = hit->wave_vector();

  ATH_MSG_DEBUG("Reconstruct channel " << hit->channel() 
		<< " waveform from " << wtime.front() 
		<< " to " << wtime.back());

  // Fill values needed for fit (peak, mean, and sigma)
  WaveformFitResult raw;
  raw.peak = hit->peak();
  raw.mean = hit->mean();
  raw.sigma = hit->width();

  //
  // Perform Gaussian fit to waveform
  WaveformFitResult gfit = fitGaussian(raw, wtime, wwave);
  if (! gfit.valid) {
    // Lets try again with a more restricted width
    ATH_MSG_WARNING( " Gaussian waveform fit failed with width " << raw.sigma << " try reducing width to 1 " );
    raw.sigma = 1.;
    gfit = fitGaussian(raw, wtime, wwave);
    if (!gfit.valid) {
      hit->set_status_bit(xAOD::WaveformStatus::GFIT_FAILED);
    }
  } 

  // Fit results (or raw if it failed)
  hit->set_peak(gfit.peak);
  hit->set_mean(gfit.mean);
  hit->set_width(gfit.sigma);
  hit->set_integral(gfit.integral);
  hit->set_localtime(gfit.time);
  
  //
  // Check for overflow
  if (m_removeOverflow && findOverflow(hit->baseline_mean(), wtime, wwave)) {
    ATH_MSG_INFO("Found waveform overflow");
    hit->set_status_bit(xAOD::WaveformStatus::WAVE_OVERFLOW);
  }
  
  //
  // Perform CB fit
  WaveformFitResult cbfit = fitCBall(gfit, wtime, wwave);
  if (! cbfit.valid) {
    ATH_MSG_WARNING("CrystalBall fit failed for channel " << hit->channel() << "!");
    // Still have gaussian parameters as an estimate
    hit->set_status_bit(xAOD::WaveformStatus::CBFIT_FAILED);
  } else {
    hit->set_peak(cbfit.peak);
    hit->set_mean(cbfit.mean);
    hit->set_width(cbfit.sigma);
    hit->set_integral(cbfit.integral);
    hit->set_localtime(cbfit.time);
    
    hit->set_alpha(cbfit.alpha);
    hit->set_nval(cbfit.nval);
  }

  ATH_MSG_DEBUG("Done reconstructing channel " << hit->channel() 
		<< " waveform from " << wtime.front() << " to " << wtime.back());

}

// Returns location of peak in array wave
// Return value is -1 if peak is below threshold
int
WaveformReconstructionTool::findPeak(WaveformBaselineData& baseline, 
				     float threshold,
				     std::vector<float>& wave) const 
{

  ATH_MSG_DEBUG("findPeak called");

  // Find max value location in array
  unsigned int imax = std::max_element(wave.begin(), wave.end()) - wave.begin();
  float maxval = wave[imax];
  ATH_MSG_DEBUG( "Found peak value " << maxval << " at position " << imax );

  // Check if this is over threshold (in sigma)
  if (maxval < threshold*baseline.rms) {
    ATH_MSG_DEBUG("Failed threshold");
    return -1;
  }

  return imax;
}

bool
WaveformReconstructionTool::findOverflow(float baseline,
	      std::vector<float>& time, std::vector<float>& wave) const {

  auto peakloc = std::max_element(wave.begin(), wave.end());

  // If peak value is less than baseline, we have no overflow
  if (*peakloc < baseline) return false;

  ATH_MSG_DEBUG("Removing overflows from waveform with length " << wave.size());

  // We have an overflow, remove all elements that are overflowing
  unsigned int i = peakloc - wave.begin();
  for (; i<wave.size(); i++) {
    if (wave[i] < baseline) continue;

    ATH_MSG_DEBUG("Removing position "<< i<< " with value " << wave[i] << " > " << baseline);
    // This is an overflow, remove elements
    time.erase(time.begin() + i);
    wave.erase(wave.begin() + i);
    i--;  // Decrement so that loop itertaion points to next element
  }

  ATH_MSG_DEBUG("Removed overflows, length now " << wave.size());
  return true;
}

WaveformBaselineData&
WaveformReconstructionTool::findSimpleBaseline(const RawWaveform& raw_wave) const {

  ATH_MSG_DEBUG( "findSimpleBaseline called" );
  //ATH_MSG_DEBUG( raw_wave );

  // This must be static so we can return a reference
  static WaveformBaselineData baseline;  
  baseline.clear();

  // Calling algorithm checks for proper data
  // Here we just check algorithm-specific issues
  if (int(raw_wave.n_samples()) < m_samplesForBaselineAverage.value()) {
    ATH_MSG_WARNING( "Found waveform with " << raw_wave.n_samples() << " samples, not enough to find baseline!" );
    return baseline;
  }

  const std::vector<unsigned int>& counts = raw_wave.adc_counts();

  double sumx = 0.;
  double sumx2 = 0.;

  // Look at first n values in the waveform
  unsigned int nvalues = m_samplesForBaselineAverage.value();

  for (unsigned int i=0; i< nvalues; i++) {
    sumx  += counts[i];
    sumx2 += (counts[i] * counts[i]);
  }
  double mean = sumx / nvalues;
  double mean2 = sumx2 / nvalues;

  double rms = std::sqrt(mean2 - mean*mean);

  baseline.valid = true;
  baseline.channel = raw_wave.channel();
  baseline.mean = mean;
  baseline.rms = rms;
  return baseline;
}

WaveformBaselineData&
WaveformReconstructionTool::findAdvancedBaseline(const RawWaveform& raw_wave) const {

  ATH_MSG_DEBUG( "findAdvancedBaseline called" );

  // This must be static so we can return a reference
  static WaveformBaselineData baseline;  
  baseline.clear();

  // First histogram to find most likely value
  // Turn these into configurables once this works
  int nbins = m_baselineRangeBins;
  double xlo = 0.;
  double xhi = m_baselineRange;

  TH1F h1("", "", nbins, xlo, xhi);

  // Fill this histogram with the waveform
  // Only use a subset, based on where we think there won't be signal
  const std::vector<unsigned int>& counts = raw_wave.adc_counts();
  for (int i=m_baselineSampleLo; i<=m_baselineSampleHi; i++) {
    //ATH_MSG_INFO( "Entry " << i << " Value " << counts[i] );
    h1.Fill(counts[i]);
  }

  /*
  for (auto value : raw_wave.adc_counts()) {
    //ATH_MSG_DEBUG( "Found value " << value );
    h1.Fill(value);
  }
  */

  // Find max bin
  int maxbin = h1.GetMaximumBin();
  double maxbinval = h1.GetXaxis()->GetBinCenter(maxbin);
  ATH_MSG_DEBUG( "Found max bin at " << maxbinval << " counts");

  // Fill second histogram with integer resolution around the peak
  nbins = m_baselineFitRange;
  xlo = int(maxbinval - nbins/2)-0.5;
  xhi = xlo + nbins;
  ATH_MSG_DEBUG( "Filling 2nd histogram from " << xlo << " to " << xhi);

  TH1F h2("", "", nbins, xlo, xhi);
  for (int i=m_baselineSampleLo; i<=m_baselineSampleHi; i++) {
    h2.Fill(counts[i]);
  }

  // Start with full histogram range
  double mean = h2.GetMean();
  double rms = h2.GetRMS();
  double peak = h2.GetBinContent(h2.GetMaximumBin());

  ATH_MSG_DEBUG( "Initial Mean: " << mean << " RMS: " << rms << " Peak: " << peak );

  // Restrict range to +/- 2 sigma of mean
  double window = m_baselineFitWindow;  // Window range in sigma
  h2.GetXaxis()->SetRangeUser(mean-window*rms, mean+window*rms);
  mean = h2.GetMean();
  rms = h2.GetRMS();
  peak = h2.GetBinContent(h2.GetMaximumBin());

  ATH_MSG_DEBUG( "2 Sigma Mean: " << mean << " RMS: " << rms << " Peak: " << peak);

  // Finally, fit a Gaussian to the 2 sigma range
  double fitlo = mean-window*rms;
  double fithi = mean+window*rms;

  // Define fit function and preset range
  TF1 fitfunc("BaseFit", "gaus", fitlo, fithi);
  fitfunc.SetParameters(peak, mean, rms);

  // Fit Options:
  // L - Likelihood fit
  // Q - Quiet mode (V for verbose)
  // N - Don't draw or store graphics
  // S - Return fit results
  // E - Better error estimation
  // M - Use TMinuit IMPROVE to find a better minimum
  // R - Use function range  
  TFitResultPtr fit = h2.Fit(&fitfunc, "LQNSR", "");

  int fitStatus = fit;
  double chi2 = fit->Chi2();
  unsigned int ndf = fit->Ndf();
  if (ndf == 0) ndf = 1;

  if (!fit->IsValid()) {
    ATH_MSG_WARNING( " Baseline fit failed! ");
  } else {
    // Improve estimation with fit results
    peak = fit->Parameter(0);
    mean = fit->Parameter(1);
    rms  = fit->Parameter(2);
    ATH_MSG_DEBUG( "G Fit   Mean: " << mean << " RMS: " << rms << " Peak: " << peak << " Chi2/N: " << chi2/ndf);  
  }

  baseline.valid = true;
  baseline.channel = raw_wave.channel();
  baseline.mean = mean;
  baseline.rms = rms;
  baseline.fit_status = fitStatus; 
  baseline.peak = peak;
  baseline.chi2n = chi2/ndf;
  return baseline;
}

WaveformFitResult&
WaveformReconstructionTool::findRawHitValues(const std::vector<float> time, const std::vector<float> wave) const {

  ATH_MSG_DEBUG("findRawHitValues called");

  // This must be static so we can return a reference
  static WaveformFitResult rfit;
  rfit.clear();

  // First, estimate starting values from input waveform
  // Will use this to pre-set fit values, but also as a backup in case fit fails
  double tot = 0.;
  double sum = 0.;
  double sum2 = 0.;
  for (unsigned int i=0; i<time.size(); i++) {
    //ATH_MSG_DEBUG("findRawHitValues Time: " << time[i] << " Wave: " << wave[i]);
    tot += wave[i];
    sum += time[i] * wave[i];
    sum2 += time[i] * time[i] * wave[i];
  }

  // Pointer to peak
  auto peakloc = std::max_element(wave.begin(), wave.end());

  // Initial parameters from waveform
  rfit.mean = sum/tot;
  rfit.sigma = std::sqrt(sum2/tot - rfit.mean*rfit.mean);
  rfit.peak = *peakloc;
  rfit.integral = 2*tot; // Factor of 2 because at 500 MHz, dt = 2 ns
  rfit.time = rfit.mean;

  ATH_MSG_DEBUG( 
		"Initial Mean: " << rfit.mean 
		<< " RMS: " << rfit.sigma 
		<< " Peak: " << rfit.peak 
		<< " Integral: " << rfit.integral
		 );  

  // Fix bad values
  if (isnan(rfit.sigma)) {
    ATH_MSG_DEBUG("Found sigma: " << rfit.sigma << " Replace with 2.");
    rfit.sigma = 2.;
  }

  if (rfit.mean < time.front() || rfit.mean > time.back()) {
    rfit.mean = (3*time.front() + time.back())/4.; // Set time to 1/4 of way through window
    ATH_MSG_DEBUG("Found mean: " << rfit.time << " out of range " << time.front() << "-" << time.back() << " Replace with " << rfit.mean);
    rfit.time = rfit.mean;
  }

  return rfit;
}

WaveformFitResult&
WaveformReconstructionTool::fitGaussian(const WaveformFitResult& raw, const std::vector<float> time, const std::vector<float> wave) const {

  ATH_MSG_DEBUG("fitGaussian called");

  // This must be static so we can return a reference
  static WaveformFitResult gfit;
  gfit.clear();

  // Start with raw values by default
  gfit = raw;

  // Start by creating a TGraph and fitting this with a Gaussian
  // Must pass arrays to TGraph, use pointer to first vector element
  TGraph tg(time.size(), &time[0], &wave[0]);

  // Define fit function and preset range
  TF1 gfunc("gfunc", "gaus");
  gfunc.SetParameters(raw.peak, raw.mean, raw.sigma);
  gfunc.SetParError(0, raw.peak/10.);
  gfunc.SetParError(1, raw.sigma);
  gfunc.SetParError(2, raw.sigma / 5.);
  gfunc.SetParLimits(2, 0., 20.);  // Constrain width

  TFitResultPtr gfitptr = tg.Fit(&gfunc, "QNS", "");

  gfit.fit_status = gfitptr;
  gfit.valid = (gfit.fit_status == 0);

  if (!gfitptr->IsValid()) {
    ATH_MSG_WARNING( " Gaussian waveform fit failed! ");
  } else {
    // Improve estimation with fit results
    gfit.peak = gfitptr->Parameter(0);
    gfit.mean = gfitptr->Parameter(1);
    gfit.sigma = gfitptr->Parameter(2);
    gfit.integral = gfunc.Integral(time.front(), time.back());

    // Find time here
    gfit.time = gfunc.GetX( gfit.peak * m_timingPeakFraction, time.front(), gfit.mean );

    if (isnan(gfit.time)) {
      ATH_MSG_WARNING(" Gaussian fit returned time as NaN, assume fit failed");
      gfit = raw;
      gfit.fit_status = false;
    }

    ATH_MSG_DEBUG("G Fit   Mean: " << gfit.mean << " RMS: " << gfit.sigma 
		  << " Peak: " << gfit.peak << " Int: " << gfit.integral << " Time: " << gfit.time);
  }

  return gfit;
}

WaveformFitResult&
WaveformReconstructionTool::fitCBall(const WaveformFitResult& gfit, 
     const std::vector<float> time, const std::vector<float> wave) const {

  ATH_MSG_DEBUG("fitCBall called");

  // This must be static so we can return a reference
  static WaveformFitResult cbfit;
  cbfit.clear();

  // Must pass arrays to TGraph, use pointer to first vector element
  TGraph tg(time.size(), &time[0], &wave[0]);

  // Values to preset CB fit
  cbfit.peak = abs(gfit.peak);
  cbfit.mean = gfit.mean;
  cbfit.sigma = abs(gfit.sigma);
  if (cbfit.sigma > 20.) cbfit.sigma = 2.;
  cbfit.alpha = -0.5;  // Negative to get tail on high side
  cbfit.nval = 2.;

  // Define fit function and preset values
  TF1 cbfunc("cbfunc", "crystalball");
  cbfunc.SetParameter(0, cbfit.peak); // Peak height
  cbfunc.SetParError(0,  cbfit.peak/10.);
  cbfunc.SetParameter(1, cbfit.mean); // Mean
  cbfunc.SetParError(1, cbfit.sigma);
  cbfunc.SetParameter(2, cbfit.sigma);  // Width
  cbfunc.SetParError(2, cbfit.sigma/5.);
  cbfunc.SetParLimits(2, 0., 20.);
  cbfunc.SetParameter(3, cbfit.alpha); // Tail parameter (negative for high-side tail)
  cbfunc.SetParError(3, -cbfit.alpha/5.);
  cbfunc.SetParLimits(3, -10., 0.);
  // Try fixing the tail first
  cbfunc.FixParameter(4, cbfit.nval);

  TFitResultPtr cbfitptr = tg.Fit(&cbfunc, "QNS", "");

  if (!cbfitptr->IsValid()) {
    ATH_MSG_DEBUG( " First Crystal Ball waveform fit failed! ");
  }

  // Now try releasing the tail parameter
  cbfunc.ReleaseParameter(4);
  cbfunc.SetParameter(4, cbfit.nval);  // Tail power
  cbfunc.SetParError(4, cbfit.nval/5.);
  cbfunc.SetParLimits(4, 0., 1.E3);

  cbfitptr = tg.Fit(&cbfunc, "QNS", "");

  cbfit.fit_status = cbfitptr;
  cbfit.valid = (cbfit.fit_status == 0);

  if (!cbfitptr->IsValid()) {
    ATH_MSG_DEBUG( " Full Crystal Ball waveform fit failed! ");
  } else {
    // Improve estimation with fit results
    cbfit.peak = cbfitptr->Parameter(0);
    cbfit.mean = cbfitptr->Parameter(1);
    cbfit.sigma = cbfitptr->Parameter(2);
    cbfit.alpha = cbfitptr->Parameter(3);
    cbfit.nval = cbfitptr->Parameter(4);

    double chi2 = cbfitptr->Chi2();
    unsigned int ndf = cbfitptr->Ndf();
    if (ndf == 0) ndf = 1;
    cbfit.chi2ndf = chi2/ndf;

    cbfit.integral = cbfunc.Integral(time.front(), time.back());

    // Find time here
    cbfit.time = cbfunc.GetX( cbfit.peak * m_timingPeakFraction, time.front(), cbfit.mean );

    if (isnan(cbfit.time)) {
      ATH_MSG_WARNING(" Crystal Ball fit returned time as NaN, assume fit failes");
      cbfit.time = gfit.time;
      cbfit.fit_status = false;
    }

    ATH_MSG_DEBUG("CB Fit  Mean: " << cbfit.mean << " RMS: " << cbfit.sigma 
		  << " Peak: " << cbfit.peak << " Int: " << cbfit.integral
		  << " Time: " << cbfit.time 
		  << " N: " << cbfit.nval << " Alpha: " << cbfit.alpha 
		  << " Chi2/Ndf: " << cbfit.chi2ndf );

  }

  return cbfit;
}
