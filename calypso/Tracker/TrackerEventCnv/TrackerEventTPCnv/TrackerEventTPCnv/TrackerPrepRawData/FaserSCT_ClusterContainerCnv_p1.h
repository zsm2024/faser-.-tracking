/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_CLUSTERCONTAINERCNV_P1_H
#define FASERSCT_CLUSTERCONTAINERCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   FaserSCT_ClusterContainerCnv_p1.h
//
//-----------------------------------------------------------------------------
 
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/TrackerPRD_Container_p1.h"

#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

class MsgStream;
class FaserSCT_ID;
class StoreGateSvc;

namespace Tracker{
class FaserSCT_ClusterContainerCnv_p1 : public T_AthenaPoolTPCnvBase< Tracker::FaserSCT_ClusterContainer, Tracker::TrackerPRD_Container_p1 >
 
{
 public:
   typedef Tracker::TrackerPRD_Container_p1 PERS;
   typedef Tracker::FaserSCT_ClusterContainer TRANS;
   FaserSCT_ClusterContainerCnv_p1(): m_sctId{nullptr}, m_storeGate{nullptr}, m_SCTDetEleCollKey{"SCT_DetectorElementCollection"}, m_useDetectorElement{true}, m_isInitialized(false) {}
   virtual void transToPers(const TRANS* transCont, PERS* persCont, MsgStream &log);
   virtual void persToTrans(const PERS* persCont, TRANS* transCont, MsgStream &log);
   virtual Tracker::FaserSCT_ClusterContainer* createTransient(const Tracker::TrackerPRD_Container_p1* persObj, MsgStream& log);
   // Method for test/SCT_ClusterContainerCnv_p1_test.cxx
   void setUseDetectorElement(const bool useDetectorElement);
 private:
   const FaserSCT_ID *m_sctId;
   StoreGateSvc *m_storeGate;
   SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey;
   // Declaration of ReadCondHandleKey in SCT_ClusterContainerCnv_p1 triggers memory leak in SCT_ClusterCnv_p1_test.
   bool m_useDetectorElement;
   bool m_isInitialized;
   StatusCode initialize(MsgStream &log);
};
 
}
#endif 
 
