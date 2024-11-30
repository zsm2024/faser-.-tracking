/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FaserSCT_CLUSTERCONTAINERCNV_P0_H
#define FaserSCT_CLUSTERCONTAINERCNV_P0_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "AthContainers/DataVector.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

typedef DataVector<TrackerRawDataCollection<FaserSCT_Cluster> > FaserSCT_ClusterContainer_p0;

class FaserSCT_ID;
class MsgStream;

class FaserSCT_ClusterContainerCnv_p0  : public T_AthenaPoolTPCnvBase<FaserSCT_ClusterContainer, FaserSCT_ClusterContainer_p0> {
 public:
  FaserSCT_ClusterContainerCnv_p0();

  // ID helper can't be used in the constructor, need initialize()
  StatusCode initialize( MsgStream& log );

  virtual void   persToTrans(const FaserSCT_ClusterContainer_p0*, FaserSCT_ClusterContainer*, MsgStream&) override {
    // everything is done in createTransient()
  }

  virtual void   transToPers(const FaserSCT_ClusterContainer*, FaserSCT_ClusterContainer_p0*, MsgStream&) override {
  //  throw std::runtime_error("Writing SCT PRDs in the old format is not supported");
  }

  virtual FaserSCT_ClusterContainer* createTransient(const FaserSCT_ClusterContainer_p0* /*persObj*/, MsgStream& /*log*/) override {
    std::abort(); 
  }

  FaserSCT_ClusterContainer* createTransient(FaserSCT_ClusterContainer_p0* persObj, MsgStream& log);

 private:
  const FaserSCT_ID*  m_sctId;
  SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey;
};

#endif
