/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserCaloSimEventTPCnv/CaloHits/CaloHitCollectionCnv_p1.h"
#include "FaserCaloSimEventTPCnv/CaloHits/CaloHit_p1.h"
#include "CaloHitCollectionCnv.h"


CaloHitCollection_PERS* CaloHitCollectionCnv::createPersistent(CaloHitCollection* transCont) {
  MsgStream mlog(msgSvc(), "CaloHitCollectionConverter" );
  CaloHitCollectionCnv_PERS converter;
  CaloHitCollection_PERS *persObj = converter.createPersistent( transCont, mlog );
  return persObj;
}


CaloHitCollection* CaloHitCollectionCnv::createTransient() {
    MsgStream mlog(msgSvc(), "CaloHitCollectionConverter" );
    CaloHitCollectionCnv_p1   converter_p1;
    CaloHitCollectionCnv_p1a   converter_p1a;

    static const pool::Guid   p1_guid("134E8045-AB99-43EF-9AD1-324C48830B64");
    static const pool::Guid   p1a_guid("02C108EE-0AD9-4444-83BD-D5273FCDEF6F");

    CaloHitCollection       *trans_cont(0);
    if( this->compareClassGuid(p1a_guid)) {
      std::unique_ptr< CaloHitCollection_p1a >   col_vect( this->poolReadObject< CaloHitCollection_p1a >() );
      trans_cont = converter_p1a.createTransient( col_vect.get(), mlog );
    } else if( this->compareClassGuid(p1_guid)) {
      std::unique_ptr< CaloHitCollection_p1 >   col_vect( this->poolReadObject< CaloHitCollection_p1 >() );
      trans_cont = converter_p1.createTransient( col_vect.get(), mlog );
    }  else {
      throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}
