/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_CLUSTERCONTAINERCNV_P3_H
#define FASERSCT_CLUSTERCONTAINERCNV_P3_H

// SCT_DriftCircleContainerCnv_p3, T/P separation of SCT PRD
// author D.Costanzo <davide.costanzo@cern.ch>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerEventTPCnv/FaserSCT_ClusterContainer_p3.h"

#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

class FaserSCT_ID;
class StoreGateSvc;

class FaserSCT_ClusterContainerCnv_p3 : public T_AthenaPoolTPCnvBase<Tracker::FaserSCT_ClusterContainer, Tracker::FaserSCT_ClusterContainer_p3>
{
 public:
  FaserSCT_ClusterContainerCnv_p3() : m_sctId{nullptr}, m_storeGate{nullptr}, m_SCTDetEleCollKey{"SCT_DetectorElementCollection"}, m_useDetectorElement{true}, m_isInitialized{false} {};
  
  virtual void transToPers(const Tracker::FaserSCT_ClusterContainer* transCont,
                           Tracker::FaserSCT_ClusterContainer_p3* persCont,
                           MsgStream &log) ;
  virtual void persToTrans(const Tracker::FaserSCT_ClusterContainer_p3* persCont,
                           Tracker::FaserSCT_ClusterContainer* transCont,
                           MsgStream &log) ;

  virtual Tracker::FaserSCT_ClusterContainer* createTransient(const Tracker::FaserSCT_ClusterContainer_p3* persObj, MsgStream& log);

  // Methods for test/SCT_ClusterContainerCnv_p3_test.cxx
  void setIdHelper(const FaserSCT_ID* sct_id);
  void setUseDetectorElement(const bool useDetectorElement);

 private:
  const FaserSCT_ID *m_sctId;
  StoreGateSvc *m_storeGate;
  SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey;
  bool m_useDetectorElement;
  bool m_isInitialized;
  StatusCode initialize(MsgStream &log);

};

#endif
