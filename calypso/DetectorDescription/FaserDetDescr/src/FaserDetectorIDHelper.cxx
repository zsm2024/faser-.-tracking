/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 
 -----------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include "FaserDetectorIDHelper.h"
#include "IdDict/IdDictDefs.h"  
#include "FaserDetDescr/FaserDetectorID.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

FaserDetectorIDHelper::FaserDetectorIDHelper(void)
	:
	m_isSLHC(false),
	m_emulsion_region_index(UNDEFINED),
	m_veto_region_index(UNDEFINED),
	m_trigger_region_index(UNDEFINED),
	m_preshower_region_index(UNDEFINED),
    m_sct_region_index(UNDEFINED),
    m_ecal_region_index(UNDEFINED),
	m_initialized(false),
    m_msgSvc(0)
{}

FaserDetectorIDHelper::~FaserDetectorIDHelper(void)
{
}

int         
FaserDetectorIDHelper::initialize_from_dictionary(const IdDictMgr& dict_mgr)
{

    if(m_initialized) return(0);
    m_initialized = true;

    FaserDetectorID faser_id;
    ExpandedIdentifier id;

    const IdDictDictionary* 	dict = dict_mgr.find_dictionary ("Neutrino"); 
    if(!dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "FaserDetectorIDHelper" );
            log << MSG::WARNING << "initialize_from_dictionary - cannot access Neutrino dictionary "
                << endmsg;
        }
        else {
            std::cout << " FaserDetectorIDHelper::initialize_from_dictionary - Warning: cannot access Neutrino dictionary "
                      << std::endl;
        }
    }
    else {
	// Check if this is SLHC layout
	// m_isSLHC = (dict->m_version=="SLHC");

	// Save index to a VETO region for unpacking
	id = faser_id.emulsion_exp(); 
	if (dict->find_region(id, m_emulsion_region_index)) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find emulsion region index: id, reg "  
                    << (std::string)id << " " << m_emulsion_region_index
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorIDHelper::initialize_from_dictionary - Warning: unable to find emulsion region index: id, reg "  
                          << (std::string)id << " " << m_emulsion_region_index
                          << std::endl;
            }
	}
    }

    dict = dict_mgr.find_dictionary ("Scintillator"); 
    if(!dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "FaserDetectorIDHelper" );
            log << MSG::WARNING << "initialize_from_dictionary - cannot access Scintillator dictionary "
                << endmsg;
        }
        else {
            std::cout << " FaserDetectorIDHelper::initialize_from_dictionary - Warning: cannot access Scintillator dictionary "
                      << std::endl;
        }
    }
    else {
	// Check if this is SLHC layout
	m_isSLHC = (dict->m_version=="SLHC");

	// Save index to a VETO region for unpacking
	id = faser_id.veto_exp(); 
	if (dict->find_region(id, m_veto_region_index)) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorIDHelper" );
                log << MSG::WARNING << "initialize_from_dictionary - unable to find veto region index: id, reg "  
                    << (std::string)id << " " << m_veto_region_index
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorIDHelper::initialize_from_dictionary - Warning: unable to find veto region index: id, reg "  
                          << (std::string)id << " " << m_veto_region_index
                          << std::endl;
            }
	}

	//if (!m_isSLHC) {
	    
	    // Save index to a TRIGGER region for unpacking
	    id = faser_id.trigger_exp();
	    if (dict->find_region(id, m_trigger_region_index)) {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorIDHelper" );
                    log << MSG::WARNING << "initialize_from_dictionary - unable to find trigger region index: id, reg "  
                        << (std::string)id << " " << m_trigger_region_index
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorIDHelper::initialize_from_dictionary - Warning: unable to trigger sct region index: id, reg "  
                              << (std::string)id << " " << m_trigger_region_index
                              << std::endl;
                }
	    }

	    // Save index to a PRESHOWER region for unpacking
	    id = faser_id.preshower_exp(); 
	    if (dict->find_region(id, m_preshower_region_index)) {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorIDHelper" );
                    log << MSG::WARNING << "initialize_from_dictionary - unable to find preshower region index: id, reg "  
                        << (std::string)id << " " << m_preshower_region_index
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorIDHelper::initialize_from_dictionary - Warning: unable to find preshower region index: id, reg "  
                              << (std::string)id << " " << m_preshower_region_index
                              << std::endl;
                }
            }
	//}
    }
    
    dict = dict_mgr.find_dictionary ("Tracker"); 
    if(!dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "FaserDetectorIDHelper" );
            log << MSG::WARNING << "initialize_from_dictionary - cannot access Tracker dictionary "
                << endmsg;
        }
        else {
            std::cout << " FaserDetectorIDHelper::initialize_from_dictionary - Warning: cannot access Tracker dictionary "
                      << std::endl;
        }
    }
    else {
        // Save index to a SCT region for unpacking
        id = faser_id.sct_exp(); 
        if (dict->find_region(id, m_sct_region_index)) {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorIDHelper" );
                    log << MSG::WARNING << "initialize_from_dictionary - unable to find sct region index: id, reg "  
                        << (std::string)id << " " << m_sct_region_index
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorIDHelper::initialize_from_dictionary - Warning: unable to find sct region index: id, reg "  
                            << (std::string)id << " " << m_sct_region_index
                            << std::endl;
                }
        }	
    }
    
    // Get Calorimetry dictionary for both LVL1 and Dead material
    dict = dict_mgr.find_dictionary ("Calorimeter"); 
    if(!dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "FaserDetectorIDHelper" );
            log << MSG::WARNING << "initialize_from_dictionary - cannot access Calorimeter dictionary "
                << endmsg;
        }
        else {
            std::cout << " FaserDetectorIDHelper::initialize_from_dictionary - Warning: cannot access Calorimeter dictionary "
                      << std::endl;
        }
    }
    else {
        // Save index to an ECAL region for unpacking
        id = faser_id.ecal_exp(); 
        if (dict->find_region(id, m_ecal_region_index)) {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorIDHelper" );
                    log << MSG::WARNING << "initialize_from_dictionary - unable to find ecal region index: id, reg "  
                        << (std::string)id << " " << m_ecal_region_index
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorIDHelper::initialize_from_dictionary - Warning: unable to find ecal region index: id, reg "  
                            << (std::string)id << " " << m_ecal_region_index
                            << std::endl;
                }
        }	
    }
        
    return (0);
}
