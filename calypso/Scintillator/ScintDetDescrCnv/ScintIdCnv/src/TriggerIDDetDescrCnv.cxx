/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Scint DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include "TriggerIDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 

#include "IdDictDetDescr/IdDictManager.h"
#include "ScintIdentifier/TriggerID.h"


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
TriggerIDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

//--------------------------------------------------------------------

StatusCode 
TriggerIDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "TriggerIDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }
    
    // The following is an attempt to "bootstrap" the loading of a
    // proxy for TriggerID into the detector store. However,
    // TriggerIDDetDescrCnv::initialize is NOT called by the conversion
    // service.  So for the moment, this cannot be use. Instead the
    // DetDescrCnvSvc must do the bootstrap from a parameter list.


//      // Add Scint_DetDescrManager proxy as entry point to the detector store
//      // - this is ONLY needed for the manager of each system
//      sc = addToDetStore(classID(), "TriggerID");
//      if (sc.isFailure()) {
//  	log << MSG::FATAL << "Unable to add proxy for TriggerID to the Detector Store!" << endmsg;
//  	return StatusCode::FAILURE;
//      } else {}

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
TriggerIDDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "TriggerIDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
TriggerIDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), "TriggerIDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a TriggerID helper object in the detector store" << endmsg;

    // Create a new TriggerID

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

    // Only create new helper if it is the first pass or if there is a
    // change in the the file or tag
    bool initHelper               = false;

    const IdDictMgr* mgr          = idDictMgr->manager();

    // Internal Scint id tag
    std::string   scintIDTag      = mgr->tag();

    // DoChecks flag
    bool doChecks                 = mgr->do_checks();

    IdDictDictionary* dict = mgr->find_dictionary("Scintillator");  
    if (!dict) {
	log << MSG::ERROR 
	    << "unable to find idDict for Scintillator" 
	    << endmsg;
	return StatusCode::FAILURE;
    }

    // File to be read for Scint ids
    std::string   scintIDFileName = dict->file_name();

    // Tag of RDB record for Scint ids
    std::string   scintIdDictTag  = dict->dict_tag();


    if (m_triggerId) {

	// Trigger id helper already exists - second pass. Check for a
	// change 
	if (scintIDTag != m_scintIDTag) { 
	    // Internal Scint id tag
	    initHelper = true;
	    log << MSG::DEBUG << " Changed internal Scint id tag: " 
		<< scintIDTag << endmsg;
	}
	if (scintIDFileName != m_scintIDFileName) {
	    // File to be read for Scint ids
	    initHelper = true;
	    log << MSG::DEBUG << " Changed ScintFileName:" 
		<< scintIDFileName << endmsg;
	}
	if (scintIdDictTag != m_scintIdDictTag) {
	    // Tag of RDB record for Scint ids
	    initHelper = true;
	    log << MSG::DEBUG << " Changed ScintIdDictTag: "
		<< scintIdDictTag 
		<< endmsg;
	}
	if (doChecks != m_doChecks) {
	    // DoChecks flag
	    initHelper = true;
	    log << MSG::DEBUG << " Changed doChecks flag: "
		<< doChecks
		<< endmsg;
        }
    }
    else {
	// create the helper
	m_triggerId = new TriggerID;
	initHelper = true;
        // add in message service for printout
        m_triggerId->setMessageSvc(msgSvc());
    }
    
    if (initHelper) {
	if (idDictMgr->initializeHelper(*m_triggerId)) {
	    log << MSG::ERROR << "Unable to initialize TriggerID" << endmsg;
	    return StatusCode::FAILURE;
	} 
	// Save state:
	m_scintIDTag      = scintIDTag;
	m_scintIDFileName = scintIDFileName;
	m_scintIdDictTag  = scintIdDictTag;
	m_doChecks        = doChecks;
    }

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = SG::asStorable(m_triggerId);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long
TriggerIDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
TriggerIDDetDescrCnv::classID() { 
    return ClassID_traits<TriggerID>::ID(); 
}

//--------------------------------------------------------------------
TriggerIDDetDescrCnv::TriggerIDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<TriggerID>::ID(), svcloc),
    m_triggerId(0),
    m_doChecks(false)

{}

