/*
   Copyright (C) 2021 CERN for the benefit of the FASER collaboration
   */

#include "FaserSCT_SpacePointContainerCnv.h"

/*
StatusCode FaserSCT_SpacePointContainerCnv::initialize() {
  ATH_MSG_INFO("FaserSCT_SpacePointContainerCnv::initialize()");

  StatusCode sc = FaserSCT_SpacePointContainerCnvBase::initialize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Cannot initialize cnv base !");
    return StatusCode::FAILURE;
  }

  // get StoreGate service. This is needed only for clients 
  // that register collections directly to the SG instead of adding 
  // them to the container.
  sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("StoreGate service not found !");
    return StatusCode::FAILURE;
  }

  // get DetectorStore service
  StoreGateSvc* detStore(nullptr);
  if (service("DetectorStore", detStore).isFailure()) {
    ATH_MSG_FATAL("DetectorStore service not found !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Found DetectorStore.");
  }

  // Get the SCT helper from the detector store
  const FaserSCT_ID* idhelper(nullptr);
  if (detStore->retrieve(idhelper, "FaserSCT_ID").isFailure()) {
    ATH_MSG_FATAL("Could not get FaserSCT_ID helper !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Found the FaserSCT_ID helper.");
  }
  ATH_MSG_DEBUG("Converter initialized");

  return StatusCode::SUCCESS;
}
*/

FaserSCT_SpacePointContainer_PERS*
FaserSCT_SpacePointContainerCnv::createPersistent (FaserSCT_SpacePointContainer* transObj) {
  ATH_MSG_DEBUG("FaserSCT_SpacePointContainerCnv::createPersistent()");

  FaserSCT_SpacePointContainerCnv_PERS converter;

  FaserSCT_SpacePointContainer_PERS* persObj(nullptr);
  persObj = converter.createPersistent(transObj, msg());
  return persObj;
}


FaserSCT_SpacePointContainer*
FaserSCT_SpacePointContainerCnv::createTransient() {
  ATH_MSG_DEBUG("FaserSCT_SpacePointContainerCnv::createTransient()");

  static const pool::Guid p0_guid("DB0397F9-A163-496F-BC17-C7E507A1FA50");
  FaserSCT_SpacePointContainer* transObj(nullptr);

  if (compareClassGuid(p0_guid)) {
    std::unique_ptr<FaserSCT_SpacePointContainer_PERS> col_vect(poolReadObject<FaserSCT_SpacePointContainer_PERS>());
    FaserSCT_SpacePointContainerCnv_PERS converter;
    transObj = converter.createTransient( col_vect.get(), msg());
  } else {
    throw std::runtime_error("Unsupported persistent version of FaserSCT_SpacePointContainer");
  }

  return transObj;
}
