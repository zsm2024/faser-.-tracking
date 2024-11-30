/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_ClusterContainerCnv.h"

#include "MsgUtil.h"

#include "TrackerIdentifier/FaserSCT_ID.h"
#include "StoreGate/StoreGateSvc.h"

#include <iostream>
#include <memory>

  FaserSCT_ClusterContainerCnv::FaserSCT_ClusterContainerCnv (ISvcLocator* svcloc)
    : SCT_ClusterContainerCnvBase(svcloc, "FaserSCT_ClusterContainerCnv"),
      m_converter_p3(),
      m_storeGate(nullptr)
  {}


StatusCode FaserSCT_ClusterContainerCnv::initialize() {
   ATH_MSG_INFO("FaserSCT_ClusterContainerCnv::initialize()");

   StatusCode sc = SCT_ClusterContainerCnvBase::initialize();
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
   
   if (m_converter_p3.initialize(msg()).isFailure())
   {
     ATH_MSG_FATAL("Could not initialize converter!");
     return StatusCode::FAILURE;
   }

   ATH_MSG_DEBUG("Converter initialized");

   return StatusCode::SUCCESS;
}


FaserSCT_ClusterContainer* FaserSCT_ClusterContainerCnv::createTransient() {
  //  MsgStream log(msgSvc(), "FaserSCT_ClusterContainerCnv" );
  static const pool::Guid   p3_guid("84C2A724-5B0C-417A-BFE2-0A59CE66f17D"); // before t/p split

  //ATH_MSG_DEBUG("createTransient(): main converter");
  FaserSCT_ClusterContainer* p_collection(nullptr);
  if ( compareClassGuid(p3_guid) ) {
    //ATH_MSG_DEBUG("createTransient(): T/P version 3 detected");
    std::unique_ptr< SCT_ClusterContainer_PERS >  p_coll( poolReadObject< SCT_ClusterContainer_PERS >() );
    p_collection = m_converter_p3.createTransient( p_coll.get(), msg() );

  } else {
     throw std::runtime_error("Unsupported persistent version of SCT_ClusterContainer");

  }
  return p_collection;
}


SCT_ClusterContainer_PERS* FaserSCT_ClusterContainerCnv::createPersistent (FaserSCT_ClusterContainer* transCont) {
   SCT_ClusterContainer_PERS* sctdc_p;//= m_converter_p3.createPersistent( transCont, msg() );
   return sctdc_p;
}
