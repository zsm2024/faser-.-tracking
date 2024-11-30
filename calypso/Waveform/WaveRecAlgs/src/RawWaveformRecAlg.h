#ifndef WAVERECALGS_RAWWAVEFORMRECALG_H
#define WAVERECALGS_RAWWAVEFORMRECALG_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Data classes
#include "WaveRawEvent/RawWaveformContainer.h"

#include "xAODFaserWaveform/WaveformClock.h"
#include "xAODFaserWaveform/WaveformClockAuxInfo.h"

#include "xAODFaserWaveform/WaveformHit.h"
#include "xAODFaserWaveform/WaveformHitContainer.h"
#include "xAODFaserWaveform/WaveformHitAuxContainer.h"

// Tool classes
#include "WaveRecTools/IWaveformReconstructionTool.h"

// Handles
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// STL
#include <string>

class RawWaveformRecAlg : public AthReentrantAlgorithm {

 public:
  // Constructor
  RawWaveformRecAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~RawWaveformRecAlg() = default;

  /** @name Usual algorithm methods */
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  //@}

  //
  // Look for more than one hit in each channel
  BooleanProperty m_findMultipleHits{this, "FindMultipleHits", true};

 private:

  /** @name Disallow default instantiation, copy, assignment */
  //@{
  RawWaveformRecAlg() = delete;
  RawWaveformRecAlg(const RawWaveformRecAlg&) = delete;
  RawWaveformRecAlg &operator=(const RawWaveformRecAlg&) = delete;
  //@}

  /**
   * @name Reconstruction tool
   */
  ToolHandle<IWaveformReconstructionTool> m_recoTool
    {this, "WaveformReconstructionTool", "WaveformReconstructionTool"};

  /**
   * @name Input raw waveform data using SG::ReadHandleKey
   */
  //@{
  SG::ReadHandleKey<RawWaveformContainer> m_waveformContainerKey
    {this, "WaveformContainerKey", ""};
  //@}

  /**
   * @name Input WaveformClock data using SG::ReadHandleKey
   */
  //@{
  SG::ReadHandleKey<xAOD::WaveformClock> m_clockKey
    {this, "WaveformClockKey", "WaveformClock"};
  //@}

  /**
   * @name Output data using SG::WriteHandleKey
   */
  //@{
  SG::WriteHandleKey<xAOD::WaveformHitContainer> m_waveformHitContainerKey
    {this, "WaveformHitContainerKey", ""};
  //@}

  /**
   * @name Counters
   * Use mutable to be updated in const methods.
   * AthReentrantAlgorithm is const during event processing.
   * Use std::atomic to be multi-thread safe.
   */
  //@{
  mutable std::atomic<int> m_numberOfEvents{0};
  mutable std::atomic<int> m_numberOfWaveforms{0};
  mutable std::atomic<int> m_numberOfSecondaries{0};
  mutable std::atomic<int> m_numberOfOverflows{0};
  mutable std::atomic<int> m_numberOfFitErrors{0};
  //@}

};

#endif // WAVERECALGS_RAWWAVEFORMRECALG_H
