#ifndef SCINTDIGIALGS_SCINTWAVEFORMDIGIALG_H
#define SCINTDIGIALGS_SCINTWAVEFORMDIGIALG_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Data classes
#include "WaveRawEvent/RawWaveformContainer.h"
#include "ScintSimEvent/ScintHitCollection.h"

// Tool classes
#include "WaveDigiTools/IWaveformDigitisationTool.h"
#include "WaveformConditionsTools/IWaveformCableMappingTool.h"
#include "WaveformConditionsTools/IWaveformDigiConditionsTool.h"

// Handles
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// STL
#include <string>
#include <vector>

class ScintWaveformDigiAlg : public AthReentrantAlgorithm {

 public:
  // Constructor
  ScintWaveformDigiAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ScintWaveformDigiAlg() = default;

  /** @name Usual algorithm methods */
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  //@}

  //
  // Simulate detailed timing of waveforms
  BooleanProperty m_advancedTiming{this, "AdvancedTiming", true};

 private:

  /** @name Disallow default instantiation, copy, assignment */
  //@{
  ScintWaveformDigiAlg() = delete;
  ScintWaveformDigiAlg(const ScintWaveformDigiAlg&) = delete;
  ScintWaveformDigiAlg &operator=(const ScintWaveformDigiAlg&) = delete;
  //@}

  /**
   * @name Digitisation tool
   */
  //@{
  ToolHandle<IWaveformDigitisationTool> m_digiTool
    {this, "WaveformDigitisationTool", "WaveformDigitisationTool"};
  //@}

  /**
   * @name Mapping tool
   */
  //@{
  ToolHandle<IWaveformCableMappingTool> m_mappingTool
    {this, "WaveformCableMappingTool", "WaveformCableMappingTool"};
  //@}

  /**
   * @name Digitization parameters tool
   */
  //@{
  ToolHandle<IWaveformDigiConditionsTool> m_digiCondTool
    {this, "DigiConditionsTool", ""};
  //@}

  /**
   * @name Input HITS using SG::ReadHandleKey
   */
  //@{
  SG::ReadHandleKey<ScintHitCollection> m_scintHitContainerKey 
  {this, "ScintHitContainerKey", ""};
  //@}

  /**
   * @name Output data using SG::WriteHandleKey
   */
  //@{
  SG::WriteHandleKey<RawWaveformContainer> m_waveformContainerKey
    {this, "WaveformContainerKey", ""};
  //@}

  mutable bool m_first {true};
};


#endif // SCINTDIGIALGS_SCINTDIGIALG_H
