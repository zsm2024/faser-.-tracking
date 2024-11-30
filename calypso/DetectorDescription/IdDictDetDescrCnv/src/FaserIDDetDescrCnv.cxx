/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include "FaserIDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 
#include "AthenaKernel/StorableConversions.h"

#include "IdDictDetDescr/IdDictManager.h"
#include "FaserDetDescr/FaserDetectorID.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

//--------------------------------------------------------------------

long int   
FaserIDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

//--------------------------------------------------------------------

StatusCode 
FaserIDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "FaserIDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }

    // The following is an attempt to "bootstrap" the loading of a
    // proxy for AtlasDetectorID into the detector store. However,
    // AtlasIDDetDescrCnv::initialize is NOT called by the conversion
    // service.  So for the moment, this cannot be use. Instead the
    // DetDescrCnvSvc must do the bootstrap from a parameter list.


//      // Add InDet_DetDescrManager proxy as entry point to the detector store
//      // - this is ONLY needed for the manager of each system
//      sc = addToDetStore(classID(), "PidelID");
//      if (sc.isFailure()) {
//  	log << MSG::FATAL << "Unable to add proxy for AtlasDetectorID to the Detector Store!" << endmsg;
//  	return StatusCode::FAILURE;
//      } else {}

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
FaserIDDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "FaserIDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
FaserIDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), "FaserIDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a FaserDetectorID helper object in the detector store" << endmsg;

    // Create a new FaserDetectorID

    DetDescrAddress* ddAddr;
    ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
    if(!ddAddr) {
        log << MSG::FATAL << "Could not cast to DetDescrAddress." << endmsg;
        return StatusCode::FAILURE;
    }

    // Get the StoreGate key of this container.
    std::string helperKey  = *( ddAddr->par() );
    if ("" == helperKey) {
        log << MSG::DEBUG << "No Helper key " << endmsg;
    }
    else {
        log << MSG::DEBUG << "Helper key is " << helperKey << endmsg;
    }
    

    // get DetectorStore service
    StoreGateSvc * detStore;
    StatusCode status = serviceLocator()->service("DetectorStore", detStore);
    if (status.isFailure()) {
        log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
        return StatusCode::FAILURE;
    } else {}
 
    // Get the dictionary manager from the detector store
    const IdDictManager* idDictMgr;
    status = detStore->retrieve(idDictMgr, "IdDict");
    if (status.isFailure()) {
        log << MSG::FATAL << "Could not get IdDictManager !" << endmsg;
        return StatusCode::FAILURE;
    } 
    else {
        log << MSG::DEBUG << " Found the IdDictManager. " << endmsg;
    }


    // create the helper
    auto faser_id = std::make_unique<FaserDetectorID>();
    faser_id->setMessageSvc(msgSvc());

    if (idDictMgr->initializeHelper(*faser_id)) {
        log << MSG::ERROR << "Unable to initialize FaserDetectorID" << endmsg;
        return StatusCode::FAILURE;
    } 

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = SG::asStorable(std::move(faser_id));

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long int
FaserIDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
FaserIDDetDescrCnv::classID() { 
    return ClassID_traits<FaserDetectorID>::ID(); 
}

//--------------------------------------------------------------------
FaserIDDetDescrCnv::FaserIDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<FaserDetectorID>::ID(), svcloc)
{}



