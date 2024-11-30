#ifndef WAVERECALGS_WAVECLOCKRECALG_H
#define WAVERECALGS_WAVECLOCKRECALG_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Input data
#include "WaveRawEvent/RawWaveformContainer.h"

// Output data
// #include "WaveRecEvent/WaveformClock.h"
#include "xAODFaserWaveform/WaveformClock.h"
#include "xAODFaserWaveform/WaveformClockAuxInfo.h"
#include "WaveRecTools/IClockReconstructionTool.h"

#include "StoreGate/ReadHandleKey.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// STL
#include <string>

class WaveClockRecAlg : public AthReentrantAlgorithm {

 public:
  // Constructor
  WaveClockRecAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~WaveClockRecAlg() = default;

  /** @name Usual algorithm methods */
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  //@}

 private:

  /** @name Disallow default instantiation, copy, assignment */
  //@{
  WaveClockRecAlg() = delete;
  WaveClockRecAlg(const WaveClockRecAlg&) = delete;
  WaveClockRecAlg &operator=(const WaveClockRecAlg&) = delete;
  //@}

  /**
   * @name Reconstruction tool
   */
  ToolHandle<IClockReconstructionTool> m_recoTool
    {this, "ClockReconstructionTool", "ClockReconstructionTool"};

  /**
   * @name Input data using SG::ReadHandleKey
   */
  //@{
  SG::ReadHandleKey<RawWaveformContainer> m_waveformContainerKey
    {this, "WaveformContainerKey", "ClockWaveforms"};
  //@}

  /**
   * @name Output data using SG::WriteHandleKey
   */
  //@{
  SG::WriteHandleKey<xAOD::WaveformClock> m_waveformClockKey
    {this, "WaveformClockKey", "WaveformClock"};
  //@}

  /**
   * @name Counters
   * Use mutable to be updated in const methods.
   * AthReentrantAlgorithm is const during event processing.
   * Use std::atomic to be multi-thread safe.
   */
  //@{
  mutable std::atomic<int> m_numberOfEvents{0};
  //@}

};

#endif // WAVERECALGS_WAVECLOCKRECALG_H
