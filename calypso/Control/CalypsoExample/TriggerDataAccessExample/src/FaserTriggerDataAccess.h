/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

/*
 * FaserTriggerDataAccess.h
 * 
 * Simple algorithm to access TLB data from storegate.
 * Try to write a proper thread-safe algorithm.
 *
 */

#ifndef FASERTRIGGERDATAACCESS_H
#define FASERTRIGGERDATAACCESS_H 

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"

#include "xAODFaserTrigger/FaserTriggerData.h"

class FaserTriggerDataAccess: public AthReentrantAlgorithm
{
 public:
  FaserTriggerDataAccess(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FaserTriggerDataAccess();

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

 private:
  /// StoreGate key
  SG::ReadHandleKey<xAOD::FaserTriggerData> m_FaserTriggerData
    { this, "FaserTriggerDataKey", "FaserTriggerData", "ReadHandleKey for xAOD::FaserTriggerData"};
};

#endif /* FASERTRIGGERDATAACCESS_H */
