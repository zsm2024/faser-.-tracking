#ifndef WAVERECALGS_PSEUDOSCINTHITTOWAVEFORMRECALG_H
#define WAVERECALGS_PSEUDOSCINTHITTOWAVEFORMRECALG_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Data classes
#include "ScintSimEvent/ScintHitCollection.h"

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

class PseudoScintHitToWaveformRecAlg : public AthReentrantAlgorithm {

 public:
  // Constructor
  PseudoScintHitToWaveformRecAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PseudoScintHitToWaveformRecAlg() = default;

  /** @name Usual algorithm methods */
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  //@}

 private:

  /** @name Disallow default instantiation, copy, assignment */
  //@{
  PseudoScintHitToWaveformRecAlg() = delete;
  PseudoScintHitToWaveformRecAlg(const PseudoScintHitToWaveformRecAlg&) = delete;
  PseudoScintHitToWaveformRecAlg &operator=(const PseudoScintHitToWaveformRecAlg&) = delete;
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
  SG::ReadHandleKey<ScintHitCollection> m_scintHitContainerKey 
  {this, "ScintHitContainerKey", ""};
  //@}

  /**
   * @name Output data using SG::WriteHandleKey
   */
  //@{
  SG::WriteHandleKey<xAOD::WaveformHitContainer> m_waveformHitContainerKey
    {this, "WaveformHitContainerKey", ""};
  //@}

};

#endif // WAVERECALGS_PSEUDOSCINTHITTOWAVEFORMRECALG_H
