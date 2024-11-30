#include "WaveClockRecAlg.h"

WaveClockRecAlg::WaveClockRecAlg(const std::string& name, 
				   ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { 
}

StatusCode 
WaveClockRecAlg::initialize() {
  ATH_MSG_INFO(name() << "::initalize()" );

  // Initalize tools
  ATH_CHECK( m_recoTool.retrieve() );

  // Set key to read waveform from
  ATH_CHECK( m_waveformContainerKey.initialize() );

  // Set key to write container
  ATH_CHECK( m_waveformClockKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode 
WaveClockRecAlg::finalize() {
  ATH_MSG_INFO(name() << "::finalize()");
  ATH_MSG_INFO( m_numberOfEvents << " events processed" );
  return StatusCode::SUCCESS;
}

StatusCode 
WaveClockRecAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("Executing");

  ATH_MSG_DEBUG("Run: " << ctx.eventID().run_number() 
		<< " Event: " << ctx.eventID().event_number());

  // Find the input waveform container
  SG::ReadHandle<RawWaveformContainer> waveformHandle(m_waveformContainerKey, ctx);

  ATH_CHECK( waveformHandle.isValid() );
  ATH_MSG_DEBUG("Found ReadHandle for Waveforms");

  if (waveformHandle->size() == 0) {
    ATH_MSG_DEBUG("Waveform container found with zero length!");
    return StatusCode::SUCCESS;
  }

  // Check length (should be one)
  if (waveformHandle->size() != 1) {
    ATH_MSG_WARNING("Found waveform container " << m_waveformContainerKey 
		    << " with size " << waveformHandle->size() << "!");
  }

  // Create the output clock container
  SG::WriteHandle<xAOD::WaveformClock> clockHandle(m_waveformClockKey, ctx);
  //ATH_CHECK( clockHandle.record( std::make_unique<WaveformClock>() ) );
  ATH_CHECK( clockHandle.record( std::make_unique<xAOD::WaveformClock>(),
				 std::make_unique<xAOD::WaveformClockAuxInfo>() ) );

  auto clock = clockHandle.ptr();

  // Reconstruct first element
  for(const auto& wave : *waveformHandle) {

    ATH_MSG_DEBUG("Reconstruct waveform for channel " << wave->channel());

    // Make some sanity checks before going further
    if (wave->adc_counts().size() == 0) {
      ATH_MSG_ERROR( "Found waveform for channel " << wave->channel() 
		     << " with size " << wave->adc_counts().size() << "!");
      // Create dummy hit here, or just skip?
      return StatusCode::FAILURE;
    }
    
    if (wave->adc_counts().size() != wave->n_samples()) {
      ATH_MSG_WARNING( "Found waveform for channel " << wave->channel() 
		       << " with size " << wave->adc_counts().size() 
		       << " not equal to number of samples " << wave->n_samples());
      return StatusCode::SUCCESS;
    }

    // Reconstruct the hits
    CHECK( m_recoTool->reconstruct(*wave, clock) );

    // Keep track of how many we reconstructed
    m_numberOfEvents++;

    // Only do one if there happen to be more
    break;
  }

  ATH_MSG_DEBUG(waveformHandle.name() << " created");

  return StatusCode::SUCCESS;
}

