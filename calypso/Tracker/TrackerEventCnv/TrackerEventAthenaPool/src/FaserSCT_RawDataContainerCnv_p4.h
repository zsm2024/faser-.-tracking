/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_RAWDATACONTAINERCNV_P4_H
#define FASERSCT_RAWDATACONTAINERCNV_P4_H

// FaserSCT_RawDataContainerCnv_p4, T/P separation of SCT Raw data
// author Susumu Oda <Susumu.Oda@cern.ch>

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrackerEventAthenaPool/FaserSCT_RawDataContainer_p4.h"
#include "TrackerRawData/FaserSCT_RDO_Container.h"

class FaserSCT_ID;

// We do NOT use T_AthenaPoolTPCnvIDCont because we do all of the
// conversion in the .cxx. Same as for the LAr Raw Channels
// Note that this is used for a container of SCT Raw Data
// that containes only SCR1_RawData concrete types
// FaserSCT_RDO_ContainerCnv.cxx delegates the conversion to this
// converter if the objects are of the correct type


class FaserSCT_RawDataContainerCnv_p4 : public T_AthenaPoolTPCnvBase<FaserSCT_RDO_Container, FaserSCT_RawDataContainer_p4>
{
 private:
  const FaserSCT_ID* m_sctId;
  int m_type;
 public:
  FaserSCT_RawDataContainerCnv_p4() : m_sctId(nullptr), m_type(0) {};
  
  virtual void  persToTrans(const FaserSCT_RawDataContainer_p4* persCont,
                            FaserSCT_RDO_Container* transCont,
                            MsgStream& log);
  virtual void  transToPers(const FaserSCT_RDO_Container* transCont,
                            FaserSCT_RawDataContainer_p4* persCont,
                            MsgStream& log);
  virtual FaserSCT_RDO_Container* createTransient(const FaserSCT_RawDataContainer_p4* persObj, MsgStream& log);

  // ID helper can't be used in the constructor, need initialize()
  void initialize(const FaserSCT_ID* idhelper) { m_sctId = idhelper; }
  void setType(int type) { m_type = type; }
};

#endif // FASERSCT_RAWDATACONTAINERCNV_P4_H
