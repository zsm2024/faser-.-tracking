#ifndef WAVEFORMANALYSISEXAMPLE_WAVEFORMHITALG_H
#define WAVEFORMANALYSISEXAMPLE_WAVEFORMHITALG_H

#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "xAODFaserWaveform/WaveformHitContainer.h"

#include <TH1.h>

/* WaveformHit reading example - Carl Gwilliam + Lottie Cavanagh, Liverpool */

class WaveformHitAlg : public AthHistogramAlgorithm
{
public:
  WaveformHitAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual ~WaveformHitAlg();
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  
  SG::ReadHandle<xAOD::WaveformHitContainer> m_waveformHits;   
  //SG::ReadHandleKey<xAOD::WaveformHitContainer> m_faserWaveformHitKey { this, "WaveformHitContainerKey", "CaloWaveformHits" };
};

#endif
