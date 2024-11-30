/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */


#include "NeutrinoGeoModelUtils/DistortedMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"

namespace NeutrinoDD {
  DistortedMaterialManager::DistortedMaterialManager() {
    ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap

    MsgStream log(Athena::getMessageSvc(), "ExtraMaterialManager");
    log << MSG::DEBUG << "Initialized Neutrino Distorted Material Manager" << endmsg;

    StoreGateSvc* detStore;
    StatusCode sc;
    sc = svcLocator->service("DetectorStore", detStore);
    if (sc.isFailure()) log << MSG::FATAL << "Could not locate DetectorStore" << endmsg;

    IRDBAccessSvc* rdbSvc;
    sc = svcLocator->service("RDBAccessSvc", rdbSvc);
    if (sc.isFailure()) log << MSG::FATAL << "Could not locate RDBAccessSvc" << endmsg;

    // Get version tag and node for InDet.
    DecodeFaserVersionKey versionKey("Neutrino");
    std::string detectorKey = versionKey.tag();
    std::string detectorNode = versionKey.node();

    log << MSG::DEBUG << "Retrieving Record Sets from database ..." << endmsg;
    log << MSG::DEBUG << "Key = " << detectorKey << " Node = " << detectorNode << endmsg;

    m_xMatTable = rdbSvc->getRecordsetPtr("NeutrinoExtraMaterial", detectorKey, detectorNode, "FASERDD");

    const StoredMaterialManager* theGeoMaterialManager = 0;
    sc = detStore->retrieve(theGeoMaterialManager, "MATERIALS");
    if (sc.isFailure()) log << MSG::FATAL << "Could not locate GeoModel Material manager" << endmsg;
    m_materialManager = theGeoMaterialManager;
  }
} // end namespace
