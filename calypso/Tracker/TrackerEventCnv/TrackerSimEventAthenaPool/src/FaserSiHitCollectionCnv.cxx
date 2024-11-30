/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackerSimEventTPCnv/TrackerHits/FaserSiHitCollectionCnv_p3.h"
#include "FaserSiHitCollectionCnv.h"


FaserSiHitCollection_PERS* FaserSiHitCollectionCnv::createPersistent(FaserSiHitCollection* transCont) {
  MsgStream mlog(msgSvc(), "FaserSiHitCollectionConverter" );
  FaserSiHitCollectionCnv_PERS converter;
  FaserSiHitCollection_PERS *persObj = converter.createPersistent( transCont, mlog );
  return persObj;
}


FaserSiHitCollection* FaserSiHitCollectionCnv::createTransient() {
    MsgStream mlog(msgSvc(), "FaserSiHitCollectionConverter" );
    FaserSiHitCollectionCnv_p3   converter_p3;
    FaserSiHitCollectionCnv_p3a   converter_p3a;

    static const pool::Guid   p3_guid("FF9508DE-3E25-425D-9556-16D319DCE0E1");
    static const pool::Guid   p3a_guid("72FD9F51-AB1B-4DF7-B430-6CCAE0A994DB");

    FaserSiHitCollection       *trans_cont(0);
    if( this->compareClassGuid(p3a_guid)) 
    {
      std::unique_ptr< FaserSiHitCollection_p3a >   col_vect( this->poolReadObject< FaserSiHitCollection_p3a >() );
      trans_cont = converter_p3a.createTransient( col_vect.get(), mlog );
    }
    else if( this->compareClassGuid(p3_guid)) 
    {
      std::unique_ptr< FaserSiHitCollection_p3 >   col_vect( this->poolReadObject< FaserSiHitCollection_p3 >() );
      trans_cont = converter_p3.createTransient( col_vect.get(), mlog );
    }  
    else {
      throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}
