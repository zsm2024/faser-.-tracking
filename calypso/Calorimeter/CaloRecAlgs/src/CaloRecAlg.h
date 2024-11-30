#ifndef CALORECALGS_CALORECALG_H
#define CALORECALGS_CALORECALG_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// Data classes
#include "xAODFaserWaveform/WaveformHitContainer.h"

#include "xAODFaserCalorimeter/CalorimeterHit.h"
#include "xAODFaserCalorimeter/CalorimeterHitContainer.h"
#include "xAODFaserCalorimeter/CalorimeterHitAuxContainer.h"

// Tool classes
#include "CaloRecTools/ICaloRecTool.h"

// Include Athena stuff for Conditions db reading
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

// Handles
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

// ROOT
#include "TF1.h"

// STL
#include <string>
#include <vector>

class CaloRecAlg : public AthReentrantAlgorithm {

 public:
  // Constructor
  CaloRecAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CaloRecAlg() = default;

  /** @name Usual algorithm methods */
  //@{
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  //@}

 private:

  /** @name Disallow default instantiation, copy, assignment */
  //@{
  CaloRecAlg() = delete;
  CaloRecAlg(const CaloRecAlg&) = delete;
  CaloRecAlg &operator=(const CaloRecAlg&) = delete;
  //@}

  /**
   * @name Reconstruction tool
   */
  ToolHandle<ICaloRecTool> m_recoCalibTool {this, "CaloRecTool", "CaloRecTool"};

  /**
   * @name Input raw waveform data using SG::ReadHandleKey
   */
  //@{
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_caloWaveHitContainerKey {this, "CaloWaveHitContainerKey", "CaloWaveformHits"};
  //@}

  //@{
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_calo2WaveHitContainerKey {this, "Calo2WaveHitContainerKey", "Calo2WaveformHits"};
  //@}

  //@{
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_preshowerWaveHitContainerKey {this, "PreshowerWaveHitContainerKey", "PreshowerWaveformHits"};
  //@}

  /**
   * @name Output data using SG::WriteHandleKey
   */
  //@{
  SG::WriteHandleKey<xAOD::CalorimeterHitContainer> m_caloHitContainerKey {this, "CaloHitContainerKey", "CaloHits"};
  SG::WriteHandleKey<xAOD::CalorimeterHitContainer> m_calo2HitContainerKey {this, "Calo2HitContainerKey", "Calo2Hits"};
  SG::WriteHandleKey<xAOD::CalorimeterHitContainer> m_preshowerHitContainerKey {this, "PreshowerHitContainerKey", "PreshowerHits"};
  //@}

  Gaudi::Property<bool> m_isMC {this, "isMC", false};

};

#endif // CALORECALGS_CALORECALG_H
