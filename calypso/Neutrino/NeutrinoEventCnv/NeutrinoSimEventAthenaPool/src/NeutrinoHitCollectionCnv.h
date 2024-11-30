/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NEUTRINOHITCOLLECTIONCNV
#define NEUTRINOHITCOLLECTIONCNV

#include "NeutrinoSimEvent/NeutrinoHitCollection.h"
#include "NeutrinoSimEventTPCnv/NeutrinoHits/NeutrinoHitCollection_p1a.h"
#include "NeutrinoSimEventTPCnv/NeutrinoHits/NeutrinoHitCollectionCnv_p1a.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// typedef to the latest persistent version
typedef NeutrinoHitCollection_p1a     NeutrinoHitCollection_PERS;
typedef NeutrinoHitCollectionCnv_p1a  NeutrinoHitCollectionCnv_PERS;

class NeutrinoHitCollectionCnv  : public T_AthenaPoolCustomCnv<NeutrinoHitCollection, NeutrinoHitCollection_PERS > {
  friend class CnvFactory<NeutrinoHitCollectionCnv>;
public:
  NeutrinoHitCollectionCnv(ISvcLocator* svcloc) :
        T_AthenaPoolCustomCnv<NeutrinoHitCollection, NeutrinoHitCollection_PERS >( svcloc) {}
protected:
  NeutrinoHitCollection_PERS*  createPersistent(NeutrinoHitCollection* transCont);
  NeutrinoHitCollection*       createTransient ();
};


#endif
