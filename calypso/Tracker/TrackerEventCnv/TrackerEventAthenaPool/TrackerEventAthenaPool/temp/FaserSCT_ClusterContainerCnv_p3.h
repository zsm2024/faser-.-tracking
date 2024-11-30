/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FaserSCT_CLUSTERCONTAINERCNV_P3_H
#define FaserSCT_CLUSTERCONTAINERCNV_P3_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerEventAthenaPool/FaserSCT_ClusterContainer_p3.h"

#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

class FaserSCT_ID;
class StoreGateSvc;

class FaserSCT_ClusterContainerCnv_p3 : public T_AthenaPoolTPCnvBase<FaserSCT_ClusterContainer, FaserSCT_ClusterContainer_p3>
{
 public:
  FaserSCT_ClusterContainerCnv_p3() : m_sctId{nullptr}, m_storeGate{nullptr}, m_SCTDetEleCollKey{"SCT_DetectorElementCollection"}, m_useDetectorElement{true}, m_isInitialized{false} {};
  
  virtual void transToPers(const FaserSCT_ClusterContainer* transCont,
                           FaserSCT_ClusterContainer_p3* persCont,
                           MsgStream &log) ;
  virtual void persToTrans(const FaserSCT_ClusterContainer_p3* persCont,
                           FaserSCT_ClusterContainer* transCont,
                           MsgStream &log) ;

  virtual FaserSCT_ClusterContainer* createTransient(const FaserSCT_ClusterContainer_p3* persObj, MsgStream& log);

  // Methods for test/FaserSCT_ClusterContainerCnv_p3_test.cxx
  void setIdHelper(const FaserSCT_ID* sct_id);
  void setUseDetectorElement(const bool useDetectorElement);
  StatusCode initialize(MsgStream &log);

 private:
  const FaserSCT_ID *m_sctId;
  StoreGateSvc *m_storeGate;
  SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey;
  bool m_useDetectorElement;
  bool m_isInitialized;

};

#endif
