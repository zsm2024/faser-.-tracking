#ifndef CALODIGIALGS_CALOWAVEFORMDIGIALG_H
#define CALODIGIALGS_CALOWAVEFORMDIGIALG_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Data classes
#include "WaveRawEvent/RawWaveformContainer.h"
#include "FaserCaloSimEvent/CaloHitCollection.h"

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

class CaloWaveformDigiAlg: public AthReentrantAlgorithm {

 public:
  // Constructor
  CaloWaveformDigiAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CaloWaveformDigiAlg() = default;

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
  CaloWaveformDigiAlg() = delete;
  CaloWaveformDigiAlg(const CaloWaveformDigiAlg&) = delete;
  CaloWaveformDigiAlg &operator=(const CaloWaveformDigiAlg&) = delete;
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
    {this, "DigiConditionsTool", "CaloDigiConditionsTool"};
  //@}

  /**
   * @name Input HITS using SG::ReadHandleKey
   */
  //@{
  SG::ReadHandleKey<CaloHitCollection> m_caloHitContainerKey 
  {this, "CaloHitContainerKey", ""};
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



#endif // CALODIGIALGS_CALODIGIALG_H
