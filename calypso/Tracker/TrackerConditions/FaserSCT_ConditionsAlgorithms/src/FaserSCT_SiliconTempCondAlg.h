/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef FASERSCT_SILICONTEMPCONDALG
#define FASERSCT_SILICONTEMPCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "FaserSCT_ConditionsData/FaserSCT_DCSStatCondData.h"
#include "FaserSCT_ConditionsData/FaserSCT_DCSFloatCondData.h"
#include "FaserSCT_ConditionsTools/ISCT_DCSConditionsTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"

class FaserSCT_ID;

class FaserSCT_SiliconTempCondAlg : public AthReentrantAlgorithm
{  
 public:
  FaserSCT_SiliconTempCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FaserSCT_SiliconTempCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  BooleanProperty m_useState{this, "UseState", true, "Flag to use state conditions folder"};
  SG::ReadCondHandleKey<FaserSCT_DCSStatCondData> m_readKeyState{this, "ReadKeyState", "SCT_DCSStatCondData", "Key of input state conditions data"};
  SG::ReadCondHandleKey<FaserSCT_DCSFloatCondData> m_readKeyTemp0{this, "ReadKeyTemp", "SCT_DCSTemp0CondData", "Key of input (hybrid) temperature conditions data"};
  SG::WriteCondHandleKey<FaserSCT_DCSFloatCondData> m_writeKey{this, "WriteKey", "SCT_SiliconTempCondData", "Key of output (sensor) temperature conditions data"};
  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
  ToolHandle<ISCT_DCSConditionsTool> m_sctDCSTool{this, "DCSConditionsTool", "TrackerSCT_DCSConditionsTool", "Tool to retrived SCT DCS information"};
  const FaserSCT_ID* m_pHelper{nullptr}; //!< ID helper for FaserSCT
};

#endif // FASERSCT_SILICONTEMPCONDALG
