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
    : FaserSCT_ClusterContainerCnvBase(svcloc, "FaserSCT_ClusterContainerCnv"),
      m_converter_p0(),
      m_storeGate(nullptr)
  {}


StatusCode FaserSCT_ClusterContainerCnv::initialize() {
   ATH_MSG_INFO("FaserSCT_ClusterContainerCnv::initialize()");

   StatusCode sc = FaserSCT_ClusterContainerCnvBase::initialize();
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
   
   if (m_converter_p0.initialize(msg()).isFailure())
   {
     ATH_MSG_FATAL("Could not initialize converter!");
     return StatusCode::FAILURE;
   }

   ATH_MSG_DEBUG("Converter initialized");

   return StatusCode::SUCCESS;
}


Tracker::FaserSCT_ClusterContainer* FaserSCT_ClusterContainerCnv::createTransient() {
  //  MsgStream log(msgSvc(), "SCT_ClusterContainerCnv" );
//   static const pool::Guid   p0_guid("586ED4E3-FA8F-4A9D-A919-9D42221975B8"); // before t/p split (was: A180F372-0D52-49C3-8AA0-0939CB0B8179)
  static const pool::Guid   p1_guid("5EF374F6-EA93-4739-9ECE-FBBA9C928818"); // with SCT_Cluster_tlp1 (was: 657F6546-F5CD-4166-9567-16AD9C96D286)
//   static const pool::Guid   p2_guid("ECE7D831-0F31-4E6F-A6BE-2ADDE90083BA"); // with SCT_Cluster_p2
  static const pool::Guid   p3_guid("42921F1E-8D65-4DBF-B309-6510794E05E0"); // with SCT_Cluster_p3 (was: 623F5836-369F-4A94-9DD4-DAD728E93C13)

  //ATH_MSG_DEBUG("createTransient(): main converter");
  Tracker::FaserSCT_ClusterContainer* p_collection(nullptr);
  if ( compareClassGuid(p3_guid) ) {
    //ATH_MSG_DEBUG("createTransient(): T/P version 3 detected");
    std::unique_ptr< FaserSCT_ClusterContainer_PERS >  p_coll( poolReadObject< FaserSCT_ClusterContainer_PERS >() );
    p_collection = m_TPConverter_p3.createTransient( p_coll.get(), msg() );
   
  } else if ( compareClassGuid(p1_guid) ) {
    //ATH_MSG_DEBUG("createTransient(): T/P version 1 detected");
    std::unique_ptr< Tracker::FaserSCT_ClusterContainer_tlp1 >  p_coll( poolReadObject< Tracker::FaserSCT_ClusterContainer_tlp1 >() );
    p_collection = m_TPConverter.createTransient( p_coll.get(), msg() );

//   } else if ( compareClassGuid(p2_guid) ) {
//     //ATH_MSG_DEBUG("createTransient(): T/P version 2 detected");
//     std::unique_ptr< InDet::SCT_ClusterContainer_p2 >  p_coll( poolReadObject< InDet::SCT_ClusterContainer_p2 >() );
//     p_collection = m_TPConverter_p2.createTransient( p_coll.get(), msg() );

//   } else if ( compareClassGuid(p0_guid) ) {
//     //ATH_MSG_DEBUG("createTransient(): Old input file");
//     std::unique_ptr< FaserSCT_ClusterContainer_p0 >  col_vect( poolReadObject< FaserSCT_ClusterContainer_p0 >() );
//     p_collection = m_converter_p0.createTransient( col_vect.get(), msg() );

  } else {
     throw std::runtime_error("Unsupported persistent version of FaserSCT_ClusterContainer");

  }
  return p_collection;
}


FaserSCT_ClusterContainer_PERS* FaserSCT_ClusterContainerCnv::createPersistent (Tracker::FaserSCT_ClusterContainer* transCont) {
   FaserSCT_ClusterContainer_PERS* sctdc_p= m_TPConverter_p3.createPersistent( transCont, msg() );
   return sctdc_p;
}
