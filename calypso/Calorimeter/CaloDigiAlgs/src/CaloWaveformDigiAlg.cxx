#include "CaloWaveformDigiAlg.h"

#include "FaserCaloSimEvent/CaloHitIdHelper.h"

#include "CaloReadoutGeometry/EcalDetectorManager.h"

#include "CaloReadoutGeometry/CaloDetectorElement.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <map>
#include <utility>
#include <cmath>

CaloWaveformDigiAlg::CaloWaveformDigiAlg(const std::string& name, 
					 ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{ 

}

StatusCode 
CaloWaveformDigiAlg::initialize() {
  ATH_MSG_INFO(name() << "::initalize()");

  // Initalize tools
  ATH_CHECK( m_digiTool.retrieve() );
  ATH_CHECK( m_mappingTool.retrieve() );
  ATH_CHECK( m_digiCondTool.retrieve() );

  // Set key to read waveform from
  ATH_CHECK( m_caloHitContainerKey.initialize() );

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
CaloWaveformDigiAlg::finalize() {
  ATH_MSG_INFO(name() << "::finalize()");

  return StatusCode::SUCCESS;
}

StatusCode 
CaloWaveformDigiAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("Executing");
  ATH_MSG_DEBUG("Run: " << ctx.eventID().run_number() << " Event: " << ctx.eventID().event_number());

  // Find the input HIT collection
  SG::ReadHandle<CaloHitCollection> caloHitHandle(m_caloHitContainerKey, ctx);

  ATH_CHECK( caloHitHandle.isValid() );
  ATH_MSG_DEBUG("Found ReadHandle for CaloHitCollection " << m_caloHitContainerKey);

  // Find the output waveform container
  SG::WriteHandle<RawWaveformContainer> waveformContainerHandle(m_waveformContainerKey, ctx);
  ATH_CHECK( waveformContainerHandle.record( std::make_unique<RawWaveformContainer>()) );

  ATH_MSG_DEBUG("WaveformsContainer '" << waveformContainerHandle.name() << "' initialized");

  if (caloHitHandle->size() == 0) {
    ATH_MSG_DEBUG("CaloHitCollection found with zero length!");
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
    waveforms = m_digiTool->generate_calo_timing_waveforms(ctx, m_digiCondTool, caloHitHandle.get());
  } else {
    waveforms = m_digiTool->generate_calo_waveforms(ctx, m_digiCondTool, caloHitHandle.get());
  }

  // Loop over wavefrom vectors to make and store waveform
  unsigned int digitizer_samples = m_digiTool->digitizer_samples();
  for (const auto& w : waveforms) {
    RawWaveform* wfm = new RawWaveform();    		  
    wfm->setWaveform(0, w.second);
    wfm->setIdentifier(w.first);
    wfm->setSamples(digitizer_samples);
    // Don't write out waveforms with no channel defined
    // Causes problems in downstream reco (not known in cable map
    int channel = m_mappingTool->getChannelMapping(w.first);
    if (channel < 0) {
      ATH_MSG_DEBUG("Skipping waveform with unknown channel!");
      continue;
    }
    wfm->setChannel(channel);
    waveformContainerHandle->push_back(wfm);
  }

  //m_chrono->chronoStop("Write");

  ATH_MSG_DEBUG("WaveformsHitContainer " << waveformContainerHandle.name() << "' filled with "<< waveformContainerHandle->size() <<" items");

  // Cleanup
  waveforms.clear();

  return StatusCode::SUCCESS;
}

