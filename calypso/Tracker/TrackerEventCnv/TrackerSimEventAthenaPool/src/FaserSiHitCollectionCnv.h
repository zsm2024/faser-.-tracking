/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSIHITCOLLECTIONCNV
#define FASERSIHITCOLLECTIONCNV

#include "TrackerSimEvent/FaserSiHitCollection.h"
#include "TrackerSimEventTPCnv/TrackerHits/FaserSiHitCollection_p3.h"
#include "TrackerSimEventTPCnv/TrackerHits/FaserSiHitCollectionCnv_p3.h"
#include "TrackerSimEventTPCnv/TrackerHits/FaserSiHitCollection_p3a.h"
#include "TrackerSimEventTPCnv/TrackerHits/FaserSiHitCollectionCnv_p3a.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// typedef to the latest persistent version
typedef FaserSiHitCollection_p3a     FaserSiHitCollection_PERS;
typedef FaserSiHitCollectionCnv_p3a  FaserSiHitCollectionCnv_PERS;

class FaserSiHitCollectionCnv  : public T_AthenaPoolCustomCnv<FaserSiHitCollection, FaserSiHitCollection_PERS > {
  friend class CnvFactory<FaserSiHitCollectionCnv>;
public:
  FaserSiHitCollectionCnv(ISvcLocator* svcloc) :
        T_AthenaPoolCustomCnv<FaserSiHitCollection, FaserSiHitCollection_PERS >( svcloc) {}
protected:
  FaserSiHitCollection_PERS*  createPersistent(FaserSiHitCollection* transCont);
  FaserSiHitCollection*       createTransient ();
};


#endif
