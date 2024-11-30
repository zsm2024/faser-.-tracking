#include "CaloRecAlg.h"
#include <math.h>

CaloRecAlg::CaloRecAlg(const std::string& name, 
		       ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator) { 

}

StatusCode CaloRecAlg::initialize() {
  ATH_MSG_INFO(name() << "::initalize()" );

  // Set key to read calo hits from
  ATH_CHECK( m_caloWaveHitContainerKey.initialize() );
  ATH_CHECK( m_calo2WaveHitContainerKey.initialize() );

  // Set key to read preshower hits from
  ATH_CHECK( m_preshowerWaveHitContainerKey.initialize() );

  // Set key to write container
  ATH_CHECK( m_caloHitContainerKey.initialize() );
  ATH_CHECK( m_calo2HitContainerKey.initialize() );
  ATH_CHECK( m_preshowerHitContainerKey.initialize() );

  // Initalize tools
  ATH_CHECK( m_recoCalibTool.retrieve() );

  return StatusCode::SUCCESS;
}
//----------------------------------------------------------------------
StatusCode CaloRecAlg::finalize() {
  ATH_MSG_INFO(name() << "::finalize()");

  return StatusCode::SUCCESS;
}
//----------------------------------------------------------------------
StatusCode CaloRecAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("Executing");

  ATH_MSG_DEBUG("Run: " << ctx.eventID().run_number() 
		<< " Event: " << ctx.eventID().event_number());

  // Find the input waveform hit containers
  SG::ReadHandle<xAOD::WaveformHitContainer> caloWaveHitHandle(m_caloWaveHitContainerKey, ctx);

  ATH_CHECK( caloWaveHitHandle.isValid() );
  ATH_MSG_DEBUG("Found ReadHandle for WaveformHitContainer " << m_caloWaveHitContainerKey);

  if (caloWaveHitHandle->size() == 0) {
    ATH_MSG_DEBUG("Calorimeter Waveform Hit container found with zero length!");
  }

  SG::ReadHandle<xAOD::WaveformHitContainer> calo2WaveHitHandle(m_calo2WaveHitContainerKey, ctx);
  
  SG::ReadHandle<xAOD::WaveformHitContainer> preshowerWaveHitHandle(m_preshowerWaveHitContainerKey, ctx);

  ATH_CHECK( preshowerWaveHitHandle.isValid() );
  ATH_MSG_DEBUG("Found ReadHandle for WaveformHitContainer " << m_preshowerWaveHitContainerKey);

  if (preshowerWaveHitHandle->size() == 0) {
    ATH_MSG_DEBUG("Preshower Waveform Hit container found with zero length!");
  }

  // Only correct gain in real data
  bool correct_gain = !m_isMC;
  
  // Reconstruct calorimeter hits
  SG::WriteHandle<xAOD::CalorimeterHitContainer> caloHitContainerHandle(m_caloHitContainerKey, ctx);
  ATH_CHECK( caloHitContainerHandle.record( std::make_unique<xAOD::CalorimeterHitContainer>(),
					std::make_unique<xAOD::CalorimeterHitAuxContainer>() ) );
  ATH_MSG_DEBUG("WaveformsHitContainer '" << caloHitContainerHandle.name() << "' initialized");

  // Loop over calo hits and calibrate each primary hit
  for( const auto& hit : *caloWaveHitHandle ) {
    if (hit->status_bit(xAOD::WaveformStatus::SECONDARY)) continue;
    xAOD::CalorimeterHit* calo_hit = new xAOD::CalorimeterHit();
    caloHitContainerHandle->push_back(calo_hit);
    calo_hit->addHit(caloWaveHitHandle.get(), hit);
    m_recoCalibTool->reconstruct(ctx, calo_hit, correct_gain);
  }
  ATH_MSG_DEBUG("CaloHitContainer '" << caloHitContainerHandle.name() << "' filled with "<< caloHitContainerHandle->size() <<" items");

  // Reconstruct preshower hits
  SG::WriteHandle<xAOD::CalorimeterHitContainer> preshowerHitContainerHandle(m_preshowerHitContainerKey, ctx);
  ATH_CHECK( preshowerHitContainerHandle.record( std::make_unique<xAOD::CalorimeterHitContainer>(),
                    std::make_unique<xAOD::CalorimeterHitAuxContainer>() ) );
  ATH_MSG_DEBUG("WaveformsHitContainer '" << preshowerHitContainerHandle.name() << "' initialized");

  for( const auto& hit : *preshowerWaveHitHandle ) {
    if (hit->status_bit(xAOD::WaveformStatus::SECONDARY)) continue;
    xAOD::CalorimeterHit* calo_hit = new xAOD::CalorimeterHit();
    preshowerHitContainerHandle->push_back(calo_hit);
    calo_hit->addHit(preshowerWaveHitHandle.get(), hit);
    m_recoCalibTool->reconstruct(ctx, calo_hit, correct_gain=false);
  }  
  ATH_MSG_DEBUG("PreshowerHitContainer '" << preshowerHitContainerHandle.name() << "' filled with "<< preshowerHitContainerHandle->size() <<" items");
  

  // High-gain calo isn't guaranteed to exist
  if ( calo2WaveHitHandle.isValid() ) {
    ATH_MSG_DEBUG("Found ReadHandle for WaveformHitContainer " << m_calo2WaveHitContainerKey);

    if (calo2WaveHitHandle->size() == 0) {
      ATH_MSG_DEBUG("Calorimeter 2 Waveform Hit container found with zero length!");
    }

    // Reconstruct high-gain calorimeter hits
    SG::WriteHandle<xAOD::CalorimeterHitContainer> calo2HitContainerHandle(m_calo2HitContainerKey, ctx);
    ATH_CHECK( calo2HitContainerHandle.record( std::make_unique<xAOD::CalorimeterHitContainer>(),
					       std::make_unique<xAOD::CalorimeterHitAuxContainer>() ) );
    ATH_MSG_DEBUG("WaveformsHitContainer '" << calo2HitContainerHandle.name() << "' initialized");

    for( const auto& hit : *calo2WaveHitHandle ) {
      if (hit->status_bit(xAOD::WaveformStatus::SECONDARY)) continue;
      xAOD::CalorimeterHit* calo_hit = new xAOD::CalorimeterHit();
      calo2HitContainerHandle->push_back(calo_hit);    
      calo_hit->addHit(calo2WaveHitHandle.get(), hit);
      m_recoCalibTool->reconstruct(ctx, calo_hit, correct_gain=false);
    }
    ATH_MSG_DEBUG("Calo2HitContainer '" << calo2HitContainerHandle.name() << "' filled with "<< calo2HitContainerHandle->size() <<" items");
    
  } else {
    ATH_MSG_DEBUG("No ReadHandle for WaveformHitContainer " << m_calo2WaveHitContainerKey);
  }

  
  return StatusCode::SUCCESS;
}

