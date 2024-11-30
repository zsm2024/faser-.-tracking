// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef FASERSCT_DCSCONDITIONSHVCONDALG
#define FASERSCT_DCSCONDITIONSHVCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "FaserSCT_ConditionsData/FaserSCT_DCSFloatCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class FaserSCT_DCSConditionsHVCondAlg : public AthReentrantAlgorithm 
{  
 public:
  FaserSCT_DCSConditionsHVCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FaserSCT_DCSConditionsHVCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/SCT/DCS/HV", "Key of input (raw) HV conditions folder"};
  SG::WriteCondHandleKey<FaserSCT_DCSFloatCondData> m_writeKey{this, "WriteKey", "SCT_DCSHVCondData", "Key of output (derived) HV conditions data"};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

  BooleanProperty m_returnHVTemp{this, "ReturnHVTemp", true};
};

#endif // SCT_DCSCONDITIONSHVCONDALG
