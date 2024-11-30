/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/**
 * @file WaveformDigitisationTool.cxx
 * Implementation file for the WaveformDigitisationTool class
 * @ author C. Gwilliam, 2021
 **/

#include "WaveformDigitisationTool.h"

#include "ScintReadoutGeometry/ScintDetectorElement.h"
#include "CaloReadoutGeometry/CaloDetectorElement.h"

// Constructor
WaveformDigitisationTool::WaveformDigitisationTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

// Initialization
StatusCode
WaveformDigitisationTool::initialize() {
  ATH_MSG_INFO( name() << "::initalize()" );

  // Setup ID helpers
  ATH_CHECK(detStore()->retrieve(m_vetoID, "VetoID"));
  ATH_CHECK(detStore()->retrieve(m_vetoNuID, "VetoNuID"));
  ATH_CHECK(detStore()->retrieve(m_triggerID, "TriggerID"));
  ATH_CHECK(detStore()->retrieve(m_preshowerID, "PreshowerID"));
  ATH_CHECK(detStore()->retrieve(m_ecalID, "EcalID"));

  // Setup detector managers
  // These don't necessarily all exist depending on the geometry
  // so don't use ATH_CHECK
  if (StatusCode::SUCCESS != detStore()->retrieve(m_vetoDetMan, "Veto"))
    ATH_MSG_WARNING("Couldn't load Veto detector manager!");
  if (StatusCode::SUCCESS != detStore()->retrieve(m_vetoNuDetMan, "VetoNu"))
    ATH_MSG_WARNING("Couldn't load VetoNu detector manager!");
  if (StatusCode::SUCCESS != detStore()->retrieve(m_triggerDetMan, "Trigger"))
    ATH_MSG_WARNING("Couldn't load Trigger detector manager!");
  if (StatusCode::SUCCESS != detStore()->retrieve(m_preshowerDetMan, "Preshower"))
    ATH_MSG_WARNING("Couldn't load Preshower detector manager!");
  if (StatusCode::SUCCESS != detStore()->retrieve(m_caloDetMan, "Ecal"))
    ATH_MSG_WARNING("Couldn't load Calo detector manager!");

  // Show our parameters (for debugging for now)
  ATH_MSG_INFO(m_digitizerPeriod);
  ATH_MSG_INFO(m_digitizerSamples);
  ATH_MSG_INFO(m_overSamples);

  m_random = new TRandom3(0);

  return StatusCode::SUCCESS;
}

std::vector<float>
WaveformDigitisationTool::evaluate_timekernel(TF1* kernel) const {
  
  std::vector<float> timekernel;
  timekernel.reserve(m_digitizerSamples);
  
  for (unsigned int i=0; int(i) < m_digitizerSamples; i++) {
    timekernel.push_back(kernel->Eval(m_digitizerPeriod*i));  
  }
  
  return timekernel;
}

float
WaveformDigitisationTool::generate_baseline(float mean, float rms) const {
  return m_random->Gaus(mean, rms);
}


/// Fill EvsT histograms
/// Time should be in ns
void 
WaveformDigitisationTool::fill_evst_hist(EvstHistMap& map, const Identifier& id, float time, float energy) const {

  if (map.find(id) == map.end()) {
    ATH_MSG_DEBUG("Creating EvsT hist for channel" << id);
    // Start histogram before zero to pick up any early arriving hits
    map[id] = new TH1D("", "", m_overSamples * (m_digitizerSamples+m_preSamples), -m_digitizerPeriod*m_preSamples, m_digitizerPeriod*m_digitizerSamples);
  }

  if (time < (-m_digitizerPeriod*m_preSamples))
    ATH_MSG_WARNING("ID " << id << " hit found with time " << time << " E " << energy << "!");

  map[id]->Fill(time, energy);
}

/// Delete histograms and clear map
void 
WaveformDigitisationTool::clear_evst_hist(EvstHistMap& map) const {

  // Delete histogram objects
  for (auto& it : map)
    delete it.second;

  // Also remove map elements (now null pointers)
  map.clear();
}

//template <class HitCollection>
//std::map<Identifier, std::vector<uint16_t>>& 
//WaveformDigitisationTool::generateWaveforms(
//		const EventContext& ctx,
//		const ToolHandle<IWaveformDigiConditionsTool>& digiCondTool,
//		const HitCollection* hitCollection) const {

//std::map<Identifier, std::vector<uint16_t>>&
//WaveformDigitisationTool::test_context(const EventContext& ctx) const {
//  static std::map<Identifier, std::vector<uint16_t>> waveforms;
//  return waveforms;
//}

TF1*
WaveformDigitisationTool::create_kernel(const EventContext& ctx, 
					const ToolHandle<IWaveformDigiConditionsTool>& digiCondTool) const {

  TF1* kernel = new TF1("PDF", "[4] * ROOT::Math::crystalball_pdf(x, [0],[1],[2],[3])", 0, 1200);

  kernel->SetParameter(0, digiCondTool->cb_alpha(ctx));
  kernel->SetParameter(1, digiCondTool->cb_n(ctx));
  kernel->SetParameter(2, digiCondTool->cb_sigma(ctx));
  kernel->SetParameter(3, digiCondTool->cb_mean(ctx));
  kernel->SetParameter(4, digiCondTool->cb_norm(ctx));

  return kernel;
}

std::map<Identifier, std::vector<uint16_t>>& 
WaveformDigitisationTool::generate_scint_waveforms(
		const EventContext& ctx,
		const ToolHandle<IWaveformDigiConditionsTool>& digiCondTool,
		const ScintHitCollection* hitCollection) const {

  // Create digitization kernel
  TF1* kernel = create_kernel(ctx, digiCondTool);

  // Helpers, to be filled below as needed

  // Create structure to store waveform for each channel
  // We return a reference to this, so it must be static
  static std::map<Identifier, std::vector<uint16_t>> waveforms;

  // Make sure this is empty
  waveforms.clear();

  auto first = hitCollection->begin();

  if (first->isVeto()) {
    waveforms = create_waveform_map(m_vetoID);
  } else if (first->isVetoNu()) {
    waveforms = create_waveform_map(m_vetoNuID); 
  } else if (first->isTrigger()) {
    waveforms = create_waveform_map(m_triggerID);
  } else if (first->isPreshower()) {
    waveforms = create_waveform_map(m_preshowerID);
  }

  // Make energy sums
  Identifier id;
  std::map<Identifier, float> esum;

  for (const auto& hit : *hitCollection) {

    if (hit.isTrigger()) {
      Identifier plate_id = m_triggerID->plate_id(hit.getIdentifier());      

      id = m_triggerID->pmt_id(plate_id, 0); // ID for PMT 0
      esum[id] += hit.energyLoss();

      id = m_triggerID->pmt_id(plate_id, 1); // ID for PMT 1
      esum[id] += hit.energyLoss();
    } else {
      id = hit.getIdentifier();
      esum[id] += hit.energyLoss();
    }
  } // Done with loop over hits

  // Now make waveforms
  int value;
  float signal, baseline;

  for (const auto& w : waveforms) {

    auto id = w.first;

    // Now fill each entry with the sum of the kernel
    // and some random background
    for (unsigned int i=0; int(i) < m_digitizerSamples; i++) {
      signal = kernel->Eval(m_digitizerPeriod * i) * esum[id];
      baseline = generate_baseline(digiCondTool->base_mean(ctx),
				   digiCondTool->base_rms(ctx));

      // Waveform is some random baseline plus a negative signal
      value = std::round(baseline - signal);

      // Don't let value go below zero
      if (value < 0) 
	waveforms[id].push_back(0);
      else
	waveforms[id].push_back(value);

    } // End of loop over bins

  } // End of loop over channels

  // Clean up after ourselves
  delete kernel;

  return waveforms;
}

std::map<Identifier, std::vector<uint16_t>>& 
WaveformDigitisationTool::generate_scint_timing_waveforms(
		const EventContext& ctx,
		const ToolHandle<IWaveformDigiConditionsTool>& digiCondTool,
		const ScintHitCollection* hitCollection) const {

  // Create structure to store waveform for each channel
  // We return a reference to this, so it must be static
  static std::map<Identifier, std::vector<uint16_t>> waveforms;

  // Make sure this is empty
  waveforms.clear();

  // Create TOF values
  std::map<Identifier, float> time_of_flight;

  auto first = hitCollection->begin();

  if (first->isVeto()) {
    waveforms = create_waveform_map(m_vetoID);
    time_of_flight = create_tof_map(m_vetoID, m_vetoDetMan);
  } else if (first->isVetoNu()) {
    waveforms = create_waveform_map(m_vetoNuID); 
    time_of_flight = create_tof_map(m_vetoNuID, m_vetoNuDetMan);
  } else if (first->isTrigger()) {
    waveforms = create_waveform_map(m_triggerID);
    time_of_flight = create_tof_map(m_triggerID, m_triggerDetMan);
  } else if (first->isPreshower()) {
    waveforms = create_waveform_map(m_preshowerID);
    time_of_flight = create_tof_map(m_preshowerID, m_preshowerDetMan);
  }

  // Make energy sums
  Identifier id;

  EvstHistMap evst_hist;

  // Make sure we clear this
  clear_evst_hist(evst_hist);

  for (const auto& hit : *hitCollection) {

    float dtime; // Time offset due to light propogation time in scintillator
    float tof;   // Time offset due to detector z position

    // Trigger plane, horizontal readout
    if (hit.isTrigger()) {
      Identifier plate_id = m_triggerID->plate_id(hit.getIdentifier());      

      // Time delay based on horizontal distance
      // Checked sign with Deion
      dtime = digiCondTool->trigger_time_slope(ctx) * (hit.localStartPosition().x()+hit.localEndPosition().x()) / 2.;
      id = m_triggerID->pmt_id(plate_id, 0); // ID for PMT 0
      tof = time_of_flight[id];

      // Fill our histogram for this channel
      fill_evst_hist(evst_hist, id, hit.meanTime()-dtime-tof, hit.energyLoss());

      id = m_triggerID->pmt_id(plate_id, 1); // ID for PMT 1
      tof = time_of_flight[id];
      fill_evst_hist(evst_hist, id, hit.meanTime()+dtime-tof, hit.energyLoss());

      // Could also change amplitude based on distance to PMT, but not yet

    } else {

      // All others have only 1 PMT, use vertical position for dtime

      id = hit.getIdentifier();
      tof = time_of_flight[id];

      // +y means shorter times, only apply to preshower
      if (hit.isPreshower())
	dtime = -digiCondTool->preshower_time_slope(ctx) * (hit.localStartPosition().y() + hit.localEndPosition().y()) / 2.;
      else
	dtime = 0.;
	  
      fill_evst_hist(evst_hist, id, hit.meanTime()+dtime-tof, hit.energyLoss());  
    }

    // Print out hit times
    ATH_MSG_DEBUG("Scint Hit: " << id << " E: " << hit.energyLoss() 
		  << " t: " << hit.meanTime()
		  << " x: " << hit.localStartPosition().x()
		  << " y: " << hit.localStartPosition().y()
		  << " dt: " << dtime
		  << " tof: " << tof
		  );

  } // Done with loop over hits

  // Debugging printout 
  for (auto it : evst_hist) {
    ATH_MSG_DEBUG("Nonzero bins Evst hist id = " << it.first);
    for (unsigned int ibin=0; int(ibin) <= it.second->GetNbinsX(); ibin++) {
      if (it.second->GetBinContent(ibin) == 0) continue;
      ATH_MSG_DEBUG(" bin: " << ibin << " t:" << it.second->GetBinCenter(ibin) << " val: " << it.second->GetBinContent(ibin));
    }
  }

  // Now make waveforms
  convolve_waveforms(ctx, digiCondTool, evst_hist, waveforms);

  // Check what we have done
  for (const auto& w : waveforms) {
    ATH_MSG_DEBUG("Waveform for ID " << w.first);
    for(unsigned int i=400; i<450; i++) 
      ATH_MSG_DEBUG(digitizer_period()*i << "ns -> " << w.second[i]);
  }

  // Cleanup
  clear_evst_hist(evst_hist);

  return waveforms;
}

std::map<Identifier, std::vector<uint16_t>>& 
WaveformDigitisationTool::generate_calo_waveforms(
		const EventContext& ctx,
		const ToolHandle<IWaveformDigiConditionsTool>& digiCondTool,
		const CaloHitCollection* hitCollection) const {

  // Create digitization kernel
  TF1* kernel = create_kernel(ctx, digiCondTool);

  // Helpers, to be filled below as needed

  // Create structure to store waveform for each channel
  // We return a reference to this, so it must be static
  static std::map<Identifier, std::vector<uint16_t>> waveforms;

  // Make sure this is empty
  waveforms.clear();

  waveforms = create_waveform_map(m_ecalID);

  // Make energy sums
  Identifier id;
  std::map<Identifier, float> esum;

  for (const auto& hit : *hitCollection) {
    id = hit.getIdentifier();
    esum[id] += hit.energyLoss();
  } // Done with loop over hits

  // Now make waveforms
  int value;
  float signal, baseline;

  for (const auto& w : waveforms) {

    auto id = w.first;

    // Now fill each entry with the sum of the kernel
    // and some random background
    for (unsigned int i=0; int(i) < m_digitizerSamples; i++) {
      signal = kernel->Eval(m_digitizerPeriod * i) * esum[id];
      baseline = generate_baseline(digiCondTool->base_mean(ctx),
				   digiCondTool->base_rms(ctx));

      // Waveform is some random baseline plus a negative signal
      value = std::round(baseline - signal);

      // Don't let value go below zero
      if (value < 0) 
	waveforms[id].push_back(0);
      else
	waveforms[id].push_back(value);

    } // End of loop over bins

  } // End of loop over channels

  // Clean up after ourselves
  delete kernel;

  return waveforms;
}

std::map<Identifier, std::vector<uint16_t>>& 
WaveformDigitisationTool::generate_calo_timing_waveforms(
		const EventContext& ctx,
		const ToolHandle<IWaveformDigiConditionsTool>& digiCondTool,
		const CaloHitCollection* hitCollection) const {

  // Create structure to store waveform for each channel
  // We return a reference to this, so it must be static
  static std::map<Identifier, std::vector<uint16_t>> waveforms;

  // Make sure this is empty
  waveforms.clear();

  // Create TOF values
  std::map<Identifier, float> time_of_flight;

  waveforms = create_waveform_map(m_ecalID);
  time_of_flight = create_calo_tof_map(m_ecalID, m_caloDetMan);

  // Make energy sums
  Identifier id;

  EvstHistMap evst_hist;

  // Make sure we clear this
  clear_evst_hist(evst_hist);

  for (const auto& hit : *hitCollection) {

    float tof;   // Time offset due to detector z position

    id = hit.getIdentifier();
    tof = time_of_flight[id];

    fill_evst_hist(evst_hist, id, hit.meanTime()-tof, hit.energyLoss());  

    // Print out hit times
    ATH_MSG_DEBUG("Calo Hit: " << id << " E: " << hit.energyLoss() 
		  << " t: " << hit.meanTime()
		  << " x: " << hit.localStartPosition().x()
		  << " y: " << hit.localStartPosition().y()
		  << " z: " << hit.localStartPosition().z()
		  << " tof: " << tof
		  );

  } // Done with loop over hits

  // Debugging printout 
  for (auto it : evst_hist) {
    ATH_MSG_DEBUG("Nonzero bins Evst hist id = " << it.first);
    for (unsigned int ibin=0; int(ibin) <= it.second->GetNbinsX(); ibin++) {
      if (it.second->GetBinContent(ibin) == 0) continue;
      ATH_MSG_DEBUG(" bin: " << ibin << " t:" << it.second->GetBinCenter(ibin) << " val: " << it.second->GetBinContent(ibin));
    }
  }

  // Now make waveforms
  convolve_waveforms(ctx, digiCondTool, evst_hist, waveforms);

  // Check what we have done
  for (const auto& w : waveforms) {
    ATH_MSG_DEBUG("Waveform for ID " << w.first);
    for(unsigned int i=400; i<450; i++) 
      ATH_MSG_DEBUG(digitizer_period()*i << "ns -> " << w.second[i]);
  }

  // Cleanup
  clear_evst_hist(evst_hist);

  return waveforms;
}


template <class ID, class DM>
std::map<Identifier, float> 
WaveformDigitisationTool::create_tof_map(const ID* idHelper, const DM* detman) const {

  std::map<Identifier, float> time_of_flight;
  for (auto itr = idHelper->pmt_begin(); itr != idHelper->pmt_end(); ++itr) {
    const ExpandedIdentifier& extId = *itr;
    Identifier pmtid = idHelper->pmt_id(extId);

    ScintDD::ScintDetectorElement* element = detman->getDetectorElement(pmtid);
    time_of_flight[pmtid] = element->center().z() / CLHEP::c_light;

    ATH_MSG_DEBUG("Found TOF for " << pmtid << " = " << time_of_flight[pmtid]);
  }
  return time_of_flight;  
}

std::map<Identifier, float> 
WaveformDigitisationTool::create_calo_tof_map(const EcalID* idHelper, const CaloDD::EcalDetectorManager* detman) const {

  std::map<Identifier, float> time_of_flight;
  for (auto itr = idHelper->pmt_begin(); itr != idHelper->pmt_end(); ++itr) {
    const ExpandedIdentifier& extId = *itr;
    Identifier pmtid = idHelper->pmt_id(extId);

    CaloDD::CaloDetectorElement* element = detman->getDetectorElement(pmtid);
    time_of_flight[pmtid] = element->center().z() / CLHEP::c_light;

    ATH_MSG_DEBUG("Found TOF for " << pmtid << " = " << time_of_flight[pmtid]);
  }
  return time_of_flight;  
}

void
WaveformDigitisationTool::convolve_waveforms(
    const EventContext& ctx,
    const ToolHandle<IWaveformDigiConditionsTool>& digiCondTool,
    const EvstHistMap& evst_hist,
    std::map<Identifier, std::vector<uint16_t>>& waveforms
					     ) const {

  // Create digitization kernel
  TF1* kernel = create_kernel(ctx, digiCondTool);

  // Convolve Evst histogram vs. kernel for each waveform
  for (const auto& w : waveforms) {

    auto id = w.first;

    // Convolve Evst histogram with time kernel

    // Get the histogram
    auto it = evst_hist.find(id);
    if (it == evst_hist.end()) {
      ATH_MSG_DEBUG("Didn't find EvsT hist for id " << id <<"!");

      // Fill waveform with background only as Evst is empty
      for (unsigned int i=0; i < digitizer_samples(); i++) {

	float baseline = generate_baseline(digiCondTool->base_mean(ctx),
					   digiCondTool->base_rms(ctx));

	// Waveform is some random baseline
	int ivalue = std::round(baseline);

	// Don't let value go below zero
	if (ivalue < 0) 
	  waveforms[id].push_back(0);
	else
	  waveforms[id].push_back(ivalue);

      } // End of loop over digitizer samples

      // Done with this waveform
      continue;
    }

    auto h = it->second;

    // Use this vector to sum our values
    // We will oversample by the same amount as the Evst histogram
    std::vector<double> dwave(digitizer_samples() * over_samples());
    // ATH_MSG_DEBUG("Creating dwave with length " << dwave.size());

    // Convolute evst signals with the time kernel
    for(unsigned int ih=1; int(ih) < h->GetNbinsX(); ih++) {

      float value = h->GetBinContent(ih);

      // If bin is empty, move on
      if (value <= 0.) continue;

      // Bin to write into in dwave
      // Histogram doesn't start at t=0, so subtract pre_samples
      int iw = ih - 1 - (pre_samples() * over_samples());

      // Convolute the histogram with the time kernel
      for (unsigned int ik=0; ik < dwave.size(); ik++) {
	// Figure out destination bin first, skip if out of range
	if ((iw + int(ik)) < 0) continue; // add int() to fix comp warnings
	if ((iw + ik) >= dwave.size()) continue;

	float ktime = ik * digitizer_period()/over_samples();
	float kval = kernel->Eval(ktime) * value;
	dwave[iw+ik] += kval;

	// if (kval > 0.01)
	//   ATH_MSG_DEBUG(" Bin " << ih << " kernel bin " << ik << " time " << ktime << " value " << kval);
      }

    } // Done with convolution

    // Debug printout
    // ATH_MSG_DEBUG("dwave array for " << id);
    // unsigned int iw=0;
    // for (auto v : dwave) {
    //   iw++;
    //   if (v <= 0.1) continue;
    //   ATH_MSG_DEBUG(" bin "<< iw << " val " << v);
    // }

    // Need to down-sample dwave to fit waveform spacing
    // Take average (or sum / over_sampling) to get amplitude correct
    std::vector<double> dswave(digitizer_samples());
    // ATH_MSG_DEBUG("Creating dswave with length " << dswave.size());

    for (unsigned int i=0; i < dwave.size(); i++) {
      dswave[i/over_samples()] += (dwave[i] / over_samples());
    }

    // Debug printout
    // ATH_MSG_DEBUG("dswave array for " << id);
    // iw=0;
    // for (auto v : dswave) {
    //   iw++;
    //   if (v <= 0.1) continue;
    //   ATH_MSG_DEBUG(" bin "<< iw << " val " << v);
    // }

    // Now fill each entry of the waveform with the down-sampled signal 
    // and some random background
    for (unsigned int i=0; i < digitizer_samples(); i++) {

      float signal = dswave[i];
      float baseline = generate_baseline(digiCondTool->base_mean(ctx),
					 digiCondTool->base_rms(ctx));

      // Waveform is some random baseline plus a negative signal
      int ivalue = std::round(baseline - signal);

      // Don't let value go below zero
      if (ivalue < 0) 
	waveforms[id].push_back(0);
      else
	waveforms[id].push_back(ivalue);

    } // End of loop over dswave

  } // End of loop over channels

  // Clean up after ourselves
  delete kernel;

}

