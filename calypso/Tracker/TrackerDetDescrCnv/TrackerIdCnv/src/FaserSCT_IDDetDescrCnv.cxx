/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tracker DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: FaserSCT_IDDetDescrCnv.cxx,v 1.4 2007-01-16 17:06:15 dquarrie Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "FaserSCT_IDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 

#include "IdDictDetDescr/IdDictManager.h"
#include "TrackerIdentifier/FaserSCT_ID.h"


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
FaserSCT_IDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

//--------------------------------------------------------------------

StatusCode 
FaserSCT_IDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "FaserSCT_IDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }
    
    // The following is an attempt to "bootstrap" the loading of a
    // proxy for FaserSCT_ID into the detector store. However,
    // FaserSCT_IDDetDescrCnv::initialize is NOT called by the conversion
    // service.  So for the moment, this cannot be use. Instead the
    // DetDescrCnvSvc must do the bootstrap from a parameter list.


//      // Add Tracker_DetDescrManager proxy as entry point to the detector store
//      // - this is ONLY needed for the manager of each system
//      sc = addToDetStore(classID(), "PidelID");
//      if (sc.isFailure()) {
//  	log << MSG::FATAL << "Unable to add proxy for FaserSCT_ID to the Detector Store!" << endmsg;
//  	return StatusCode::FAILURE;
//      } else {}

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
FaserSCT_IDDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "FaserSCT_IDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
FaserSCT_IDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), "FaserSCT_IDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a FaserSCT_ID helper object in the detector store" << endmsg;

    // Create a new FaserSCT_ID

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

    // Internal Tracker id tag
    std::string   trackerIDTag      = mgr->tag();

    // DoChecks flag
    bool doChecks                 = mgr->do_checks();

    IdDictDictionary* dict = mgr->find_dictionary("Tracker");  
    if (!dict) {
	log << MSG::ERROR 
	    << "unable to find idDict for Tracker" 
	    << endmsg;
	return StatusCode::FAILURE;
    }

    // File to be read for Tracker ids
    std::string   trackerIDFileName = dict->file_name();

    // Tag of RDB record for Tracker ids
    std::string   trackerIdDictTag  = dict->dict_tag();


    if (m_sctId) {

	// SCT id helper already exists - second pass. Check for a
	// change 
	if (trackerIDTag != m_trackerIDTag) { 
	    // Internal Tracker id tag
	    initHelper = true;
	    log << MSG::DEBUG << " Changed internal Tracker id tag: " 
		<< trackerIDTag << endmsg;
	}
	if (trackerIDFileName != m_trackerIDFileName) {
	    // File to be read for Tracker ids
	    initHelper = true;
	    log << MSG::DEBUG << " Changed TrackerFileName:" 
		<< trackerIDFileName << endmsg;
	}
	if (trackerIdDictTag != m_trackerIdDictTag) {
	    // Tag of RDB record for Tracker ids
	    initHelper = true;
	    log << MSG::DEBUG << " Changed TrackerIdDictTag: "
		<< trackerIdDictTag 
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
	m_sctId = new FaserSCT_ID;
	initHelper = true;
        // add in message service for printout
        m_sctId->setMessageSvc(msgSvc());
    }
    
    if (initHelper) {
	if (idDictMgr->initializeHelper(*m_sctId)) {
	    log << MSG::ERROR << "Unable to initialize FaserSCT_ID" << endmsg;
	    return StatusCode::FAILURE;
	} 
	// Save state:
	m_trackerIDTag      = trackerIDTag;
	m_trackerIDFileName = trackerIDFileName;
	m_trackerIdDictTag  = trackerIdDictTag;
	m_doChecks        = doChecks;
    }

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = SG::asStorable(m_sctId);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long
FaserSCT_IDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
FaserSCT_IDDetDescrCnv::classID() { 
    return ClassID_traits<FaserSCT_ID>::ID(); 
}

//--------------------------------------------------------------------
FaserSCT_IDDetDescrCnv::FaserSCT_IDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<FaserSCT_ID>::ID(), svcloc),
    m_sctId(0),
    m_doChecks(false)

{}



