#ifndef WAVERECALGS_PSEUDOCALOHITTOWAVEFORMRECALG_H
#define WAVERECALGS_PSEUDOCALOHITTOWAVEFORMRECALG_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Data classes
#include "FaserCaloSimEvent/CaloHitCollection.h"

#include "xAODFaserWaveform/WaveformHit.h"
#include "xAODFaserWaveform/WaveformHitContainer.h"
#include "xAODFaserWaveform/WaveformHitAuxContainer.h"

// Tool classes
#include "WaveRecTools/IPseudoSimToWaveformRecTool.h"

// Handles
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// STL
#include <string>

class PseudoCaloHitToWaveformRecAlg : public AthReentrantAlgorithm {

 public:
  // Constructor
  PseudoCaloHitToWaveformRecAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PseudoCaloHitToWaveformRecAlg() = default;

  /** @name Usual algorithm methods */
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  //@}

 private:

  /** @name Disallow default instantiation, copy, assignment */
  //@{
  PseudoCaloHitToWaveformRecAlg() = delete;
  PseudoCaloHitToWaveformRecAlg(const PseudoCaloHitToWaveformRecAlg&) = delete;
  PseudoCaloHitToWaveformRecAlg &operator=(const PseudoCaloHitToWaveformRecAlg&) = delete;
  //@}

  /**
   * @name Reconstruction tool
   */
  ToolHandle<IPseudoSimToWaveformRecTool> m_recoTool
    {this, "PseudoSimToWaveformRecTool", "PseudoSimToWaveformRecTool"};

  /**
   * @name Input simulated HITS using SG::ReadHandleKey
   */
  //@{
  SG::ReadHandleKey<CaloHitCollection> m_caloHitContainerKey 
  {this, "CaloHitContainerKey", ""};
  //@}

  /**
   * @name Output data using SG::WriteHandleKey
   */
  //@{
  SG::WriteHandleKey<xAOD::WaveformHitContainer> m_waveformHitContainerKey
    {this, "WaveformHitContainerKey", ""};
  //@}

};

#endif // WAVERECALGS_PSEUDOCALOHITTOWAVEFORMRECALG_H
