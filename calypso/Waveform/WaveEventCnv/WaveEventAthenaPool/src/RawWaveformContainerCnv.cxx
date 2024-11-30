/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#include "RawWaveformContainerCnv.h"

RawWaveformContainer_PERS* 
RawWaveformContainerCnv::createPersistent (RawWaveformContainer* transCont) {
  ATH_MSG_DEBUG("RawWaveformContainerCnv::createPersistent()");

  RawWaveformContainerCnv_PERS converter;

  RawWaveformContainer_PERS* persObj(nullptr);
  persObj = converter.createPersistent( transCont, msg() );
  return persObj;
}

RawWaveformContainer* 
RawWaveformContainerCnv::createTransient() {
  ATH_MSG_DEBUG("RawWaveformContainerCnv::createTransient()");

  static const pool::Guid p0_guid("344d904d-6338-41f1-94ee-ea609ea4ae44");
  RawWaveformContainer* trans(0);

  // Check for GUID of each persistent type
  if ( compareClassGuid(p0_guid) ) {
    std::unique_ptr< RawWaveformContainer_p0 > col_vect( poolReadObject< RawWaveformContainer_p0 >() );

    RawWaveformContainerCnv_p0 converter_p0;
    trans = converter_p0.createTransient( col_vect.get(), msg() );

  } else {

    // Didn't find a known type
    throw std::runtime_error("Unsupported persistent version of RawWaveformContainer");
  }

  return trans;

}


