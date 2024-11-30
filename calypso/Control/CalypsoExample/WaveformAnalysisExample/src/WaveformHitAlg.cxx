#include "WaveformHitAlg.h"

WaveformHitAlg::WaveformHitAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthHistogramAlgorithm(name, pSvcLocator) { }

WaveformHitAlg::~WaveformHitAlg() { }

StatusCode WaveformHitAlg::initialize()
{
  //ATH_CHECK( m_faserWaveformHitKey.initialize() );
  //ATH_MSG_INFO( "Using Faser WaveformHit collection with key " << m_faserWaveformHitKey.key());

  return StatusCode::SUCCESS;
}


StatusCode WaveformHitAlg::execute()
{
  return StatusCode::SUCCESS;
}


StatusCode WaveformHitAlg::finalize()
{
  return StatusCode::SUCCESS;
}
