#ifndef LHCDATAALG_H
#define LHCDATAALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "xAODFaserLHC/FaserLHCData.h"
#include "xAODEventInfo/EventInfo.h"

#include "LHCDataTools/ILHCDataTool.h"

class LHCDataAlg : public AthReentrantAlgorithm {
 public:
  LHCDataAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~LHCDataAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  virtual bool isClonable() const override { return true; };

 private:
  ToolHandle<ILHCDataTool> m_lhcTool{this, "LHCDataTool", "LHCDataTool"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{ this, "EventInfoKey", "EventInfo", "ReadHandleKey for xAOD::EventInfo"};
  SG::WriteHandleKey<xAOD::FaserLHCData> m_lhcDataKey
    {this, "FaserLHCDataKey", "FaserLHCData"};

  // Utility function to find nearest BCID
  int findNearest(unsigned int bcid, const std::vector<unsigned char>& bcid_mask, 
		  unsigned char mask) const;

  unsigned int 
    previousColliding(unsigned int bcid, const std::vector<unsigned char>& bcid_mask) const;

  // Find the distance to the first BCID of the previous train
  unsigned int 
    previousTrain(unsigned int bcid, const std::vector<unsigned char>& bcid_mask) const;

};

#endif // LHCDATAALG_H
