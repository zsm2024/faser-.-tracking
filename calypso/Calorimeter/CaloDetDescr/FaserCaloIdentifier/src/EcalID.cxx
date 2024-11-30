/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calorimeter identifier package
 -------------------------------------------
***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>
#include "GaudiKernel/MsgStream.h"

#include "FaserCaloIdentifier/EcalID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictDefs.h"  
#include <set>
#include <algorithm>
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


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


EcalID::EcalID(void)
        :
        m_ecal_region_index(0),
        m_CALO_INDEX(0),
        m_ECAL_INDEX(1),
        m_ROW_INDEX(2),
        m_MODULE_INDEX(3),
        m_PMT_INDEX(4),
        m_dict(0),
        m_module_hash_max(0),
        m_pmt_hash_max(0)
{
}

void
EcalID::module_id_checks ( int row,  
                           int module ) const
{

    // Check that id is within allowed range

    // Fill expanded id
    ExpandedIdentifier id;
    id << calo_field_value() << ecal_field_value()
       << row << module;

    if (!m_full_module_range.match(id)) {  // module range check is sufficient
        MsgStream log(m_msgSvc, "EcalID");
        log << MSG::ERROR << " EcalID::module_id result is NOT ok. ID, range "
            << (std::string)id <<  " " << (std::string)m_full_module_range << endmsg;
    }
}

void
EcalID::pmt_id_checks ( int row,  
                        int module, 
                        int pmt) const
{

    // Check that id is within allowed range

    // Fill expanded id
    ExpandedIdentifier id;
    id << calo_field_value() << ecal_field_value()
       << row << module << pmt;

    if (!m_full_pmt_range.match(id)) {  
        MsgStream log(m_msgSvc, "EcalID");
        log << MSG::ERROR << " EcalID::pmt_id result is NOT ok. ID, range "
            << (std::string)id << " " << (std::string)m_full_pmt_range << std::endl;
    }
}

int 
EcalID::row_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext module_context1 = module_context();
    get_expanded_id(id, expId, &module_context1);
    for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
        const Range& range = m_full_module_range[i];
        if (range.match(expId)) {
            const Range::field& row_field = range[m_ROW_INDEX];
            if (row_field.has_maximum()) {
                return (row_field.get_maximum());
            }
        }
    }
    return (-999);  // default
}

int     
EcalID::pmt_max       (const Identifier& id) const
{
    ExpandedIdentifier expId;
    IdContext row_context(expId, 0, m_ROW_INDEX);
    get_expanded_id(id, expId, &row_context);
    int result = -999;
    for (unsigned int i = 0; i < m_full_pmt_range.size(); ++i) {
        const Range& range = m_full_pmt_range[i];
        if (range.match(expId)) {
            const Range::field& pmt_field = range[m_PMT_INDEX];
            if (pmt_field.has_maximum()) {
                int pmt = pmt_field.get_maximum();
                if (result < pmt) result = pmt;
            }
        }
    }
    return (result);
}

int 
EcalID::module_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext module_context1 = module_context();
    get_expanded_id(id, expId, &module_context1);
    for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
        const Range& range = m_full_module_range[i];
        if (range.match(expId)) {
            const Range::field& module_field = range[m_MODULE_INDEX];
            if (module_field.has_maximum()) {
                return (module_field.get_maximum());
            }
        }
    }
    return -1;
}

int
EcalID::initialize_from_dictionary(const IdDictMgr& dict_mgr)
{
    MsgStream log(m_msgSvc, "EcalID");
    log << MSG::INFO << "Initialize from dictionary" << endmsg;
  
    // Check whether this helper should be reinitialized
    if (!reinitialize(dict_mgr)) {
        log << MSG::INFO << "Request to reinitialize not satisfied - tags have not changed" << endmsg;
        return (0);
    }
    else {
        if (m_msgSvc) {
            log << MSG::DEBUG << "(Re)initialize" << endmsg;
        }
        else {
            std::cout  << " DEBUG (Re)initialize" << std::endl;
        }
    }

    // init base object
    if(FaserDetectorID::initialize_from_dictionary(dict_mgr)) return (1);

    // Register version of InnerDetector dictionary 
    if (register_dict_tag(dict_mgr, "Calorimeter")) return(1);

    m_dict = dict_mgr.find_dictionary ("Calorimeter"); 
    if(!m_dict) {
        log << MSG::ERROR << " EcalID::initialize_from_dict - cannot access Calorimeter dictionary " << endmsg;
        return 1;
    }

    // Initialize the field indices
    if(initLevelsFromDict()) return (1);

    //
    // Build multirange for the valid set of identifiers
    //


    // Find value for the field Scintillator
    const IdDictDictionary* faserDict = dict_mgr.find_dictionary ("FASER"); 
    int caloField   = -1;
    if (faserDict->get_label_value("subdet", "Calorimeter", caloField)) {
        log << MSG::ERROR << "Could not get value for label 'Calorimeter' of field 'subdet' in dictionary " 
            << faserDict->m_name
            << endmsg;
        return (1);
    }

    // Find value for the field Ecal
    int ecalField   = -1;
    if (m_dict->get_label_value("part", "Ecal", ecalField)) {
        log << MSG::ERROR << "Could not get value for label 'Ecal' of field 'part' in dictionary " 
            << m_dict->m_name
            << endmsg;
        return (1);
    }
    if (m_msgSvc) {
        log << MSG::DEBUG << " EcalID::initialize_from_dict " 
            << "Found field values: Ecal "  
            << ecalField
            << std::endl;
    }
    else {
        std::cout << " DEBUG EcalID::initialize_from_dict " 
                  << "Found field values: Ecal "  
                  << ecalField
                  << std::endl;
    }
    
    // Set up id for region and range prefix
    ExpandedIdentifier region_id;
    region_id.add(caloField);
    region_id.add(ecalField);
    Range prefix;
    m_full_module_range = m_dict->build_multirange(region_id, prefix, "module");
    m_full_pmt_range = m_dict->build_multirange(region_id, prefix);

    // Setup the hash tables
    if(init_hashes()) return (1);

    // Setup hash tables for finding neighbors
    if(init_neighbors()) return (1);
    
    if (m_msgSvc) {
        log << MSG::INFO << " EcalID::initialize_from_dict "  << endmsg;
        log << MSG::DEBUG  
            << "Module range -> " << (std::string)m_full_module_range
            <<   endmsg;
        log << MSG::DEBUG
            << "Pmt range -> " << (std::string)m_full_pmt_range
            << endmsg;
    }
    else {
        std::cout << " INFO EcalID::initialize_from_dict "  << std::endl;
        std::cout << " DEBUG  Module range -> " << (std::string)m_full_module_range
                  <<   std::endl;
        std::cout << " DEBUG Pmt range -> " << (std::string)m_full_pmt_range
                  << std::endl;
    }
    
    return 0;
}

int
EcalID::init_hashes(void)
{

    //
    // create a vector(s) to retrieve the hashes for compact ids. For
    // the moment, we implement a hash for modules but NOT for pmts
    //
    MsgStream log(m_msgSvc, "EcalID");
    // module hash
    m_module_hash_max = m_full_module_range.cardinality();
    m_module_vec.resize(m_module_hash_max);
    unsigned int nids = 0;
    std::set<Identifier> ids;
    for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
        const Range& range = m_full_module_range[i];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last  = range.factory_end();
        for (; first != last; ++first) {
            const ExpandedIdentifier& exp_id = (*first);
            Identifier id = module_id(exp_id[m_ROW_INDEX],
                                      exp_id[m_MODULE_INDEX]); 
            if(!(ids.insert(id)).second) {
                log << MSG::ERROR << " EcalID::init_hashes "
                    << " Error: duplicated id for module id. nid " << nids
                    << " compact id " << id.getString()
                    << " id " << (std::string)exp_id << endmsg;
                return (1);
            }
            nids++;
        }
    }
    if(ids.size() != m_module_hash_max) {
        log << MSG::ERROR << " EcalID::init_hashes "
            << " Error: set size NOT EQUAL to hash max. size " << ids.size()
            << " hash max " << m_module_hash_max 
            << endmsg;
        return (1);
    }

    nids = 0;
    std::set<Identifier>::const_iterator first = ids.begin();
    std::set<Identifier>::const_iterator last  = ids.end();
    for (; first != last && nids < m_module_vec.size(); ++first) {
        m_module_vec[nids] = (*first);
        nids++;
    }

    // pmt hash - we do not keep a vec for the pmts
    m_pmt_hash_max = m_full_pmt_range.cardinality();

    return (0);
}

    // int
    // EcalID::get_prev_in_z(const IdentifierHash& id, IdentifierHash& prev) const
    // {
    //     unsigned short index = id;
    //     if (index < m_prev_z_module_vec.size())
    //     {
    //         if (m_prev_z_module_vec[index] == NOT_VALID_HASH) return (1);
    //         prev = m_prev_z_module_vec[index];
    //         return (0);
    //     }
    //     return (1);
    // }

    // int
    // EcalID::get_next_in_z(const IdentifierHash& id, IdentifierHash& next) const
    // {
    //     unsigned short index = id;
    //     if (index < m_next_z_module_vec.size())
    //     {
    //         if (m_next_z_module_vec[index] == NOT_VALID_HASH) return (1);
    //         next = m_next_z_module_vec[index];
    //         return (0);
    //     }
    //     return (1);
    // }

int             
EcalID::get_prev_in_phi(const IdentifierHash& id, IdentifierHash& prev) const
{
    unsigned short index = id;
    if (index < m_prev_phi_module_vec.size()) {
        if (m_prev_phi_module_vec[index] == NOT_VALID_HASH) return (1);
        prev =  m_prev_phi_module_vec[index];
        return (0);
    }
    return (1);
}

int             
EcalID::get_next_in_phi(const IdentifierHash& id, IdentifierHash& next) const
{
    unsigned short index = id;
    if (index < m_next_phi_module_vec.size()) {
        if (m_next_phi_module_vec[index] == NOT_VALID_HASH) return (1);
        next =  m_next_phi_module_vec[index];
        return (0);
    }
    return (1);
}

int             
EcalID::get_prev_in_eta(const IdentifierHash& id, IdentifierHash& prev) const
{
    unsigned short index = id;
    if (index < m_prev_eta_module_vec.size()) {
        if (m_prev_eta_module_vec[index] == NOT_VALID_HASH) return (1);
        prev =  m_prev_eta_module_vec[index];
        return (0);
    }
    return (1);
}

int
EcalID::get_next_in_eta(const IdentifierHash& id, IdentifierHash& next) const
{
    unsigned short index = id;
    if (index < m_next_eta_module_vec.size()) {
        if (m_next_eta_module_vec[index] == NOT_VALID_HASH) return (1);
        next =  m_next_eta_module_vec[index];
        return (0);
    }
    return (1);
}

int
EcalID::init_neighbors(void)
{
    //
    // create a vector(s) to retrieve the hashes for compact ids for
    // module neighbors.
    //
    MsgStream log(m_msgSvc, "EcalID");

    m_prev_phi_module_vec.resize(m_module_hash_max, NOT_VALID_HASH);
    m_next_phi_module_vec.resize(m_module_hash_max, NOT_VALID_HASH);
    m_prev_eta_module_vec.resize(m_module_hash_max, NOT_VALID_HASH);
    m_next_eta_module_vec.resize(m_module_hash_max, NOT_VALID_HASH);

    for (unsigned int i = 0; i < m_full_module_range.size(); ++i) {
        const Range& range = m_full_module_range[i];
        const Range::field& phi_field = range[m_MODULE_INDEX];
        const Range::field& eta_field = range[m_ROW_INDEX];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last  = range.factory_end();
        for (; first != last; ++first) {
            const ExpandedIdentifier& exp_id = (*first);
            ExpandedIdentifier::element_type previous_phi;
            ExpandedIdentifier::element_type next_phi;
            ExpandedIdentifier::element_type previous_eta;
            ExpandedIdentifier::element_type next_eta;
            bool pphi = phi_field.get_previous(exp_id[m_MODULE_INDEX], previous_phi);
            bool nphi = phi_field.get_next    (exp_id[m_MODULE_INDEX], next_phi);
            bool peta = eta_field.get_previous(exp_id[m_ROW_INDEX], previous_eta);
            bool neta = eta_field.get_next    (exp_id[m_ROW_INDEX], next_eta);

            IdContext      wcontext = module_context();
            
            // First get primary hash id
            IdentifierHash hash_id;
            Identifier id = module_id(exp_id[m_ROW_INDEX],
                                     exp_id[m_MODULE_INDEX]); 
            if (get_hash(id, hash_id, &wcontext)) {
                log << MSG::ERROR << " EcalID::init_neighbors - unable to get hash, exp/compact "
                    << show_to_string(id, &wcontext)
                    << " " << (std::string)m_full_module_range << endmsg;
                return (1);
            }

            // index for the subsequent arrays
            unsigned short index = hash_id;
            assert (hash_id < m_prev_phi_module_vec.size());
            assert (hash_id < m_next_phi_module_vec.size());
            assert (hash_id < m_prev_eta_module_vec.size());
            assert (hash_id < m_next_eta_module_vec.size());
            
            if (pphi) {
                // Get previous phi hash id
                ExpandedIdentifier expId = exp_id;
                expId[m_MODULE_INDEX] = previous_phi;
                Identifier id = module_id(expId[m_ROW_INDEX],
                                          expId[m_MODULE_INDEX]);
                if (get_hash(id, hash_id, &wcontext)) {
                    log << MSG::ERROR << " EcalID::init_neighbors - unable to get previous phi hash, exp/compact " << id.getString() << " " 
                        << endmsg;
                    return (1);
                }
                m_prev_phi_module_vec[index] = hash_id;
            }
            
            if (nphi) {
                // Get next phi hash id
                ExpandedIdentifier expId = exp_id;
                expId[m_MODULE_INDEX] = next_phi;
                Identifier id = module_id(expId[m_ROW_INDEX],
                                          expId[m_MODULE_INDEX]);
                if (get_hash(id, hash_id, &wcontext)) {
                    log << MSG::ERROR << " EcalID::init_neighbors - unable to get next phi hash, exp/compact " << id.getString() << 
                        " " << MSG::hex << id.getString() << MSG::dec << endmsg;
                    return (1);
                }
                m_next_phi_module_vec[index] = hash_id;
            }
            
            if (peta) {
                // Get previous eta hash id
                ExpandedIdentifier expId = exp_id;
                expId[m_ROW_INDEX] = previous_eta;
                Identifier id = module_id(expId[m_ROW_INDEX],
                                          expId[m_MODULE_INDEX]);
                if (get_hash(id, hash_id, &wcontext)) {
                    log << MSG::ERROR << " EcalID::init_neighbors - unable to get previous eta hash, exp/compact " << id.getString() 
                        << " " << std::endl;
                    return (1);
                }
                m_prev_eta_module_vec[index] = hash_id;
            }
            
            if (neta) {
                // Get next eta hash id
                ExpandedIdentifier expId = exp_id;
                expId[m_ROW_INDEX] = next_eta;
                Identifier id = module_id(expId[m_ROW_INDEX],
                                          expId[m_MODULE_INDEX]);
                if (get_hash(id, hash_id, &wcontext)) {
                    log << MSG::ERROR << " EcalID::init_neighbors - unable to get next eta hash, exp/compact " << id.getString() 
                        << " " << endmsg;
                    return (1);
                }
                m_next_eta_module_vec[index] = hash_id;
            }
            

        //  std::cout << " EcalID::init_neighbors "
        //            << " phi, previous, next " << id[m_MODULE_INDEX]
        //            << " " << pphi
        //            << " " << previous_phi
        //            << " " << nphi
        //            << " " << next_phi
        //            << " eta, previous, next " << id[m_ROW_INDEX]
        //            << " " << peta
        //            << " " << previous_eta
        //            << " " << neta
        //            << " " << next_eta
        //            << " id " << (std::string)(*first) 
        //            << std::endl;
        }
    }
    return (0);
}



int     
EcalID::initLevelsFromDict()
{
    MsgStream log(m_msgSvc, "EcalID");
    if(!m_dict) {
        log << MSG::ERROR << " EcalID::initLevelsFromDict - dictionary NOT initialized " << endmsg;
        return (1);
    }
    
    // Find out which identifier field corresponds to each level. Use
    // names to find each field/leve.

    m_CALO_INDEX                = 999;
    m_ECAL_INDEX                = 999;
    m_ROW_INDEX                 = 999;
    m_MODULE_INDEX              = 999;
    m_PMT_INDEX                 = 999;    

    // Save index to a Ecal region for unpacking
    ExpandedIdentifier id; 
    id << calo_field_value() << ecal_field_value();
    if (m_dict->find_region(id, m_ecal_region_index)) {
        log << MSG::ERROR << "EcalID::initLevelsFromDict - unable to find ecal region index: id, reg "  
            << (std::string)id << " " << m_ecal_region_index
            << endmsg;
        return (1);
    }

    // Find a Ecal region
    IdDictField* field = m_dict->find_field("subdet");
    if (field) {
        m_CALO_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "EcalID::initLevelsFromDict - unable to find 'subdet' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("part");
    if (field) {
        m_ECAL_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "EcalID::initLevelsFromDict - unable to find 'part' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("row");
    if (field) {
        m_ROW_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "EcalID::initLevelsFromDict - unable to find 'row' field "   << endmsg;
        return (1);
    }
    field = m_dict->find_field("module");
    if (field) {
        m_MODULE_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR<< "EcalID::initLevelsFromDict - unable to find 'module' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("pmt");
    if (field) {
        m_PMT_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "EcalID::initLevelsFromDict - unable to find 'pmt' field " << endmsg;    
        return (1);
    }
    
    // Set the field implementations

    const IdDictRegion& region = *m_dict->m_regions[m_ecal_region_index];

    m_calo_impl      = region.m_implementation[m_CALO_INDEX]; 
    m_ecal_impl      = region.m_implementation[m_ECAL_INDEX]; 
    m_row_impl       = region.m_implementation[m_ROW_INDEX]; 
    m_module_impl    = region.m_implementation[m_MODULE_INDEX];
    m_pmt_impl       = region.m_implementation[m_PMT_INDEX]; 

    if (m_msgSvc) {
        log << MSG::DEBUG << "decode index and bit fields for each level: " << endmsg;
        log << MSG::DEBUG << "calo      "  << m_calo_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << "ecal      "  << m_ecal_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "row       "  << m_row_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "module    "  << m_module_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "pmt       "  << m_pmt_impl.show_to_string() << endmsg; 
    }
    else {
        std::cout << " DEBUG decode index and bit fields for each level: " << std::endl;
        std::cout << " DEBUG calo    "  << m_calo_impl.show_to_string() << std::endl;
        std::cout << " DEBUG ecal    "  << m_ecal_impl.show_to_string() << std::endl; 
        std::cout << " DEBUG row     "  << m_row_impl.show_to_string() << std::endl; 
        std::cout << " DEBUG module  "  << m_module_impl.show_to_string() << std::endl;
        std::cout << " DEBUG pmt     "  << m_pmt_impl.show_to_string() << std::endl; 
    }
    
    std::cout << "calo "  << m_calo_impl.decode_index() << " " 
              <<   (std::string)m_calo_impl.ored_field() << " " 
              << std::hex    << m_calo_impl.mask() << " " 
              << m_calo_impl.zeroing_mask() << " " 
              << std::dec    << m_calo_impl.shift() << " "
              << m_calo_impl.bits() << " "
              << m_calo_impl.bits_offset()
              << std::endl;
    std::cout << "ecal "     << m_ecal_impl.decode_index() << " " 
              <<   (std::string)m_ecal_impl.ored_field() << " " 
              << std::hex    << m_ecal_impl.mask() << " " 
              << m_ecal_impl.zeroing_mask() << " " 
              << std::dec    << m_ecal_impl.shift() << " "
              << m_ecal_impl.bits() << " "
              << m_ecal_impl.bits_offset()
              << std::endl;
    std::cout << "row "<< m_row_impl.decode_index() << " " 
              <<   (std::string)m_row_impl.ored_field() << " " 
              << std::hex    << m_row_impl.mask() << " " 
              << m_row_impl.zeroing_mask() << " " 
              << std::dec    << m_row_impl.shift() << " "
              << m_row_impl.bits() << " "
              << m_row_impl.bits_offset()
              << std::endl;
    std::cout << "module "    << m_module_impl.decode_index() << " " 
              <<   (std::string)m_module_impl.ored_field() << " " 
              << std::hex    << m_module_impl.mask() << " " 
              << m_module_impl.zeroing_mask() << " " 
              << std::dec    << m_module_impl.shift() << " "
              << m_module_impl.bits() << " "
              << m_module_impl.bits_offset()
              << std::endl;
    std::cout << "pmt"   << m_pmt_impl.decode_index() << " " 
              <<   (std::string)m_pmt_impl.ored_field() << " " 
              << std::hex    << m_pmt_impl.mask() << " " 
              << m_pmt_impl.zeroing_mask() << " " 
              << std::dec    << m_pmt_impl.shift() << " "
              << m_pmt_impl.bits() << " "
              << m_pmt_impl.bits_offset()
              << std::endl;

    return (0);
}

EcalID::size_type       
EcalID::module_hash_max (void) const
{
    return m_module_hash_max;
}

EcalID::size_type       
EcalID::pmt_hash_max (void) const
{
    return m_pmt_hash_max;
}

EcalID::const_id_iterator       EcalID::module_begin             (void) const
{
    return (m_module_vec.begin());
}

EcalID::const_id_iterator       EcalID::module_end               (void) const
{
    return (m_module_vec.end());
}

EcalID::const_expanded_id_iterator      EcalID::pmt_begin     (void) const
{
    return (m_full_pmt_range.factory_begin());
}

EcalID::const_expanded_id_iterator      EcalID::pmt_end       (void) const
{
    return (m_full_pmt_range.factory_end());
}

// From hash get Identifier
int     
EcalID::get_id          (const IdentifierHash& hash_id,
                         Identifier& id,
                         const IdContext* context) const
{

    int result = 1;
    id.clear();

    size_t begin = (context) ? context->begin_index(): 0;
    // cannot get hash if end is 0:
    size_t end   = (context) ? context->end_index()  : 0; 
    if (0 == begin) { 
        // No hashes yet for ids with prefixes
        if (m_MODULE_INDEX == end) {
            if (hash_id < (unsigned int)(m_module_vec.end() - m_module_vec.begin())) {
                id = m_module_vec[hash_id];
                result = 0;
            }
        }
        else if (m_PMT_INDEX == end) {
            // Do not know how to calculate strip id from hash yet!!
            std::cout << "Do not know how to calculate pmt id from hash yet!!" << std::endl;
        }
    }
    return (result);
}

void
EcalID::get_expanded_id (const Identifier& id,
                         ExpandedIdentifier& exp_id,
                         const IdContext* context) const
{
    exp_id.clear();
    exp_id << calo_field_value()
           << ecal_field_value()
           << row(id)
           << module(id);
    if(!context || context->end_index() == m_PMT_INDEX) 
    {
       	exp_id << pmt(id);
    }
}

int     
EcalID::get_hash        (const Identifier& id, 
                         IdentifierHash& hash_id,
                         const IdContext* context) const
{

    // Get the hash code from either a vec (for module) or calculate
    // it (pmts). For the former, we convert to compact and call
    // get_hash again. For the latter, we calculate the hash from the
    // Identifier.

    int result = 1;
    hash_id = 0;
    size_t begin = (context) ? context->begin_index(): 0;
    size_t end   = (context) ? context->end_index()  : 0; 
    if (0 == begin) {
        // No hashes yet for ids with prefixes
        if (m_MODULE_INDEX  == end) {
            hash_id = module_hash(id);
            if (hash_id.is_valid()) result = 0;
        }
        else if (context && context->end_index() == m_PMT_INDEX) {
            // Must calculate for pmt hash
            ExpandedIdentifier new_id;
            get_expanded_id(id, new_id);
            hash_id =  m_full_pmt_range.cardinalityUpTo(new_id);
            result = 0;
        }
    }
    return (result);
}


void    
EcalID::test_module_packing      (void) const
{
    MsgStream log(m_msgSvc, "EcalID");

    if (m_dict) {
        
        int nids = 0;
        int nerr = 0;
        IdContext context = module_context();
        const_id_iterator first = m_module_vec.begin();
        const_id_iterator last  = m_module_vec.end();
        for (; first != last; ++first, ++nids) {
            Identifier id = (*first);
            ExpandedIdentifier exp_id;
            get_expanded_id(id, exp_id, &context);
            Identifier new_id = module_id(exp_id[m_ROW_INDEX],
                                          exp_id[m_MODULE_INDEX]);
            if (id != new_id) {
                log << MSG::ERROR << "EcalID::test_module_packing: new and old compacts not equal. New/old/expanded ids " 
                    << MSG::hex << show_to_string(id) << " " << show_to_string(new_id) << " " << MSG::dec 
                    << (std::string)exp_id << endmsg;
                nerr++;
                continue;
            }
            // check row id
            if (!exp_id[m_MODULE_INDEX]) {
                
                Identifier new_id1 = row_id(exp_id[m_ROW_INDEX]);
                if (id != new_id1) {
                    log << MSG::ERROR << "EcalID::test_module_packing: new and old row ids not equal. New/old/expanded ids " 
                        << MSG::hex << show_to_string(id) << " " << show_to_string(new_id1) << " " << MSG::dec 
                        << (std::string)exp_id << endmsg;
                    nerr++;
                    continue;
                }
            }
        }

        if (m_msgSvc) { 
            log << MSG::DEBUG << "EcalID::test_module_packing: tested module and row ids. nids, errors " 
                << nids << " " << nerr << endmsg;
        }
        else {
            std::cout << " DEBUG EcalID::test_module_packing: tested module and row ids. nids, errors " 
                      << nids << " " << nerr << std::endl;
        }
        
        nids = 0;
        context = pmt_context();
        const_expanded_id_iterator      first_ecal = pmt_begin();  
        const_expanded_id_iterator      last_ecal  = pmt_end();
        for (; first_ecal != last_ecal; ++first_ecal, ++nids) {
            // if (nids%10000 != 1) continue;
            const ExpandedIdentifier& exp_id = *first_ecal;
            ExpandedIdentifier new_exp_id;

            Identifier id = module_id(exp_id[m_ROW_INDEX],
                                      exp_id[m_MODULE_INDEX]);
            get_expanded_id(id, new_exp_id, &context);
            if (exp_id[0] != new_exp_id[0] ||
                exp_id[1] != new_exp_id[1] ||
                exp_id[2] != new_exp_id[2] ||
                exp_id[3] != new_exp_id[3])
            {
                log << MSG::ERROR << "EcalID::test_module_packing: new and old ids not equal. New/old/compact ids "
                    << (std::string)new_exp_id << " " << (std::string)exp_id
                    << " " << show_to_string(id) << endmsg;
                continue;
            }

            Identifier pmtid	;
	        Identifier pmtid1	;
           	pmtid = pmt_id ( 
                       exp_id[m_ROW_INDEX],
					   exp_id[m_MODULE_INDEX],
					   exp_id[m_PMT_INDEX]);

    	   	pmtid1 = pmt_id (	    
                        row(pmtid),
                        module(pmtid),
                        pmt(pmtid));

            if (pmtid != pmtid1) {
                log << MSG::ERROR << "EcalID::test_module_packing: new and old pmt ids not equal. New/old ids "
                    << " " << show_to_string(pmtid1) << " " 
                    << show_to_string(pmtid) << endmsg;
            }
        }

        if (m_msgSvc) {
            log << MSG::DEBUG << "EcalID::test_module_packing: Successful tested " 
                << nids << " ids. " 
                << endmsg;
        }
        else {
            std::cout << " DEBUG EcalID::test_module_packing: Successful tested " 
                      << nids << " ids. " 
                      << std::endl;
        }
    }
    else {
        log << MSG::ERROR << "EcalID::test_module_packing: Unable to test module packing - no dictionary has been defined. " 
            << endmsg;
    }
}

