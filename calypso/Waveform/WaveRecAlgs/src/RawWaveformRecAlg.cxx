#include "RawWaveformRecAlg.h"

RawWaveformRecAlg::RawWaveformRecAlg(const std::string& name, 
					 ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { 

}

StatusCode 
RawWaveformRecAlg::initialize() {
  ATH_MSG_INFO(name() << "::initalize()" );

  // Initalize tools
  ATH_CHECK( m_recoTool.retrieve() );

  // Set key to read waveform from
  ATH_CHECK( m_waveformContainerKey.initialize() );

  // Set key to read clock info
  ATH_CHECK( m_clockKey.initialize() );

  // Set key to write container
  ATH_CHECK( m_waveformHitContainerKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode 
RawWaveformRecAlg::finalize() {
  ATH_MSG_INFO(name() << "::finalize()");

  ATH_MSG_INFO( m_numberOfEvents << " events processed" );
  if ( m_numberOfEvents > 0) { 
    ATH_MSG_INFO( m_numberOfWaveforms   << " waveforms found over threshold" );
    ATH_MSG_INFO( m_numberOfSecondaries << " secondary waveforms found" );
    ATH_MSG_INFO( m_numberOfOverflows   << " overflows" );
    ATH_MSG_INFO( m_numberOfFitErrors   << " fit errors" );
  }

  return StatusCode::SUCCESS;
}

StatusCode 
RawWaveformRecAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("Executing");

  // Keep track of some statistics
  m_numberOfEvents++;

  ATH_MSG_DEBUG("Run: " << ctx.eventID().run_number() 
		<< " Event: " << ctx.eventID().event_number());

  // Find the input waveform container
  SG::ReadHandle<RawWaveformContainer> waveformHandle(m_waveformContainerKey, ctx);

  ATH_CHECK( waveformHandle.isValid() );
  ATH_MSG_DEBUG("Found ReadHandle for RawWaveformContainer " << m_waveformContainerKey);

  // Find the output waveform container
  SG::WriteHandle<xAOD::WaveformHitContainer> hitContainerHandle(m_waveformHitContainerKey, ctx);
  ATH_CHECK( hitContainerHandle.record( std::make_unique<xAOD::WaveformHitContainer>(),
					std::make_unique<xAOD::WaveformHitAuxContainer>() ) );

  ATH_MSG_DEBUG("WaveformsHitContainer '" << hitContainerHandle.name() << "' initialized");

  if (waveformHandle->size() == 0) {
    ATH_MSG_DEBUG("Waveform container found with zero length!");
    return StatusCode::SUCCESS;
  }

  // First reconstruct the primary hit (based on trigger time)
  for( const auto& wave : *waveformHandle) {
    ATH_MSG_DEBUG("Reconstruct primary waveform for channel " << wave->channel());
    CHECK( m_recoTool->reconstructPrimary(*wave, hitContainerHandle.ptr()) );
  }

  // Second, reconstruct any additional out of time hits
  if (m_findMultipleHits) {
    for( const auto& wave : *waveformHandle) {
      ATH_MSG_DEBUG("Reconstruct secondary waveform for channel " << wave->channel());
      CHECK( m_recoTool->reconstructSecondary(*wave, hitContainerHandle.ptr()) );
    }
  }

  // Also find the clock information
  SG::ReadHandle<xAOD::WaveformClock> clockHandle(m_clockKey, ctx);
  const xAOD::WaveformClock* clockptr = NULL;

  // Fix timing for all hits
  // Can survive without this, but make a note
  if ( clockHandle.isValid() ) {
    ATH_MSG_DEBUG("Found ReadHandle for WaveformClock");
    clockptr = clockHandle.ptr();
    CHECK( m_recoTool->setLocalTime(clockptr, hitContainerHandle.ptr()) );
  } else {
    ATH_MSG_WARNING("Didn't find ReadHandle for WaveformClock!");
  }

  ATH_MSG_DEBUG("WaveformsHitContainer '" << hitContainerHandle.name() << "' filled with "<< hitContainerHandle->size() <<" items");

  // Keep track of some statistics
  for (const auto& hit : *(hitContainerHandle.ptr())) {
    if (hit->status_bit(xAOD::WaveformStatus::THRESHOLD_FAILED)) continue;

    m_numberOfWaveforms++;
    if (hit->status_bit(xAOD::WaveformStatus::WAVE_OVERFLOW)) m_numberOfOverflows++;
    if (hit->status_bit(xAOD::WaveformStatus::SECONDARY)) m_numberOfSecondaries++;
    
    if (hit->status_bit(xAOD::WaveformStatus::GFIT_FAILED)) {
      m_numberOfFitErrors++;
    } else if (hit->status_bit(xAOD::WaveformStatus::CBFIT_FAILED)) {
      m_numberOfFitErrors++;
    }
  }

  return StatusCode::SUCCESS;
}

