#include "ScintWaveformDigiAlg.h"

#include "ScintReadoutGeometry/VetoDetectorManager.h"
#include "ScintReadoutGeometry/VetoNuDetectorManager.h"
#include "ScintReadoutGeometry/TriggerDetectorManager.h"
#include "ScintReadoutGeometry/PreshowerDetectorManager.h"

#include "ScintReadoutGeometry/ScintDetectorElement.h"

#include "GaudiKernel/PhysicalConstants.h"

#include <map>
#include <cmath>


ScintWaveformDigiAlg::ScintWaveformDigiAlg(const std::string& name, 
					 ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { 

}

StatusCode 
ScintWaveformDigiAlg::initialize() {
  ATH_MSG_INFO(name() << "::initalize()" );

  // Initalize tools
  ATH_CHECK( m_digiTool.retrieve() );
  ATH_CHECK( m_mappingTool.retrieve() );
  ATH_CHECK( m_digiCondTool.retrieve() );

  // Set key to read waveform from
  ATH_CHECK( m_scintHitContainerKey.initialize() );

  // Set key to write container
  ATH_CHECK( m_waveformContainerKey.initialize() );

  // And print out our timing simulation
  if (m_advancedTiming) {
    ATH_MSG_INFO("Using improved digitization timing");
  } else {
    ATH_MSG_INFO("Using simple digitization timing");
  }
  
  return StatusCode::SUCCESS;
}

StatusCode 
ScintWaveformDigiAlg::finalize() {
  ATH_MSG_INFO(name() << "::finalize()");

  return StatusCode::SUCCESS;
}

StatusCode 
ScintWaveformDigiAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("Executing");
  ATH_MSG_DEBUG("Run: " << ctx.eventID().run_number() << " Event: " << ctx.eventID().event_number());

  // Find the input HITS collection
  SG::ReadHandle<ScintHitCollection> scintHitHandle(m_scintHitContainerKey, ctx);

  ATH_CHECK( scintHitHandle.isValid() );
  ATH_MSG_DEBUG("Found ReadHandle for ScintHitCollection " << m_scintHitContainerKey);

  // Find the output waveform container
  SG::WriteHandle<RawWaveformContainer> waveformContainerHandle(m_waveformContainerKey, ctx);
  ATH_CHECK( waveformContainerHandle.record( std::make_unique<RawWaveformContainer>()) );

  ATH_MSG_DEBUG("WaveformsContainer '" << waveformContainerHandle.name() << "' initialized");
  
  if (scintHitHandle->size() == 0) {
    ATH_MSG_DEBUG("ScintHitCollection found with zero length!");
    return StatusCode::SUCCESS;
  }

  // Create structure to store pulse for each channel
  std::map<Identifier, std::vector<uint16_t>> waveforms;
  waveforms.clear();

  if (m_first) {
    m_first = false;

    // Write kernel parameters
    ATH_MSG_INFO(name() << ": initialize waveform digitization kernel");
    ATH_MSG_INFO(" Norm:  " << m_digiCondTool->cb_norm());
    ATH_MSG_INFO(" Mean:  " << m_digiCondTool->cb_mean());
    ATH_MSG_INFO(" Sigma: " << m_digiCondTool->cb_sigma());
    ATH_MSG_INFO(" Alpha: " << m_digiCondTool->cb_alpha());
    ATH_MSG_INFO(" N:     " << m_digiCondTool->cb_n());
  }
  
  // Try our new tool instead
  if (m_advancedTiming) {
    waveforms = m_digiTool->generate_scint_timing_waveforms(ctx, m_digiCondTool, scintHitHandle.get());
  } else {
    waveforms = m_digiTool->generate_scint_waveforms(ctx, m_digiCondTool, scintHitHandle.get());
  }

  // Loop over waveform vectors to make and store raw waveform
  unsigned int nsamples = m_digiTool->digitizer_samples();
  for (const auto& w : waveforms) {
    RawWaveform* wfm = new RawWaveform();
    wfm->setWaveform(0, w.second);
    wfm->setIdentifier(w.first);
    wfm->setSamples(nsamples);
    // Only save this waveform if channel mapping is valid
    // This will avoid making a waveform for the missing veto counter
    int channel = m_mappingTool->getChannelMapping(w.first);
    if (channel < 0) {
      ATH_MSG_DEBUG("Skipping waveform with unknown channel!");
      continue;
    }
    wfm->setChannel(channel);
    waveformContainerHandle->push_back(wfm);
  }

  ATH_MSG_DEBUG("WaveformsHitContainer " << waveformContainerHandle.name() << "' filled with "<< waveformContainerHandle->size() <<" items");

  // Cleanup 
  waveforms.clear();

  return StatusCode::SUCCESS;
}


