/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef FASERSCT_SIPROPERTIESCONDALG
#define FASERSCT_SIPROPERTIESCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "FaserSCT_ConditionsData/FaserSCT_DCSFloatCondData.h"
#include "FaserSiPropertiesTool/FaserSiliconPropertiesVector.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class FaserSCT_ID;

class FaserSCT_SiPropertiesCondAlg : public AthReentrantAlgorithm 
{  
 public:
  FaserSCT_SiPropertiesCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FaserSCT_SiPropertiesCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  DoubleProperty m_temperatureMin{this, "TemperatureMin", -80., "Minimum temperature allowed in Celcius."};
  DoubleProperty m_temperatureMax{this, "TemperatureMax", 100., "Maximum temperature allowed in Celcius."};
  DoubleProperty m_temperatureDefault{this, "TemperatureDefault", -7., "Default temperature in Celcius."};
  SG::ReadCondHandleKey<FaserSCT_DCSFloatCondData> m_readKeyTemp{this, "ReadKeyeTemp", "SCT_SiliconTempCondData", "Key of input sensor temperature conditions folder"};
  SG::ReadCondHandleKey<FaserSCT_DCSFloatCondData> m_readKeyHV{this, "ReadKeyHV", "SCT_SiliconBiasVoltCondData", "Key of input bias voltage conditions folder"};
  SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  SG::WriteCondHandleKey<Tracker::FaserSiliconPropertiesVector> m_writeKey{this, "WriteKey", "SCTSiliconPropertiesVector", "Key of output silicon properties conditions folder"};
  ServiceHandle<ICondSvc> m_condSvc;
  ToolHandle<ISiliconConditionsTool> m_siCondTool{this, "SiConditionsTool", "SCT_SiliconConditionsTool", "SiConditionsTool to be used"};
  const FaserSCT_ID* m_pHelper; //!< ID helper for FaserSCT
};

#endif // FASERSCT_SIPROPERTIESCONDALG
