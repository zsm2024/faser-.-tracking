#include "PseudoScintHitToWaveformRecAlg.h"

PseudoScintHitToWaveformRecAlg::PseudoScintHitToWaveformRecAlg(const std::string& name, 
					 ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { 

}

StatusCode 
PseudoScintHitToWaveformRecAlg::initialize() {
  ATH_MSG_INFO(name() << "::initalize()" );

  // Initalize tools
  ATH_CHECK( m_recoTool.retrieve() );

  // Set key to read waveform from
  ATH_CHECK( m_scintHitContainerKey.initialize() );

  // Set key to write container
  ATH_CHECK( m_waveformHitContainerKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode 
PseudoScintHitToWaveformRecAlg::finalize() {
  ATH_MSG_INFO(name() << "::finalize()");

  return StatusCode::SUCCESS;
}

StatusCode 
PseudoScintHitToWaveformRecAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("Executing");

  ATH_MSG_DEBUG("Run: " << ctx.eventID().run_number() 
		<< " Event: " << ctx.eventID().event_number());

  // Find the input HIT collection
  SG::ReadHandle<ScintHitCollection> scintHitHandle(m_scintHitContainerKey, ctx);

  ATH_CHECK( scintHitHandle.isValid() );
  ATH_MSG_DEBUG("Found ReadHandle for ScintHitCollection " << m_scintHitContainerKey);

  // Find the output waveform container
  SG::WriteHandle<xAOD::WaveformHitContainer> waveformHitContainerHandle(m_waveformHitContainerKey, ctx);
  ATH_CHECK( waveformHitContainerHandle.record( std::make_unique<xAOD::WaveformHitContainer>(),
						std::make_unique<xAOD::WaveformHitAuxContainer>() ) );

  ATH_MSG_DEBUG("WaveformsHitContainer '" << waveformHitContainerHandle.name() << "' initialized");

  if (scintHitHandle->size() == 0) {
    ATH_MSG_DEBUG("ScintHitCollection found with zero length!");
    return StatusCode::SUCCESS;
  }

  // "Reconstruct" the hits
  CHECK( m_recoTool->reconstruct<ScintHitCollection>(scintHitHandle.ptr(),
						     waveformHitContainerHandle.ptr()) );

  
  ATH_MSG_DEBUG("WaveformsHitContainer '" << waveformHitContainerHandle.name() << "' filled with "<< waveformHitContainerHandle->size() <<" items");
  
  return StatusCode::SUCCESS;
}

