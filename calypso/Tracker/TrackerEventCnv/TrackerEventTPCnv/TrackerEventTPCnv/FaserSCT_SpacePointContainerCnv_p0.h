/*
 Copyright 2021 CERN for the benefit of the FASER collaboration
*/

#ifndef FASERSCT_SPACEPOINTCONTAINERCNV_P0_H
#define FASERSCT_SPACEPOINTCONTAINERCNV_P0_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "TrackerEventTPCnv/FaserSCT_SpacePointContainer_p0.h"

class FaserSCT_SpacePointContainerCnv_p0 : public T_AthenaPoolTPCnvBase<FaserSCT_SpacePointContainer, FaserSCT_SpacePointContainer_p0> {
 public:
  FaserSCT_SpacePointContainerCnv_p0() {};

  virtual void persToTrans(const FaserSCT_SpacePointContainer_p0* persObj,
                           FaserSCT_SpacePointContainer* transObj,
                           MsgStream& log);

  virtual void transToPers(const FaserSCT_SpacePointContainer* transObj, 
      FaserSCT_SpacePointContainer_p0* persObj, 
      MsgStream& log);

  virtual FaserSCT_SpacePointContainer* createTransient(const FaserSCT_SpacePointContainer_p0* persObj, MsgStream& log);
 private:
};

#endif  // SPACEPOINTCONTAINERCNV_P0_H
