/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calo DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include "EcalIDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 

#include "IdDictDetDescr/IdDictManager.h"
#include "FaserCaloIdentifier/EcalID.h"


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
EcalIDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

//--------------------------------------------------------------------

StatusCode 
EcalIDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "EcalIDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }
    
    // The following is an attempt to "bootstrap" the loading of a
    // proxy for EcalID into the detector store. However,
    // EcalIDDetDescrCnv::initialize is NOT called by the conversion
    // service.  So for the moment, this cannot be use. Instead the
    // DetDescrCnvSvc must do the bootstrap from a parameter list.


//      // Add Calo_DetDescrManager proxy as entry point to the detector store
//      // - this is ONLY needed for the manager of each system
//      sc = addToDetStore(classID(), "EcalID");
//      if (sc.isFailure()) {
//  	log << MSG::FATAL << "Unable to add proxy for EcalID to the Detector Store!" << endmsg;
//  	return StatusCode::FAILURE;
//      } else {}

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
EcalIDDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "EcalIDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
EcalIDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), "EcalIDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating an EcalID helper object in the detector store" << endmsg;

    // Create a new EcalID

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

    // Internal Calo id tag
    std::string   caloIDTag      = mgr->tag();

    // DoChecks flag
    bool doChecks                 = mgr->do_checks();

    IdDictDictionary* dict = mgr->find_dictionary("Calorimeter");  
    if (!dict) {
	log << MSG::ERROR 
	    << "unable to find idDict for Calorimeter" 
	    << endmsg;
	return StatusCode::FAILURE;
    }

    // File to be read for Calo ids
    std::string   caloIDFileName = dict->file_name();

    // Tag of RDB record for Calo ids
    std::string   caloIdDictTag  = dict->dict_tag();


    if (m_ecalId) {

	// Ecal id helper already exists - second pass. Check for a
	// change 
	if (caloIDTag != m_caloIDTag) { 
	    // Internal Calo id tag
	    initHelper = true;
	    log << MSG::DEBUG << " Changed internal Calo id tag: " 
		<< caloIDTag << endmsg;
	}
	if (caloIDFileName != m_caloIDFileName) {
	    // File to be read for Calo ids
	    initHelper = true;
	    log << MSG::DEBUG << " Changed CaloFileName:" 
		<< caloIDFileName << endmsg;
	}
	if (caloIdDictTag != m_caloIdDictTag) {
	    // Tag of RDB record for Calo ids
	    initHelper = true;
	    log << MSG::DEBUG << " Changed CaloIdDictTag: "
		<< caloIdDictTag 
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
	m_ecalId = new EcalID;
	initHelper = true;
        // add in message service for printout
        m_ecalId->setMessageSvc(msgSvc());
    }
    
    if (initHelper) {
	if (idDictMgr->initializeHelper(*m_ecalId)) {
	    log << MSG::ERROR << "Unable to initialize EcalID" << endmsg;
	    return StatusCode::FAILURE;
	} 
	// Save state:
	m_caloIDTag      = caloIDTag;
	m_caloIDFileName = caloIDFileName;
	m_caloIdDictTag  = caloIdDictTag;
	m_doChecks        = doChecks;
    }

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = SG::asStorable(m_ecalId);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long
EcalIDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
EcalIDDetDescrCnv::classID() { 
    return ClassID_traits<EcalID>::ID(); 
}

//--------------------------------------------------------------------
EcalIDDetDescrCnv::EcalIDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<EcalID>::ID(), svcloc),
    m_ecalId(0),
    m_doChecks(false)

{}

