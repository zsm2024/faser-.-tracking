/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ScintSimEventTPCnv/ScintHits/ScintHitCollectionCnv_p1.h"
#include "ScintSimEventTPCnv/ScintHits/ScintHitCollectionCnv_p1a.h"
#include "ScintSimEventTPCnv/ScintHits/ScintHit_p1.h"
#include "ScintHitCollectionCnv.h"


ScintHitCollection_PERS* ScintHitCollectionCnv::createPersistent(ScintHitCollection* transCont) {
  MsgStream mlog(msgSvc(), "ScintHitCollectionConverter" );
  ScintHitCollectionCnv_PERS converter;
  ScintHitCollection_PERS *persObj = converter.createPersistent( transCont, mlog );
  return persObj;
}


ScintHitCollection* ScintHitCollectionCnv::createTransient() {
    MsgStream mlog(msgSvc(), "ScintHitCollectionConverter" );
    ScintHitCollectionCnv_p1   converter_p1;
    ScintHitCollectionCnv_p1a  converter_p1a;

    static const pool::Guid   p1_guid("B2573A16-4B46-4E1E-98E3-F93421680779");
    static const pool::Guid   p1a_guid("0DFC461F-9FF5-4447-B4F0-7CC7157191D1");

    ScintHitCollection       *trans_cont(0);
    if( this->compareClassGuid(p1a_guid)) 
    {
      std::unique_ptr< ScintHitCollection_p1a >   col_vect( this->poolReadObject< ScintHitCollection_p1a >() );
      trans_cont = converter_p1a.createTransient( col_vect.get(), mlog );
    }
    else if( this->compareClassGuid(p1_guid)) 
    {
      std::unique_ptr< ScintHitCollection_p1 >   col_vect( this->poolReadObject< ScintHitCollection_p1 >() );
      trans_cont = converter_p1.createTransient( col_vect.get(), mlog );
    }  else {
      throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}
