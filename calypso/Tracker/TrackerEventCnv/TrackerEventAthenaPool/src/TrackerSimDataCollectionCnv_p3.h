/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERSIMDATACOLLECTIONCNV_P3_H
#define TRACKERSIMDATACOLLECTIONCNV_P3_H

// TrackerSimDataCollectionCnv_p3, T/P separation of TrackerSimData
// author D.Costanzo <davide.costanzo@cern.ch>,O.Arnaez <olivier.arnaez@cern.ch>

#include "TrackerSimData/TrackerSimDataCollection.h"
#include "TrackerEventAthenaPool/TrackerSimDataCollection_p3.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class TrackerSimDataCollectionCnv_p3 : public T_AthenaPoolTPCnvBase<TrackerSimDataCollection, TrackerSimDataCollection_p3>
{
 public:
  TrackerSimDataCollectionCnv_p3() {};

  virtual void  persToTrans(const TrackerSimDataCollection_p3* persCont,
                            TrackerSimDataCollection* transCont,
                            MsgStream &log) ;
  virtual void  transToPers(const TrackerSimDataCollection* transCont,
                            TrackerSimDataCollection_p3* persCont,
                            MsgStream &log) ;

};

#endif
