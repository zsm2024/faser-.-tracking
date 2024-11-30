// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef FASERSCT_DCSCONDITIONSSTATCONDALG
#define FASERSCT_DCSCONDITIONSSTATCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "FaserSCT_ConditionsData/FaserSCT_DCSStatCondData.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class FaserSCT_DCSConditionsStatCondAlg : public AthReentrantAlgorithm 
{  
 public:
  FaserSCT_DCSConditionsStatCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FaserSCT_DCSConditionsStatCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; }

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyHV{this, "ReadKeyHV", "/SCT/DCS/HV", "Key of input (raw) HV conditions folder"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyState{this, "ReadKeyState", "/SCT/DCS/CHANSTAT", "Key of input (raw) State conditions folder"};
  SG::WriteCondHandleKey<FaserSCT_DCSStatCondData> m_writeKeyState{this, "WriteKeyState", "SCT_DCSStatCondData", "Key of output (derived) State conditions data"};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

  bool m_doState{true};
  BooleanProperty m_readAllDBFolders{this, "ReadAllDBFolders", true};
  BooleanProperty m_returnHVTemp{this, "ReturnHVTemp", true};
  StringProperty m_chanstatCut{this, "StateCut", "NORM"};
  FloatProperty m_hvLowLimit{this, "HVCutLow", 0.0};
  FloatProperty m_hvUpLimit{this, "HVCutUp", 1000000.0};
  BooleanProperty m_useHV{this, "UseDefaultHV", false};
  FloatProperty m_useHVLowLimit{this, "useHVLow", 19.};
  FloatProperty m_useHVUpLimit{this, "useHVUp", 1000000.0};
  StringProperty m_useHVChanCut{this, "useHVChan", "LOOSE"};
};

#endif // FASERSCT_DCSCONDITIONSSTATCONDALG
