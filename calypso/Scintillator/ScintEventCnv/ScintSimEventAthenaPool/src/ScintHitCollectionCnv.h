/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCINTHITCOLLECTIONCNV
#define SCINTHITCOLLECTIONCNV

#include "ScintSimEvent/ScintHitCollection.h"
#include "ScintSimEventTPCnv/ScintHits/ScintHitCollection_p1.h"
#include "ScintSimEventTPCnv/ScintHits/ScintHitCollectionCnv_p1.h"
#include "ScintSimEventTPCnv/ScintHits/ScintHitCollection_p1a.h"
#include "ScintSimEventTPCnv/ScintHits/ScintHitCollectionCnv_p1a.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// typedef to the latest persistent version
typedef ScintHitCollection_p1a     ScintHitCollection_PERS;
typedef ScintHitCollectionCnv_p1a  ScintHitCollectionCnv_PERS;

class ScintHitCollectionCnv  : public T_AthenaPoolCustomCnv<ScintHitCollection, ScintHitCollection_PERS > {
  friend class CnvFactory<ScintHitCollectionCnv>;
public:
  ScintHitCollectionCnv(ISvcLocator* svcloc) :
        T_AthenaPoolCustomCnv<ScintHitCollection, ScintHitCollection_PERS >( svcloc) {}
protected:
  ScintHitCollection_PERS*  createPersistent(ScintHitCollection* transCont);
  ScintHitCollection*       createTransient ();
};


#endif
