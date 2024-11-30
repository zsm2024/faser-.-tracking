/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_ClusterContainerCnv_p0.h"

#include "MsgUtil.h"

// Athena
#include "AthenaKernel/errorcheck.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/StatusCode.h"

#include <iostream>
#include <sstream>
#include <string>

//================================================================


FaserSCT_ClusterContainerCnv_p0::FaserSCT_ClusterContainerCnv_p0():
  m_sctId{nullptr},
  m_SCTDetEleCollKey{"SCT_DetectorElementCollection"}
{
}

StatusCode FaserSCT_ClusterContainerCnv_p0::initialize(MsgStream& log ) {

  ISvcLocator* svcLocator = Gaudi::svcLocator();

  // Get the messaging service, print where you are
  log << MSG::INFO << "FaserSCT_ClusterContainerCnv::initialize()" << endmsg;

  StoreGateSvc* detStore = nullptr;
  CHECK( svcLocator->service("DetectorStore", detStore) );
  CHECK( detStore->retrieve(m_sctId, "FaserSCT_ID") );
  CHECK( m_SCTDetEleCollKey.initialize() );
  MSG_DEBUG(log,"Converter initialized.");

  return StatusCode::SUCCESS;
}

Tracker::FaserSCT_ClusterContainer* FaserSCT_ClusterContainerCnv_p0::createTransient(FaserSCT_ClusterContainer_p0* persObj, MsgStream& log) {

  std::unique_ptr<Tracker::FaserSCT_ClusterContainer> trans(std::make_unique<Tracker::FaserSCT_ClusterContainer>(m_sctId->wafer_hash_max()) );
  //  MSG_DEBUG(log,"Read PRD vector, size " << persObj->size());

  SG::ReadCondHandle<TrackerDD::SiDetectorElementCollection> sctDetEleHandle(m_SCTDetEleCollKey);
  const TrackerDD::SiDetectorElementCollection* elements(*sctDetEleHandle);
  if (not sctDetEleHandle.isValid() or elements==nullptr) {
    log << MSG::FATAL << m_SCTDetEleCollKey.fullKey() << " is not available." << endmsg;
    return trans.release();
  }
  
  for (Tracker::FaserSCT_ClusterCollection* dcColl : *persObj) {
    // Add detElem to each drift circle
    IdentifierHash collHash = dcColl->identifyHash();
    const TrackerDD::SiDetectorElement * de = elements->getDetectorElement(collHash);
    //      MSG_DEBUG(log,"Set SCT_Cluster detector element to "<< de);

    Tracker::FaserSCT_ClusterCollection::iterator itColl   = dcColl->begin();
    Tracker::FaserSCT_ClusterCollection::iterator lastColl = dcColl->end();
    for (; itColl != lastColl; ++itColl) {
      (*itColl)->m_detEl = de;
    }

    StatusCode sc= trans->addCollection(dcColl, collHash);
    if (sc.isSuccess()){
      //         MSG_VERBOSE("SCT_ClusterContainer successfully added to Container !");
    } else {
      log << MSG::ERROR << "Failed to add FaserSCT_ClusterContainer to container" << endmsg;
      return nullptr;
    }
  }
  return trans.release();
}
