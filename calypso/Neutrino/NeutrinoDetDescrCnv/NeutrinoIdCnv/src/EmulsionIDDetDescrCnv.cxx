/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Neutrino DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include "EmulsionIDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 

#include "IdDictDetDescr/IdDictManager.h"
#include "NeutrinoIdentifier/EmulsionID.h"


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
EmulsionIDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

//--------------------------------------------------------------------

StatusCode 
EmulsionIDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "EmulsionIDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }
    
    // The following is an attempt to "bootstrap" the loading of a
    // proxy for EmulsionID into the detector store. However,
    // EmulsionIDDetDescrCnv::initialize is NOT called by the conversion
    // service.  So for the moment, this cannot be use. Instead the
    // DetDescrCnvSvc must do the bootstrap from a parameter list.


//      // Add Neutrino_DetDescrManager proxy as entry point to the detector store
//      // - this is ONLY needed for the manager of each system
//      sc = addToDetStore(classID(), "EmulsionID");
//      if (sc.isFailure()) {
//  	log << MSG::FATAL << "Unable to add proxy for EmulsionID to the Detector Store!" << endmsg;
//  	return StatusCode::FAILURE;
//      } else {}

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
EmulsionIDDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "EmulsionIDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
EmulsionIDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), "EmulsionIDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a EmulsionID helper object in the detector store" << endmsg;

    // Create a new EmulsionID

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

    // Internal Neutrino id tag
    std::string   neutrinoIDTag      = mgr->tag();

    // DoChecks flag
    bool doChecks                 = mgr->do_checks();

    IdDictDictionary* dict = mgr->find_dictionary("Neutrino");  
    if (!dict) {
	log << MSG::ERROR 
	    << "unable to find idDict for Neutrino" 
	    << endmsg;
	return StatusCode::FAILURE;
    }

    // File to be read for Neutrino ids
    std::string   neutrinoIDFileName = dict->file_name();

    // Tag of RDB record for Neutrino ids
    std::string   neutrinoIdDictTag  = dict->dict_tag();


    if (m_emulsionId) {

	// Emulsion id helper already exists - second pass. Check for a
	// change 
	if (neutrinoIDTag != m_neutrinoIDTag) { 
	    // Internal Neutrino id tag
	    initHelper = true;
	    log << MSG::DEBUG << " Changed internal Neutrino id tag: " 
		<< neutrinoIDTag << endmsg;
	}
	if (neutrinoIDFileName != m_neutrinoIDFileName) {
	    // File to be read for Neutrino ids
	    initHelper = true;
	    log << MSG::DEBUG << " Changed NeutrinoFileName:" 
		<< neutrinoIDFileName << endmsg;
	}
	if (neutrinoIdDictTag != m_neutrinoIdDictTag) {
	    // Tag of RDB record for Neutrino ids
	    initHelper = true;
	    log << MSG::DEBUG << " Changed NeutrinoIdDictTag: "
		<< neutrinoIdDictTag 
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
	m_emulsionId = new EmulsionID;
	initHelper = true;
        // add in message service for printout
        m_emulsionId->setMessageSvc(msgSvc());
    }
    
    if (initHelper) {
	if (idDictMgr->initializeHelper(*m_emulsionId)) {
	    log << MSG::ERROR << "Unable to initialize EmulsionID" << endmsg;
	    return StatusCode::FAILURE;
	} 
	// Save state:
	m_neutrinoIDTag      = neutrinoIDTag;
	m_neutrinoIDFileName = neutrinoIDFileName;
	m_neutrinoIdDictTag  = neutrinoIdDictTag;
	m_doChecks        = doChecks;
    }

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = SG::asStorable(m_emulsionId);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long
EmulsionIDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
EmulsionIDDetDescrCnv::classID() { 
    return ClassID_traits<EmulsionID>::ID(); 
}

//--------------------------------------------------------------------
EmulsionIDDetDescrCnv::EmulsionIDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<EmulsionID>::ID(), svcloc),
    m_emulsionId(0),
    m_doChecks(false)

{}

