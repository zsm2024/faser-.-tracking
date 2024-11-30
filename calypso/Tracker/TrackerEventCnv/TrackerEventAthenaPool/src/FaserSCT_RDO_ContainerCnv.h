/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_RDO_CONTAINERCNV_H
#define FASERSCT_RDO_CONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// #include "SCT_RawDataContainerCnv_p1.h"
// #include "SCT_RawDataContainerCnv_p2.h"
// #include "SCT_RawDataContainerCnv_p3.h"
#include "FaserSCT_RawDataContainerCnv_p4.h"
#include "FaserSCT_RDO_ContainerCnv_p0.h"
// #include "FaserSCT1_RawDataContainerCnv_p1.h"

#include "TrackerRawData/FaserSCT_RDO_Container.h"
#include "TrackerRawData/FaserSCT_RDO_Collection.h"
#include "StoreGate/StoreGateSvc.h"

// the latest persistent representation type of DataCollection:
typedef FaserSCT_RawDataContainer_p4    FaserSCT_RDO_Container_PERS;
typedef FaserSCT_RawDataContainerCnv_p4 FaserSCT_RDO_ContainerCnv_PERS;

typedef T_AthenaPoolCustomCnv<FaserSCT_RDO_Container, FaserSCT_RDO_Container_PERS> FaserSCT_RDO_ContainerCnvBase;

class FaserSCT_RDO_ContainerCnv : public FaserSCT_RDO_ContainerCnvBase {
  friend class CnvFactory<FaserSCT_RDO_ContainerCnv>;

  // Converters need to be initialized (use ID helpers)
  // Thus they can't be local
  FaserSCT_RDO_ContainerCnv_p0      m_converter_p0;
//   FaserSCT1_RawDataContainerCnv_p1  m_converter_TP1;
//   SCT_RawDataContainerCnv_p1   m_converter_SCT_TP1;
//   SCT_RawDataContainerCnv_p2   m_converter_SCT_TP2;
//   SCT_RawDataContainerCnv_p3   m_converter_SCT_TP3;
  FaserSCT_RawDataContainerCnv_p4   m_converter_SCT_TP4;
  FaserSCT_RDO_ContainerCnv_PERS    m_converter_PERS;

  // Should not be needed at some point.
  StoreGateSvc*  m_storeGate;

 protected:
 public:
  FaserSCT_RDO_ContainerCnv (ISvcLocator* svcloc);
 protected:
  virtual FaserSCT_RDO_Container_PERS* createPersistent (FaserSCT_RDO_Container* transCont);
  virtual FaserSCT_RDO_Container* createTransient ();

  // Must initialize ID helpers
  virtual StatusCode initialize();
};

#endif
