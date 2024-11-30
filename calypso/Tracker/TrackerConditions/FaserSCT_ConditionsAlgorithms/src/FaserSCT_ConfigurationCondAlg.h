/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_CONFIGURATIONCONDALG_H
#define FASERSCT_CONFIGURATIONCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"

// TODO use instead SCT_ConfigurationCondData?
// #include "SCT_ConditionsData/SCT_ConfigurationCondData.h"
#include "FaserSCT_ConditionsData/FaserSCT_ConfigurationCondData.h"


class FaserSCT_ID;

class FaserSCT_ConfigurationCondAlg : public AthReentrantAlgorithm {
 public:
  FaserSCT_ConfigurationCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FaserSCT_ConfigurationCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  virtual bool isClonable() const override { return true; };

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey {this, "ReadKey", "/SCT/DAQ/NoisyStrips", "Key of input noisy strips folder"};
  SG::WriteCondHandleKey<FaserSCT_ConfigurationCondData> m_writeKey{this, "WriteKey", "FaserSCT_ConfigurationCondData", "Key of output (derived) conditions data"};
  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
  const FaserSCT_ID* m_idHelper{nullptr};
  Gaudi::Property<double> m_occupancyThreshold {this, "OccupancyThreshold", 0.01, "Mask strips with an occupancy larger than the OccupancyCut"};
};



#endif //FASERSCT_CONFIGURATIONCONDALG_H
