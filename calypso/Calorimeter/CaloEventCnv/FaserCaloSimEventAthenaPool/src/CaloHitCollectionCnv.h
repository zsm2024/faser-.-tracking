/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOHITCOLLECTIONCNV
#define CALOHITCOLLECTIONCNV

#include "FaserCaloSimEvent/CaloHitCollection.h"
#include "FaserCaloSimEventTPCnv/CaloHits/CaloHitCollection_p1.h"
#include "FaserCaloSimEventTPCnv/CaloHits/CaloHitCollectionCnv_p1.h"
#include "FaserCaloSimEventTPCnv/CaloHits/CaloHitCollection_p1a.h"
#include "FaserCaloSimEventTPCnv/CaloHits/CaloHitCollectionCnv_p1a.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// typedef to the latest persistent version
typedef CaloHitCollection_p1a     CaloHitCollection_PERS;
typedef CaloHitCollectionCnv_p1a  CaloHitCollectionCnv_PERS;

class CaloHitCollectionCnv  : public T_AthenaPoolCustomCnv<CaloHitCollection, CaloHitCollection_PERS > {
  friend class CnvFactory<CaloHitCollectionCnv>;
public:
  CaloHitCollectionCnv(ISvcLocator* svcloc) :
        T_AthenaPoolCustomCnv<CaloHitCollection, CaloHitCollection_PERS >( svcloc) {}
protected:
  CaloHitCollection_PERS*  createPersistent(CaloHitCollection* transCont);
  CaloHitCollection*       createTransient ();
};


#endif
