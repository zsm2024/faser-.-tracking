#include "PseudoCaloHitToWaveformRecAlg.h"

PseudoCaloHitToWaveformRecAlg::PseudoCaloHitToWaveformRecAlg(const std::string& name, 
					 ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { 

}

StatusCode 
PseudoCaloHitToWaveformRecAlg::initialize() {
  ATH_MSG_INFO(name() << "::initalize()" );

  // Initalize tools
  ATH_CHECK( m_recoTool.retrieve() );

  // Set key to read waveform from
  ATH_CHECK( m_caloHitContainerKey.initialize() );

  // Set key to write container
  ATH_CHECK( m_waveformHitContainerKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode 
PseudoCaloHitToWaveformRecAlg::finalize() {
  ATH_MSG_INFO(name() << "::finalize()");

  return StatusCode::SUCCESS;
}

StatusCode 
PseudoCaloHitToWaveformRecAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("Executing");

  ATH_MSG_DEBUG("Run: " << ctx.eventID().run_number() 
		<< " Event: " << ctx.eventID().event_number());

  // Find the input HIT collection
  SG::ReadHandle<CaloHitCollection> caloHitHandle(m_caloHitContainerKey, ctx);

  ATH_CHECK( caloHitHandle.isValid() );
  ATH_MSG_DEBUG("Found ReadHandle for CaloHitCollection " << m_caloHitContainerKey);

  // Find the output waveform container
  SG::WriteHandle<xAOD::WaveformHitContainer> waveformHitContainerHandle(m_waveformHitContainerKey, ctx);
  ATH_CHECK( waveformHitContainerHandle.record( std::make_unique<xAOD::WaveformHitContainer>(),
						std::make_unique<xAOD::WaveformHitAuxContainer>() ) );

  ATH_MSG_DEBUG("WaveformsHitContainer '" << waveformHitContainerHandle.name() << "' initialized");

  if (caloHitHandle->size() == 0) {
    ATH_MSG_DEBUG("CaloHitCollection found with zero length!");
    return StatusCode::SUCCESS;
  }

  // "Reconstruct" the hits
  CHECK( m_recoTool->reconstruct<CaloHitCollection>(caloHitHandle.ptr(),
						     waveformHitContainerHandle.ptr()) );

  
  ATH_MSG_DEBUG("WaveformsHitContainer '" << waveformHitContainerHandle.name() << "' filled with "<< waveformHitContainerHandle->size() <<" items");
  
  return StatusCode::SUCCESS;
}

