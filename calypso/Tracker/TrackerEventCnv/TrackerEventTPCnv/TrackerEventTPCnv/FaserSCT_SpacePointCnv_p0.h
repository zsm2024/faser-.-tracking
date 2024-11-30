/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

#ifndef FASERSCT_SPACEPOINTCNV_P0_H
#define FASERSCT_SPACEPOINTCNV_P0_H

#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerEventTPCnv/FaserSCT_SpacePoint_p0.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "AthLinks/ElementLink.h"
//#include "TrackerEventCnvTools/TrackerSpacePointCnvTool.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/StoreGateSvc.h"

class FaserSCT_SpacePointCnv_p0 : public T_AthenaPoolTPCnvBase<Tracker::FaserSCT_SpacePoint, FaserSCT_SpacePoint_p0> {
 public:
  FaserSCT_SpacePointCnv_p0():m_sctClusContName{"SCT_ClusterContainer"} {};

  virtual void persToTrans(const FaserSCT_SpacePoint_p0* persObj,
                           Tracker::FaserSCT_SpacePoint* transObj,
                           MsgStream& log);

  virtual void transToPers(const Tracker::FaserSCT_SpacePoint* transObj,
                           FaserSCT_SpacePoint_p0* persObj,
                           MsgStream& log);
  virtual StatusCode initialize( MsgStream& log );

  ElementLinkCnv_p1<ElementLink<Tracker::FaserSCT_ClusterContainer>> m_elCnv;
 private:
  SG::ReadHandleKey<Tracker::FaserSCT_ClusterContainer> m_sctClusContName;
//  ToolHandle<Tracker::TrackerSpacePointCnvTool> m_cnvtool{this, "SpacePointCnvTool","SpacePointCnvTool"};

};

#endif  // SPACEPOINTCNV_P0_H
