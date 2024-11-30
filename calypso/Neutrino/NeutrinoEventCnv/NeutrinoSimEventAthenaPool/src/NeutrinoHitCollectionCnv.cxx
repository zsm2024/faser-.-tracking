/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "NeutrinoSimEventTPCnv/NeutrinoHits/NeutrinoHitCollectionCnv_p1.h"
#include "NeutrinoSimEventTPCnv/NeutrinoHits/NeutrinoHit_p1.h"
#include "NeutrinoSimEventTPCnv/NeutrinoHits/NeutrinoHitCollectionCnv_p1a.h"
#include "NeutrinoHitCollectionCnv.h"


NeutrinoHitCollection_PERS* NeutrinoHitCollectionCnv::createPersistent(NeutrinoHitCollection* transCont) {
  MsgStream mlog(msgSvc(), "NeutrinoHitCollectionConverter" );
  NeutrinoHitCollectionCnv_PERS converter;
  NeutrinoHitCollection_PERS *persObj = converter.createPersistent( transCont, mlog );
  return persObj;
}


NeutrinoHitCollection* NeutrinoHitCollectionCnv::createTransient() {
    MsgStream mlog(msgSvc(), "NeutrinoHitCollectionConverter" );
    NeutrinoHitCollectionCnv_p1   converter_p1;
    NeutrinoHitCollectionCnv_p1a  converter_p1a;

    static const pool::Guid   p1_guid("2CA7AF71-1494-4378-BED4-AFB5C54398AA");
    static const pool::Guid   p1a_guid("0A3CD37D-64CE-405D-98CF-595D678B14B7");

    NeutrinoHitCollection       *trans_cont(0);
    if( this->compareClassGuid(p1a_guid)) {
      std::unique_ptr< NeutrinoHitCollection_p1a >   col_vect( this->poolReadObject< NeutrinoHitCollection_p1a >() );
      trans_cont = converter_p1a.createTransient( col_vect.get(), mlog );
    } else if( this->compareClassGuid(p1_guid)) {
      std::unique_ptr< NeutrinoHitCollection_p1 >   col_vect( this->poolReadObject< NeutrinoHitCollection_p1 >() );
      trans_cont = converter_p1.createTransient( col_vect.get(), mlog );
    }  else {
      throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}
