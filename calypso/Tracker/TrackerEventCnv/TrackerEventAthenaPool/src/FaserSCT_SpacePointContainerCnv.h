/*
 Copyright 2021 CERN for the benefit of the FASER collaboration
*/

#ifndef FASERSCT_SPACEPOINTCONTAINERCNV_H
#define FASERSCT_SPACEPOINTCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrackerEventTPCnv/FaserSCT_SpacePointContainerCnv_p0.h"

#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "TrackerEventTPCnv/FaserSCT_SpacePointContainer_p0.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadCondHandle.h"


typedef FaserSCT_SpacePointContainer_p0 FaserSCT_SpacePointContainer_PERS;
typedef FaserSCT_SpacePointContainerCnv_p0 FaserSCT_SpacePointContainerCnv_PERS;

typedef T_AthenaPoolCustomCnv<FaserSCT_SpacePointContainer, FaserSCT_SpacePointContainer_PERS> FaserSCT_SpacePointContainerCnvBase;

class FaserSCT_SpacePointContainerCnv : public FaserSCT_SpacePointContainerCnvBase {
  friend class CnvFactory<FaserSCT_SpacePointContainerCnv>;

 public:
  FaserSCT_SpacePointContainerCnv (ISvcLocator* svcloc) : FaserSCT_SpacePointContainerCnvBase(svcloc, "FaserSCT_SpacePointContainerCnv") {}

 protected:
  // Must initialize ID helpers
//  virtual StatusCode initialize();
  virtual FaserSCT_SpacePointContainer_PERS* createPersistent (FaserSCT_SpacePointContainer* transObj);
  virtual FaserSCT_SpacePointContainer* createTransient ();
};

#endif  // SPACEPOINTCONTAINERCNV_H
