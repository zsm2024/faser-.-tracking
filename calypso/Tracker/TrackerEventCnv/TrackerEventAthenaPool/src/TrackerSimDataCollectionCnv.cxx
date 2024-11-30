/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackerSimDataCollectionCnv.h"


TrackerSimDataCollection_PERS* TrackerSimDataCollectionCnv::createPersistent(TrackerSimDataCollection* transCont) {
    MsgStream mlog(msgSvc(), "TrackerSimDataCollectionCnv" );
    TrackerSimDataCollectionCnv_PERS   converter;
    TrackerSimDataCollection_PERS *persObj = converter.createPersistent( transCont, mlog );
    return persObj;
}


TrackerSimDataCollection* TrackerSimDataCollectionCnv::createTransient() {
    MsgStream mlog(msgSvc(), "TrackerSimDataCollection" );
    // TrackerSimDataCollectionCnv_p1   converter_p1;
    // TrackerSimDataCollectionCnv_p2   converter_p2;
    TrackerSimDataCollectionCnv_p3   converter_p3;

    TrackerSimDataCollection       *trans_cont(0);
    static const pool::Guid   p3_guid("E25FFB08-56F4-4594-B005-45200A725CD4");
    // static const pool::Guid   p3_guid("1430AA7B-EE92-5A41-92F3-5DD5367D6BAA");
    // static const pool::Guid   p2_guid("C648CA66-013D-44AC-B0D9-99BFB0060E84");
    // static const pool::Guid   p1_guid("333EF996-1672-4AB8-917D-187F908F1EDE");
    // static const pool::Guid   old_guid("5A50C32E-C036-4A49-AE97-716D53210BE1");
    static const pool::Guid   old_guid("DF5775E8-8C33-4B82-8322-58B7671BF678");

    if( this->compareClassGuid(p3_guid)) {
        std::unique_ptr< TrackerSimDataCollection_p3 >   col_vect( this->poolReadObject< TrackerSimDataCollection_p3 >() );
        trans_cont = converter_p3.createTransient( col_vect.get(), mlog );
    }
    // else if( this->compareClassGuid(p2_guid)) {
    //     std::unique_ptr< TrackerSimDataCollection_p2 >   col_vect( this->poolReadObject< TrackerSimDataCollection_p2 >() );
    //     trans_cont = converter_p2.createTransient( col_vect.get(), mlog );
    // }
    // else if( this->compareClassGuid(p1_guid)) {
    //     std::unique_ptr< TrackerSimDataCollection_p1 >   col_vect( this->poolReadObject< TrackerSimDataCollection_p1 >() );
    //     trans_cont = converter_p1.createTransient( col_vect.get(), mlog );
    // }
    else if(  this->compareClassGuid(old_guid)) {
        // old version from before TP separation, just return it
        trans_cont = this->poolReadObject<TrackerSimDataCollection>();
    }  else {
        throw std::runtime_error("Unsupported persistent version of Data container");
    }
    return trans_cont;
}