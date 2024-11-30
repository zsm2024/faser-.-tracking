/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_RDO_CONTAINERCNV_P0_H
#define FASERSCT_RDO_CONTAINERCNV_P0_H

#include "AthContainers/DataVector.h"

#include "TrackerRawData/FaserSCT_RDO_Container.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrackerRawData/FaserSCT1_RawData.h"

class FaserSCT_ID;

typedef  DataVector<TrackerRawDataCollection< FaserSCT1_RawData > >  FaserSCT_RDO_Container_p0;

class MsgStream;
class FaserSCT_RDO_ContainerCnv_p0  : public T_AthenaPoolTPCnvBase<FaserSCT_RDO_Container, FaserSCT_RDO_Container_p0> {
   const FaserSCT_ID*  m_sctId{nullptr};
public:
  virtual void   persToTrans(const FaserSCT_RDO_Container_p0*, FaserSCT_RDO_Container*, MsgStream&) {
    // everything is done in createTransient()
  }

  virtual void   transToPers(const FaserSCT_RDO_Container*, FaserSCT_RDO_Container_p0*, MsgStream&)  {
    throw std::runtime_error("Writing SCT RDOs in the old format is not supported");
  }

  virtual FaserSCT_RDO_Container* createTransient(const FaserSCT_RDO_Container_p0* persObj, MsgStream& log);

  // ID helper can't be used in the constructor, need initialize()
  void initialize(const FaserSCT_ID* idhelper) { m_sctId = idhelper; }
};

#endif/*FASERSCT_RDO_CONTAINERCNV_P0_H*/
