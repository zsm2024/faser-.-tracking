/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Detector Description
 -----------------------------------------
***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/MsgStream.h"
#include "FaserDetDescr/FaserDetectorID.h"
#include "IdDict/IdDictDefs.h"
#include "FaserDetectorIDHelper.h"
#include <iostream>
#include <stdio.h>
#include <assert.h>

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

FaserDetectorID::FaserDetectorID()
        :
        m_do_checks(false),
        m_do_neighbours(true),
        m_msgSvc(0),
        m_quiet(false),
        m_is_initialized_from_dict(false),
        m_DET_INDEX(999),
        m_SUBDET_INDEX(999),
        m_NEUTRINO_ID(1),
        m_SCINT_ID(2),
        m_TRACKER_ID(3),
        m_CALO_ID(4),
        m_EMULSION_ID(1),
        m_VETO_ID(1),
    	m_VETONU_ID(4),
        m_TRIGGER_ID(2),
        m_PRESHOWER_ID(3),
        m_SCT_ID(1),
        m_ECAL_ID(1),
        m_isSLHC(false),
        m_faser_dict(0),
        m_neutrino_dict(0),
        m_scint_dict(0),
        m_tracker_dict(0),
        m_calo_dict(0),
        m_helper(0)
{}

FaserDetectorID::~FaserDetectorID(void)
{
    delete m_helper;
}

FaserDetectorID::FaserDetectorID(const FaserDetectorID& other)
        :
        IdHelper(),
        m_do_checks               (other.m_do_checks),
        m_do_neighbours           (other.m_do_neighbours),
        m_msgSvc                  (other.m_msgSvc),
        m_quiet                   (other.m_quiet),
        m_dict_version            (other.m_dict_version),
        m_is_initialized_from_dict(other.m_is_initialized_from_dict),
        m_DET_INDEX               (other.m_DET_INDEX),
        m_SUBDET_INDEX            (other.m_SUBDET_INDEX),
        m_NEUTRINO_ID             (other.m_NEUTRINO_ID),
        m_SCINT_ID                (other.m_SCINT_ID),
        m_TRACKER_ID              (other.m_TRACKER_ID),
        m_CALO_ID                 (other.m_CALO_ID),
        m_EMULSION_ID             (other.m_EMULSION_ID),
        m_VETO_ID                 (other.m_VETO_ID),
        m_VETONU_ID               (other.m_VETONU_ID),
        m_TRIGGER_ID              (other.m_TRIGGER_ID),
        m_PRESHOWER_ID            (other.m_PRESHOWER_ID),
        m_SCT_ID                  (other.m_SCT_ID),
        m_ECAL_ID                 (other.m_ECAL_ID),
        m_isSLHC                  (other.m_isSLHC),
        m_faser_dict              (other.m_faser_dict),
        m_neutrino_dict           (other.m_neutrino_dict),
        m_scint_dict              (other.m_scint_dict),
        m_tracker_dict            (other.m_tracker_dict),
        m_calo_dict               (other.m_calo_dict),
        m_helper                  (0),
        m_det_impl                (other.m_det_impl),
        m_neutrino_part_impl      (other.m_neutrino_part_impl),
        m_scint_part_impl         (other.m_scint_part_impl),
        m_tracker_part_impl       (other.m_tracker_part_impl),
        m_calo_part_impl          (other.m_calo_part_impl)

{
    if (other.m_helper) {
        // Must copy helper
        m_helper = new  FaserDetectorIDHelper(*other.m_helper);
        m_helper->setMsgSvc(m_msgSvc);
    }
}

FaserDetectorID&
FaserDetectorID::operator= (const FaserDetectorID& other)
{
    if (this != &other) {
        m_do_checks             = other.m_do_checks;
        m_do_neighbours         = other.m_do_neighbours;
        m_msgSvc                = other.m_msgSvc;
        m_quiet                 = other.m_quiet;
        m_dict_version          = other.m_dict_version;
        m_is_initialized_from_dict = other.m_is_initialized_from_dict;
        m_DET_INDEX             = other.m_DET_INDEX;
        m_SUBDET_INDEX          = other.m_SUBDET_INDEX;
        m_NEUTRINO_ID           = other.m_NEUTRINO_ID;
        m_SCINT_ID              = other.m_SCINT_ID;
        m_TRACKER_ID            = other.m_TRACKER_ID;
        m_CALO_ID               = other.m_CALO_ID;
        m_EMULSION_ID           = other.m_EMULSION_ID;
        m_VETO_ID               = other.m_VETO_ID;
        m_TRIGGER_ID            = other.m_TRIGGER_ID;
        m_PRESHOWER_ID          = other.m_PRESHOWER_ID;
        m_VETONU_ID             = other.m_VETONU_ID;
        m_SCT_ID                = other.m_SCT_ID;
        m_ECAL_ID               = other.m_ECAL_ID;
        m_faser_dict            = other.m_faser_dict;
        m_neutrino_dict         = other.m_neutrino_dict;
        m_scint_dict            = other.m_scint_dict;
        m_tracker_dict          = other.m_tracker_dict;
        m_calo_dict             = other.m_calo_dict;
        m_det_impl              = other.m_det_impl;
        m_neutrino_part_impl    = other.m_neutrino_part_impl;
        m_scint_part_impl       = other.m_scint_part_impl;
        m_tracker_part_impl     = other.m_tracker_part_impl;
        m_calo_part_impl        = other.m_calo_part_impl;

        if (other.m_helper) {
            // Must copy helper.
            delete m_helper;
            m_helper = new  FaserDetectorIDHelper(*other.m_helper);
            m_helper->setMsgSvc(m_msgSvc);
        }
    }

    return (*this);
}

Identifier
FaserDetectorID::neutrino     (void) const
{

    Identifier result((Identifier::value_type)0);
    // Pack field
    m_det_impl.pack    (neutrino_field_value(), result);
    return (result);
}

Identifier
FaserDetectorID::scint        (void) const
{

    Identifier result((Identifier::value_type)0);
    // Pack field
    m_det_impl.pack    (scint_field_value(), result);
    return (result);
}

Identifier
FaserDetectorID::tracker      (void) const
{
    Identifier result((Identifier::value_type)0);
    // Pack field
    m_det_impl.pack    (tracker_field_value(), result);
    return (result);
}

Identifier
FaserDetectorID::calo(void) const
{
    Identifier result((Identifier::value_type)0);
    // Pack field
    m_det_impl.pack    (calo_field_value(), result);
    return (result);
}

Identifier
FaserDetectorID::emulsion        (void) const
{
    Identifier result((Identifier::value_type)0);
    // Pack field
    m_det_impl.pack       (neutrino_field_value(), result);
    m_neutrino_part_impl.pack(m_EMULSION_ID, result);
    return (result);
}

Identifier
FaserDetectorID::veto        (void) const
{
    Identifier result((Identifier::value_type)0);
    // Pack field
    m_det_impl.pack       (scint_field_value(), result);
    m_scint_part_impl.pack(m_VETO_ID, result);
    return (result);
}

Identifier
FaserDetectorID::vetonu      (void) const
{
    Identifier result((Identifier::value_type)0);
    // Pack field
    m_det_impl.pack       (scint_field_value(), result);
    m_scint_part_impl.pack(m_VETONU_ID, result);
    return (result);
}

Identifier
FaserDetectorID::trigger          (void) const
{
    Identifier result((Identifier::value_type)0);
    // Pack field
    m_det_impl.pack       (scint_field_value(), result);
    m_scint_part_impl.pack(m_TRIGGER_ID, result);
    return (result);
}

Identifier
FaserDetectorID::preshower          (void) const
{
    Identifier result((Identifier::value_type)0);
    // Pack field
    m_det_impl.pack       (scint_field_value(), result);
    m_scint_part_impl.pack(m_PRESHOWER_ID, result);
    return (result);
}

Identifier
FaserDetectorID::sct          (void) const
{
    Identifier result((Identifier::value_type)0);
    // Pack field
    m_det_impl.pack       (tracker_field_value(), result);
    m_tracker_part_impl.pack(m_SCT_ID, result);
    return (result);
}

Identifier
FaserDetectorID::ecal          (void) const
{
    Identifier result((Identifier::value_type)0);
    // Pack field
    m_det_impl.pack       (calo_field_value(), result);
    m_calo_part_impl.pack(m_ECAL_ID, result);
    return (result);
}

/// IdContext (indicates id length) for detector systems
IdContext
FaserDetectorID::detsystem_context (void) const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_DET_INDEX));
}

/// IdContext (indicates id length) for sub-detector
IdContext
FaserDetectorID::subdet_context  (void) const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_SUBDET_INDEX));
}


int
FaserDetectorID::get_id          (const IdentifierHash& /*hash_id*/,
                                  Identifier& /*id*/,
                                  const IdContext* /*context*/) const
{
    return (0);
}

int
FaserDetectorID::get_hash        (const Identifier& /*id*/,
                                  IdentifierHash& /*hash_id*/,
                                  const IdContext* /*context*/) const
{
    return (0);
}

int
FaserDetectorID::register_dict_tag        (const IdDictMgr& dict_mgr,
                                           const std::string& dict_name)
{
    // Register version of dictionary dict_name

    // Access dictionary by name
    IdDictDictionary* dict = dict_mgr.find_dictionary(dict_name);
    if (!dict) return(1);
    // Add in dict name, file name and version
    m_dict_names.push_back(dict_name);
    m_file_names.push_back(dict->file_name());
    m_dict_tags.push_back(dict->dict_tag());
    return (0);
}

/// Test whether an idhelper should be reinitialized based on the
/// change of tags
bool
FaserDetectorID::reinitialize             (const IdDictMgr& dict_mgr)
{
    // If no tag has been registered, then reinitialize
    if (m_dict_tags.size() == 0) return (true);

    // If no dict names have been registered, then reinitialize
    if (m_dict_names.size() == 0) return (true);

    // Loop over dict names and check version tags
    if (m_dict_names.size() != m_dict_tags.size()) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "FaserDetectorID" );
            log << MSG::ERROR << "reinitialize: dict names and tags vectors not the same length " << endmsg;
            log << MSG::ERROR << "names: " << m_dict_names.size() << " tags: " << m_dict_tags.size() << endmsg;
        }
        else {
            std::cout << MSG::ERROR << "reinitialize: dict names and tags vectors not the same length " << std::endl;
            std::cout << "FaserDetectorID::reinitialize ERROR names: " << m_dict_names.size() << " tags: " << m_dict_tags.size() << std::endl;
        }
    }
    for (unsigned int i = 0; i < m_dict_names.size(); ++i) {
        // Access dictionary by name
        IdDictDictionary* dict = dict_mgr.find_dictionary(m_dict_names[i]);
        if (!dict) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "reinitialize: could not find dict -  " << m_dict_names[i] << endmsg;
            }
            else {
              std::cout << "FaserDetectorID::reinitialize ERROR could not find dict -  " << m_dict_names[i] << std::endl;
            }
            return(false);
        }
        if (m_dict_tags[i] != dict->dict_tag()) {
            // Remove all memory of versions
            m_dict_names.clear();
            m_dict_tags.clear();
            m_file_names.clear();
            return (true);
        }
    }

    // Tags match - don't reinitialize
    return (false);
}



int
FaserDetectorID::initialize_from_dictionary(const IdDictMgr& dict_mgr)
{

    // Register version of FASER dictionary
    if (register_dict_tag(dict_mgr, "FASER")) return(1);

    // Initialize helper, needed for init of FaserDetectorID
    if(!m_helper) {
        m_helper = new FaserDetectorIDHelper;
        m_helper->setMsgSvc(m_msgSvc);
    }

    if(m_helper->initialize_from_dictionary(dict_mgr)) return (1);

    // Initialize level indices and id values from dicts
    if(initLevelsFromDict(dict_mgr)) return (1);

    m_is_initialized_from_dict = true;

    if (!m_quiet) {
      if(m_msgSvc) {
        MsgStream log(m_msgSvc, "FaserDetectorID" );
        log << MSG::INFO << "initialize_from_dictionary - OK" << endmsg;
      }
      else {
        std::cout << " FaserDetectorID::initialize_from_dictionary - OK " << std::endl;
      }
    }
    
    return (0);
}


std::string
FaserDetectorID::dictionaryVersion  (void) const
{
    return (m_dict_version);
}

bool
FaserDetectorID::is_neutrino    (const ExpandedIdentifier& id) const
{
    bool result = false;
    if ( id.fields() > 0 ){
        if ( id[0] == m_NEUTRINO_ID) result = true;
    }
    return result;
}

bool
FaserDetectorID::is_scint       (const ExpandedIdentifier& id) const
{
    bool result = false;
    if ( id.fields() > 0 ){
        if ( id[0] == m_SCINT_ID) result = true;
    }
    return result;
}

bool
FaserDetectorID::is_tracker                 (const ExpandedIdentifier& id) const
{
    bool result = false;
    if ( id.fields() > 0 ){
        if ( id[0] == m_TRACKER_ID) result = true;
    }
    return result;
}

bool
FaserDetectorID::is_calo                (const ExpandedIdentifier& id) const
{
    bool result = false;
    if ( id.fields() > 0 ){
        if ( id[0] == m_CALO_ID ) result = true;
    }
    return result;
}

bool
FaserDetectorID::is_emulsion     (const ExpandedIdentifier& id) const
{
    bool result = false;
    if ( is_neutrino(id) && id.fields() > 1 ){
        if ( id[1] == m_EMULSION_ID ) result = true;
    }
    return result;
}

bool
FaserDetectorID::is_veto       (const ExpandedIdentifier& id) const
{
    bool result = false;
    if ( is_scint(id) && id.fields() > 1 ){
        if ( id[1] == m_VETO_ID ) result = true;
    }
    return result;
}

bool
FaserDetectorID::is_vetonu       (const ExpandedIdentifier& id) const
{
    bool result = false;
    if ( is_scint(id) && id.fields() > 1 ){
        if ( id[1] == m_VETONU_ID ) result = true;
    }
    return result;
}

bool
FaserDetectorID::is_trigger         (const ExpandedIdentifier& id) const
{
    bool result = false;
    if ( is_scint(id) && id.fields() > 1 ){
        if ( id[1] == m_TRIGGER_ID ) return(true);
    }
    return result;
}

bool
FaserDetectorID::is_preshower         (const ExpandedIdentifier& id) const
{
    bool result = false;
    if ( is_scint(id) && id.fields() > 1 ){
        if ( id[1] == m_PRESHOWER_ID ) result = true;
    }
    return result;
}

bool
FaserDetectorID::is_sct         (const ExpandedIdentifier& id) const
{
    bool result = false;
    if ( is_tracker(id) && id.fields() > 1 ){
        if ( id[1] == m_SCT_ID ) result = true;
    }
    return result;
}

bool
FaserDetectorID::is_ecal         (const ExpandedIdentifier& id) const
{
    bool result = false;
    if ( is_calo(id) && id.fields() > 1 ){
        if ( id[1] == m_ECAL_ID ) result = true;
    }
    return result;
}

// Short print out of any identifier:
void
FaserDetectorID::show           (Identifier id,
                                 const IdContext* context,
                                 char sep ) const
{
    if(m_msgSvc) {
        MsgStream log(m_msgSvc, "FaserDetectorID" );
        log << MSG::INFO << show_to_string(id, context, sep) << endmsg;
    }
    else {
        std::cout << show_to_string(id, context, sep) << std::endl;
    }
}

// or provide the printout in string form
std::string
FaserDetectorID::show_to_string (Identifier id,
                                 const IdContext* context,
                                 char sep ) const
{
    // Do a generic printout of identifier

    std::string result("Unable to decode id");
    unsigned int max_index = (context) ? context->end_index()  : 999;

    if (!m_is_initialized_from_dict) return  (result);

    // Find the dictionary to use:
    IdDictDictionary*   dict = 0;
    ExpandedIdentifier expId;
    ExpandedIdentifier prefix;  // default is null prefix
    Identifier compact = id;

    if (is_neutrino(id))
    {
        dict = m_neutrino_dict;
    }
    else if (is_scint(id)) {
        dict = m_scint_dict;
    }
    else if (is_tracker(id)) {
        dict = m_tracker_dict;
    }
    else if (is_calo(id)) {
        dict = m_calo_dict;
    }

    if (!dict) return (result);

    if (dict->unpack(compact,
                     prefix,
                     max_index,
                     expId)) {
        return (result);
    }

    bool first = true;
    char temp[20];

    result = "";
    if ('.' == sep) result = "[";
    for (unsigned int i = 0; i < expId.fields(); ++i) {
        if (first) first = false;
        else result += sep;
        sprintf (temp, "%d", expId[i]);
        result += temp;
    }
    if ('.' == sep) result += "]";

//      result += " compact [";
//      sprintf (temp, "0x%x", (unsigned int)compact);
//      result += temp;
//      result += "]";

    return (result);
}



void
FaserDetectorID::print (Identifier id,
                        const IdContext* context) const
{
    if(m_msgSvc) {
        MsgStream log(m_msgSvc, "FaserDetectorID" );
        log << MSG::INFO << print_to_string(id, context) << endmsg;
    }
    else {
        std::cout << print_to_string(id, context) << std::endl;
    }
}

std::string
FaserDetectorID::print_to_string        (Identifier id,
                                         const IdContext* context) const
{
    // Print out for any Faser identifier
    std::string result;
    if (m_is_initialized_from_dict) {

        // Do a generic printout of identifier from dictionary
        unsigned int max_index = (context) ? context->end_index()  : 999;

        // Find the dictionary to use:
        IdDictDictionary*       dict = 0;
        ExpandedIdentifier expId;
        ExpandedIdentifier prefix;  // default is null prefix
        Identifier compact = id;

        if (is_neutrino(id))
        {
            dict = m_neutrino_dict;
        }
        else if (is_scint(id)) {
            dict = m_scint_dict;
        }
        else if (is_tracker(id)) {
            dict = m_tracker_dict;
        }
        else if (is_calo(id)) {
            dict = m_calo_dict;
        }

        if (!dict) return (result);

        if (dict->unpack(compact,
                         prefix,
                         max_index,
                         " ",
                         result)) {
            return (result);
        }
    }
    return (result);
}

///  Dictionary name
std::vector<std::string>
FaserDetectorID::dict_names         (void) const
{
    return (m_dict_names);
}

///  File name
std::vector<std::string>
FaserDetectorID::file_names         (void) const
{
    return (m_file_names);
}

///  Version tag for subdet dictionary
std::vector<std::string>
FaserDetectorID::dict_tags      (void) const
{
    return (m_dict_tags);
}

bool            
FaserDetectorID::do_checks      (void) const
{
    return (m_do_checks);
}

void
FaserDetectorID::set_do_checks  (bool do_checks) 
{
    m_do_checks = do_checks;
}

bool
FaserDetectorID::do_neighbours  (void) const
{
    return (m_do_neighbours);
}

void
FaserDetectorID::set_do_neighbours      (bool do_neighbours) 
{
    m_do_neighbours = do_neighbours;
}

void
FaserDetectorID::setMessageSvc  (IMessageSvc* msgSvc)
{
    m_msgSvc = msgSvc ;
}

void
FaserDetectorID::set_quiet  (bool quiet)
{
    m_quiet = quiet ;
}

void
FaserDetectorID::setDictVersion  (const IdDictMgr& dict_mgr, const std::string& name)
{
    const IdDictDictionary* dict = dict_mgr.find_dictionary (name);

    m_dict_version = dict->m_version;
}

std::string
FaserDetectorID::to_range (const ExpandedIdentifier& id) const
{

    // Build a string from the contents of an identifier

    int fields = id.fields();
    char temp[10] = "";
    std::string result("");

    for (int i = 0; i < fields; ++i) {
        sprintf( temp, "%d", id[i]);
        if (i > 0) result += '/'; // add '/' only if NOT last one
        result += temp;
    }

    return result;
}

int
FaserDetectorID::initLevelsFromDict(const IdDictMgr& dict_mgr)
{

    // Set do_checks flag
    if (dict_mgr.do_checks()) m_do_checks = true;
    // Set do_neighbours flag
    if (!dict_mgr.do_neighbours()) m_do_neighbours = false;

    IdDictLabel* label = 0;
    IdDictField* field = 0;

    // Find out from the dictionary the detector and subdetector
    // levels and id values
    m_DET_INDEX             = 999;
    m_SUBDET_INDEX          = 999;
    m_NEUTRINO_ID           = -1;
    m_SCINT_ID              = -1;
    m_TRACKER_ID            = -1;
    m_CALO_ID               = -1;
    m_EMULSION_ID           = -1;
    m_VETO_ID               = -1;
    m_TRIGGER_ID            = -1;
    m_PRESHOWER_ID          = -1;
    m_VETONU_ID             = -1;
    m_SCT_ID                = -1;
    m_ECAL_ID               = -1;

    // Save generic dict for top levels
    IdDictDictionary*   top_dict = 0;

    // Get det ids

    // Initialize ids for Neutrino subdet

    m_neutrino_dict = dict_mgr.find_dictionary ("Neutrino");
    if(!m_neutrino_dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "FaserDetectorID" );
            log << MSG::WARNING << "initLevelsFromDict - cannot access Neutrino dictionary" << endmsg;
        }
        else {
            std::cout << " FaserDetectorID::initLevelsFromDict - Warning cannot access Neutrino dictionary "
                      << std::endl;
        }
    }
    else {

        // Found Neutrino dict

        top_dict = m_neutrino_dict;  // save as top_dict

        // Check if this is SLHC layout
        // m_isSLHC = (m_scint_dict->m_version=="SLHC");

        // Get Neutrino subdets

        field = m_neutrino_dict->find_field("part");
        if (!field) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'part' field for Neutrino dictionary" << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'part' field for Neutrino dictionary"
                          << std::endl;
            }
            return (1);
        }

        label = field->find_label("Emulsion");
        if (label) {
            if (label->m_valued) {
                m_EMULSION_ID = label->m_value;
            }
            else {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorID" );
                    log << MSG::ERROR << "initLevelsFromDict - label Emulsion does NOT have a value "
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorID::initLevelsFromDict - label Emulsion does NOT have a value "
                              << std::endl;
                }
                return (1);
            }
        }
        else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'Emulsion' label "
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'Emulsion' label "
                          << std::endl;
            }
            return (1);
        }
    }

    // Initialize ids for Scint subdet
    m_scint_dict = dict_mgr.find_dictionary ("Scintillator");
    if(!m_scint_dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "FaserDetectorID" );
            log << MSG::WARNING << "initLevelsFromDict - cannot access Scintillator dictionary" << endmsg;
        }
        else {
            std::cout << " FaserDetectorID::initLevelsFromDict - Warning cannot access Scintillator dictionary "
                      << std::endl;
        }
    }
    else {

        // Found Scint dict

        top_dict = m_scint_dict;  // save as top_dict

        // Check if this is SLHC layout
        m_isSLHC = (m_scint_dict->m_version=="SLHC");

        // Get Scint subdets

        field = m_scint_dict->find_field("part");
        if (!field) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'part' field for Scintillator dictionary" << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'part' field for Scintillator dictionary"
                          << std::endl;
            }
            return (1);
        }

        label = field->find_label("Veto");
        if (label) {
            if (label->m_valued) {
                m_VETO_ID = label->m_value;
            }
            else {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorID" );
                    log << MSG::ERROR << "initLevelsFromDict - label Veto does NOT have a value "
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorID::initLevelsFromDict - label Veto does NOT have a value "
                              << std::endl;
                }
                return (1);
            }
        }
        else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'Veto' label "
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'Veto' label "
                          << std::endl;
            }
            return (1);
        }

        label = field->find_label("Trigger");
        if (label) {
            if (label->m_valued) {
                m_TRIGGER_ID = label->m_value;
            }
            else {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorID" );
                    log << MSG::ERROR << "initLevelsFromDict - label Trigger does NOT have a value "
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorID::initLevelsFromDict - label Trigger does NOT have a value "
                              << std::endl;
                }
                return (1);
            }
        }
        else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'Trigger' label "
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'Trigger' label "
                          << std::endl;
            }
            return (1);
        }

        label = field->find_label("Preshower");
        if (label) {
            if (label->m_valued) {
                m_PRESHOWER_ID = label->m_value;
            }
            else {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorID" );
                    log << MSG::ERROR << "initLevelsFromDict - label Preshower does NOT have a value "
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorID::initLevelsFromDict - label Preshower does NOT have a value "
                              << std::endl;
                }
                return (1);
            }
        }
        else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'Preshower' label "
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'Preshower' label "
                          << std::endl;
            }
            return (1);
        }

        label = field->find_label("VetoNu");
        if (label) {
            if (label->m_valued) {
                m_VETONU_ID = label->m_value;
            }
            else {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorID" );
                    log << MSG::ERROR << "initLevelsFromDict - label VetoNu does NOT have a value "
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorID::initLevelsFromDict - label VetoNu does NOT have a value "
                              << std::endl;
                }
                return (1);
            }
        }
        else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'VetoNu' label "
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'VetoNu' label "
                          << std::endl;
            }
            return (1);
        }


    }

    // Initialize ids for Tracker
    m_tracker_dict = dict_mgr.find_dictionary ("Tracker");
    if(!m_tracker_dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "FaserDetectorID" );
            log << MSG::WARNING << "initLevelsFromDict -  cannot access Tracker dictionary "
                << endmsg;
        }
        else {
            std::cout << " FaserDetectorID::initLevelsFromDict - Warning cannot access Tracker dictionary "
                      << std::endl;
        }
    }
    else {
        // Found Tracker dict

        if (!top_dict) top_dict = m_tracker_dict;  // save as top_dict

        field = m_tracker_dict->find_field("part");
        if (!field) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'station' field for Tracker dictionary"
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'station' field for Tracker dictionary"
                          << std::endl;
            }
            return (1);
        }

        label = field->find_label("SCT");
        if (label) {
            if (label->m_valued) {
                m_SCT_ID = label->m_value;
            }
            else {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorID" );
                    log << MSG::ERROR << "initLevelsFromDict - label SCT does NOT have a value "
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorID::initLevelsFromDict - label SCT does NOT have a value "
                              << std::endl;
                }
                return (1);
            }
        }
        else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'SCT' label "
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'SCT' label "
                          << std::endl;
            }
            return (1);
        }
    }

    // Initialize id for Calo and fields for lvl1 and dead material
    m_calo_dict = dict_mgr.find_dictionary ("Calorimeter");
    if(!m_calo_dict) {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "FaserDetectorID" );
            log << MSG::ERROR << "initLevelsFromDict - Warning cannot access Calorimeter dictionary "
                << endmsg;
        }
        else {
            std::cout << " FaserDetectorID::initLevelsFromDict - Warning cannot access Calorimeter dictionary "
                      << std::endl;
        }
    }
    else {
        // Found calo dict
        if (!top_dict) top_dict = m_calo_dict;  // save as top_dict

        // Get Scint subdets
        field = m_calo_dict->find_field("part");
        if (!field) {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'part' field for Calorimeter dictionary" << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'part' field for Calorimeter dictionary"
                          << std::endl;
            }
            return (1);
        }

        label = field->find_label("Ecal");
        if (label) {
            if (label->m_valued) {
                m_ECAL_ID = label->m_value;
            }
            else {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorID" );
                    log << MSG::ERROR << "initLevelsFromDict - label ECAL does NOT have a value "
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorID::initLevelsFromDict - label ECAL does NOT have a value "
                              << std::endl;
                }
                return (1);
            }
        }
        else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'ECAL' label "
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'ECAL' label "
                          << std::endl;
            }
            return (1);
        }
    }

    // set det/subdet indices
    if (top_dict) {
        field = top_dict->find_field("subdet");
        if (field) {
            m_DET_INDEX = field->m_index;
        }
        else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict -  - unable to find 'subdet' field from dict "
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'subdet' field from dict "
                          << top_dict->m_name
                          << std::endl;
            }
            return (1);
        }

        // Get neutrino id
        label = field->find_label("Neutrino");
        if (label) {
            if (label->m_valued) {
                m_NEUTRINO_ID = label->m_value;
            }
            else {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorID" );
                    log << MSG::ERROR << "initLevelsFromDict - label Neutrino does NOT have a value "
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorID::initLevelsFromDict - label Neutrino does NOT have a value "
                              << std::endl;
                }
                return (1);
            }
        }
        else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'Neutrino' label "
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'Neutrino' label "
                          << std::endl;
            }
            return (1);
        }

        // Get scint id
        label = field->find_label("Scintillator");
        if (label) {
            if (label->m_valued) {
                m_SCINT_ID = label->m_value;
            }
            else {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorID" );
                    log << MSG::ERROR << "initLevelsFromDict - label Scintillator does NOT have a value "
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorID::initLevelsFromDict - label Scintillator does NOT have a value "
                              << std::endl;
                }
                return (1);
            }
        }
        else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'Scintillator' label "
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'Scintillator' label "
                          << std::endl;
            }
            return (1);
        }

        // Get Tracker id
        label = field->find_label("Tracker");
        if (label) {
            if (label->m_valued) {
                m_TRACKER_ID = label->m_value;
            }
            else {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorID" );
                    log << MSG::ERROR << "initLevelsFromDict - label Tracker does NOT have a value "
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorID::initLevelsFromDict - label Tracker does NOT have a value "
                              << std::endl;
                }
                return (1);
            }
        }
        else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'Tracker' label "
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'Tracker' label "
                          << std::endl;
            }
            return (1);
        }

        // Get Calo id
        label = field->find_label("Calorimeter");
        if (label) {
            if (label->m_valued) {
                m_CALO_ID = label->m_value;
            }
            else {
                if(m_msgSvc) {
                    MsgStream log(m_msgSvc, "FaserDetectorID" );
                    log << MSG::ERROR << "initLevelsFromDict - label Calorimeter does NOT have a value "
                        << endmsg;
                }
                else {
                    std::cout << "FaserDetectorID::initLevelsFromDict - label Calorimeter does NOT have a value "
                              << std::endl;
                }
                return (1);
            }
        }
        else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find 'Calorimeter' label "
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find 'Calorimeter' label "
                          << std::endl;
            }
            return (1);
        }

        // Get name of next level
        std::string name;
        if (top_dict->m_name == "Neutrino")
        {
            name = "part";
        }
        else if (top_dict->m_name == "Scintillator") {
            name = "part";
        }
        else if (top_dict->m_name == "Calorimeter") {
            name = "part";
        }
        else if (top_dict->m_name == "Tracker") {
            name = "part";
        }

        // While we're here, save the index to the sub-detector level
        // ("part" for InDet)
        field = top_dict->find_field(name);
        if (field) {
            m_SUBDET_INDEX = field->m_index;
        }
        else {
            if(m_msgSvc) {
                MsgStream log(m_msgSvc, "FaserDetectorID" );
                log << MSG::ERROR << "initLevelsFromDict - unable to find field "
                    << name << " from dict "
                    << top_dict->m_name
                    << endmsg;
            }
            else {
                std::cout << "FaserDetectorID::initLevelsFromDict - unable to find field "
                          << name << " from dict "
                          << top_dict->m_name
                          << std::endl;
            }
            return (1);
        }
    }
    else {
        if(m_msgSvc) {
            MsgStream log(m_msgSvc, "FaserDetectorID" );
            log << MSG::ERROR << "initLevelsFromDict - no top dictionary defined "
                << endmsg;
        }
        else {
            std::cout << "FaserDetectorID::initLevelsFromDict - no top dictionary defined "
                      << std::endl;
        }
        return (1);
    }


    // Set the field implementations

    const IdDictRegion* region = 0;

    size_type region_index =  m_helper->emulsion_region_index();
    if (m_neutrino_dict && FaserDetectorIDHelper::UNDEFINED != region_index) {
        region                 =  m_neutrino_dict->m_regions[region_index];
        // Detector
        m_det_impl             = region->m_implementation[m_DET_INDEX];
        // InDet part
        m_neutrino_part_impl      = region->m_implementation[m_SUBDET_INDEX];
    }
    
    region_index =  m_helper->veto_region_index();
    if (m_scint_dict && FaserDetectorIDHelper::UNDEFINED != region_index) {
        region                 =  m_scint_dict->m_regions[region_index];
        // Detector
        m_det_impl             = region->m_implementation[m_DET_INDEX];
        // InDet part
        m_scint_part_impl      = region->m_implementation[m_SUBDET_INDEX];
    }

    region_index =  m_helper->sct_region_index();
    if (m_tracker_dict && FaserDetectorIDHelper::UNDEFINED != region_index) {
        region                 =  m_tracker_dict->m_regions[region_index];
        // Detector
        m_det_impl             = region->m_implementation[m_DET_INDEX];
        // InDet part
        m_tracker_part_impl    = region->m_implementation[m_SUBDET_INDEX];
    }

    region_index =  m_helper->ecal_region_index();
    if (m_calo_dict && FaserDetectorIDHelper::UNDEFINED != region_index) {
        region                 =  m_calo_dict->m_regions[region_index];
        // Detector
        m_det_impl             = region->m_implementation[m_DET_INDEX];
        // InDet part
        m_calo_part_impl    = region->m_implementation[m_SUBDET_INDEX];
    }
    return (0);
}
