/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_CLUSTERCONTAINERCNV_P0_H
#define FASERSCT_CLUSTERCONTAINERCNV_P0_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "AthContainers/DataVector.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

typedef DataVector<Trk::PrepRawDataCollection<Tracker::FaserSCT_Cluster> > FaserSCT_ClusterContainer_p0;

class FaserSCT_ID;
class MsgStream;

class FaserSCT_ClusterContainerCnv_p0  : public T_AthenaPoolTPCnvBase<Tracker::FaserSCT_ClusterContainer, FaserSCT_ClusterContainer_p0> {
 public:
  FaserSCT_ClusterContainerCnv_p0();

  // ID helper can't be used in the constructor, need initialize()
  StatusCode initialize( MsgStream& log );

  virtual void   persToTrans(const FaserSCT_ClusterContainer_p0*, Tracker::FaserSCT_ClusterContainer*, MsgStream&) override {
    // everything is done in createTransient()
  }

  virtual void   transToPers(const Tracker::FaserSCT_ClusterContainer*, FaserSCT_ClusterContainer_p0*, MsgStream&) override {
    throw std::runtime_error("Writing SCT PRDs in the old format is not supported");
  }

  virtual Tracker::FaserSCT_ClusterContainer* createTransient(const FaserSCT_ClusterContainer_p0* /*persObj*/, MsgStream& /*log*/) override {
    std::abort(); 
  }

  Tracker::FaserSCT_ClusterContainer* createTransient(FaserSCT_ClusterContainer_p0* persObj, MsgStream& log);

 private:
  const FaserSCT_ID*  m_sctId;
  SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey;
};

#endif
