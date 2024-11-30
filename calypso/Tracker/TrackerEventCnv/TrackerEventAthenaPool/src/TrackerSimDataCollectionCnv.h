/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERSIMDATACOLLECTIONCNV_H
#define TRACKERSIMDATACOLLECTIONCNV_H
                                                                                                                                                             
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
                                                                                                                                                             
#include "TrackerSimData/TrackerSimDataCollection.h"
// #include "TrackerSimDataCollectionCnv_p1.h"
// #include "TrackerSimDataCollectionCnv_p2.h"
#include "TrackerSimDataCollectionCnv_p3.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// typedef to the latest persistent version
typedef  TrackerSimDataCollection_p3     TrackerSimDataCollection_PERS;
typedef  TrackerSimDataCollectionCnv_p3  TrackerSimDataCollectionCnv_PERS;

// base class 
typedef  T_AthenaPoolCustomCnv<TrackerSimDataCollection, TrackerSimDataCollection_PERS >   TrackerSimDataCollectionCnvBase;

class TrackerSimDataCollectionCnv : public TrackerSimDataCollectionCnvBase {
  friend class CnvFactory<TrackerSimDataCollectionCnv >;
                                                                                                                                                             
protected:
public:
  TrackerSimDataCollectionCnv (ISvcLocator* svcloc) : TrackerSimDataCollectionCnvBase(svcloc) {}
protected:
  virtual TrackerSimDataCollection_PERS*   createPersistent (TrackerSimDataCollection* transCont);
  virtual TrackerSimDataCollection* createTransient ();
};
                                                                                                                                                             
#endif
