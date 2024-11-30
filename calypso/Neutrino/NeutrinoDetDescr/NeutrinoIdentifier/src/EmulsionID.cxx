/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Neutrino identifier package
 -------------------------------------------
***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>
#include "GaudiKernel/MsgStream.h"

#include "NeutrinoIdentifier/EmulsionID.h"
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


EmulsionID::EmulsionID(void)
        :
        m_emulsion_region_index(0),
        m_NEUTRINO_INDEX(0),
        m_EMULSION_INDEX(1),
        m_MODULE_INDEX(2),
        m_BASE_INDEX(3),
        m_FILM_INDEX(4),
        m_dict(0),
        m_base_hash_max(0),
        m_film_hash_max(0)
{
}

void
EmulsionID::base_id_checks ( int module,  
                          int base ) const
{

    // Check that id is within allowed range

    // Fill expanded id
    ExpandedIdentifier id;
    id << neutrino_field_value() << emulsion_field_value()
       << module << base;

    if (!m_full_base_range.match(id)) {  // module range check is sufficient
        MsgStream log(m_msgSvc, "EmulsionID");
        log << MSG::ERROR << " EmulsionID::base_id result is NOT ok. ID, range "
            << (std::string)id <<  " " << (std::string)m_full_base_range << endmsg;
    }
}

void
EmulsionID::film_id_checks ( int module,  
                        int base, 
                        int film) const
{

    // Check that id is within allowed range

    // Fill expanded id
    ExpandedIdentifier id;
    id << neutrino_field_value() << emulsion_field_value()
       << module << base << film;

    if (!m_full_film_range.match(id)) {  
        MsgStream log(m_msgSvc, "EmulsionID");
        log << MSG::ERROR << " EmulsionID::film_id result is NOT ok. ID, range "
            << (std::string)id << " " << (std::string)m_full_film_range << std::endl;
    }
}

int 
EmulsionID::module_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext base_context1 = base_context();
    get_expanded_id(id, expId, &base_context1);
    for (unsigned int i = 0; i < m_full_base_range.size(); ++i) {
        const Range& range = m_full_base_range[i];
        if (range.match(expId)) {
            const Range::field& module_field = range[m_MODULE_INDEX];
            if (module_field.has_maximum()) {
                return (module_field.get_maximum());
            }
        }
    }
    return (-999);  // default
}

int     
EmulsionID::film_max       (const Identifier& id) const
{
    ExpandedIdentifier expId;
    IdContext module_context(expId, 0, m_MODULE_INDEX);
    get_expanded_id(id, expId, &module_context);
    int result = -999;
    for (unsigned int i = 0; i < m_full_film_range.size(); ++i) {
        const Range& range = m_full_film_range[i];
        if (range.match(expId)) {
            const Range::field& film_field = range[m_FILM_INDEX];
            if (film_field.has_maximum()) {
                int film = film_field.get_maximum();
                if (result < film) result = film;
            }
        }
    }
    return (result);
}

int 
EmulsionID::base_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext base_context1 = base_context();
    get_expanded_id(id, expId, &base_context1);
    for (unsigned int i = 0; i < m_full_base_range.size(); ++i) {
        const Range& range = m_full_base_range[i];
        if (range.match(expId)) {
            const Range::field& base_field = range[m_BASE_INDEX];
            if (base_field.has_maximum()) {
                return (base_field.get_maximum());
            }
        }
    }
    return -1;
}

int
EmulsionID::initialize_from_dictionary(const IdDictMgr& dict_mgr)
{
    MsgStream log(m_msgSvc, "EmulsionID");
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
    if (register_dict_tag(dict_mgr, "Neutrino")) return(1);

    m_dict = dict_mgr.find_dictionary ("Neutrino"); 
    if(!m_dict) {
        log << MSG::ERROR << " EmulsionID::initialize_from_dict - cannot access Neutrino dictionary " << endmsg;
        return 1;
    }

    // Initialize the field indices
    if(initLevelsFromDict()) return (1);

    //
    // Build multirange for the valid set of identifiers
    //


    // Find value for the field Neutrino
    const IdDictDictionary* faserDict = dict_mgr.find_dictionary ("FASER"); 
    int neutrinoField   = -1;
    if (faserDict->get_label_value("subdet", "Neutrino", neutrinoField)) {
        log << MSG::ERROR << "Could not get value for label 'Neutrino' of field 'subdet' in dictionary " 
            << faserDict->m_name
            << endmsg;
        return (1);
    }

    // Find value for the field Emulsion
    int emulsionField   = -1;
    if (m_dict->get_label_value("part", "Emulsion", emulsionField)) {
        log << MSG::ERROR << "Could not get value for label 'Emulsion' of field 'part' in dictionary " 
            << m_dict->m_name
            << endmsg;
        return (1);
    }
    if (m_msgSvc) {
        log << MSG::DEBUG << " EmulsionID::initialize_from_dict " 
            << "Found field values: Emulsion "  
            << emulsionField
            << std::endl;
    }
    else {
        std::cout << " DEBUG EmulsionID::initialize_from_dict " 
                  << "Found field values: Emulsion "  
                  << emulsionField
                  << std::endl;
    }
    
    // Set up id for region and range prefix
    ExpandedIdentifier region_id;
    region_id.add(neutrinoField);
    region_id.add(emulsionField);
    Range prefix;
    m_full_base_range = m_dict->build_multirange(region_id, prefix, "base");
    m_full_film_range = m_dict->build_multirange(region_id, prefix);

    // Setup the hash tables
    if(init_hashes()) return (1);

    // Setup hash tables for finding neighbors
    if(init_neighbors()) return (1);
    
    if (m_msgSvc) {
        log << MSG::INFO << " EmulsionID::initialize_from_dict "  << endmsg;
        log << MSG::DEBUG  
            << "Base range -> " << (std::string)m_full_base_range
            <<   endmsg;
        log << MSG::DEBUG
            << "Film range -> " << (std::string)m_full_film_range
            << endmsg;
    }
    else {
        std::cout << " INFO EmulsionID::initialize_from_dict "  << std::endl;
        std::cout << " DEBUG  Base range -> " << (std::string)m_full_base_range
                  <<   std::endl;
        std::cout << " DEBUG Film range -> " << (std::string)m_full_film_range
                  << std::endl;
    }
    
    FaserDetectorID::setDictVersion(dict_mgr, "Neutrino");

    return 0;
}

int
EmulsionID::init_hashes(void)
{

    //
    // create a vector(s) to retrieve the hashes for compact ids. For
    // the moment, we implement a hash for bases but NOT for films
    //
    MsgStream log(m_msgSvc, "EmulsionID");
    // base hash
    m_base_hash_max = m_full_base_range.cardinality();
    m_base_vec.resize(m_base_hash_max);
    unsigned int nids = 0;
    std::set<Identifier> ids;
    for (unsigned int i = 0; i < m_full_base_range.size(); ++i) {
        const Range& range = m_full_base_range[i];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last  = range.factory_end();
        for (; first != last; ++first) {
            const ExpandedIdentifier& exp_id = (*first);
            Identifier id = base_id(exp_id[m_MODULE_INDEX],
                                     exp_id[m_BASE_INDEX]); 
            if(!(ids.insert(id)).second) {
                log << MSG::ERROR << " EmulsionID::init_hashes "
                    << " Error: duplicated id for base id. nid " << nids
                    << " compact id " << id.getString()
                    << " id " << (std::string)exp_id << endmsg;
                return (1);
            }
            nids++;
        }
    }
    if(ids.size() != m_base_hash_max) {
        log << MSG::ERROR << " EmulsionID::init_hashes "
            << " Error: set size NOT EQUAL to hash max. size " << ids.size()
            << " hash max " << m_base_hash_max 
            << endmsg;
        return (1);
    }

    nids = 0;
    std::set<Identifier>::const_iterator first = ids.begin();
    std::set<Identifier>::const_iterator last  = ids.end();
    for (; first != last && nids < m_base_vec.size(); ++first) {
        m_base_vec[nids] = (*first);
        nids++;
    }

    // film hash - we do not keep a vec for the films
    m_film_hash_max = m_full_film_range.cardinality();
    m_film_vec.resize(m_film_hash_max);
    nids = 0;
    ids.clear();
    for (unsigned int i = 0; i < m_full_film_range.size(); ++i) {
        const Range& range = m_full_film_range[i];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last  = range.factory_end();
        for (; first != last; ++first) {
            const ExpandedIdentifier& exp_id = (*first);
            Identifier id = film_id(exp_id[m_MODULE_INDEX],
                                     exp_id[m_BASE_INDEX],
                                     exp_id[m_FILM_INDEX]); 
            if(!(ids.insert(id)).second) {
                log << MSG::ERROR << " EmulsionID::init_hashes "
                    << " Error: duplicated id for film id. nid " << nids
                    << " compact id " << id.getString()
                    << " id " << (std::string)exp_id << endmsg;
                return (1);
            }
            nids++;
        }
    }
    if(ids.size() != m_film_hash_max) {
        log << MSG::ERROR << " EmulsionID::init_hashes "
            << " Error: set size for films NOT EQUAL to hash max. size " << ids.size()
            << " hash max " << m_base_hash_max 
            << endmsg;
        return (1);
    }

    nids = 0;
    first = ids.begin();
    last  = ids.end();
    for (; first != last && nids < m_film_vec.size(); ++first) {
        m_film_vec[nids] = (*first);
        nids++;
    }


    return (0);
}

    int
    EmulsionID::get_prev_in_z(const IdentifierHash& id, IdentifierHash& prev) const
    {
        unsigned short index = id;
        if (index < m_prev_z_base_vec.size())
        {
            if (m_prev_z_base_vec[index] == NOT_VALID_HASH) return (1);
            prev = m_prev_z_base_vec[index];
            return (0);
        }
        return (1);
    }

    int
    EmulsionID::get_next_in_z(const IdentifierHash& id, IdentifierHash& next) const
    {
        unsigned short index = id;
        if (index < m_next_z_base_vec.size())
        {
            if (m_next_z_base_vec[index] == NOT_VALID_HASH) return (1);
            next = m_next_z_base_vec[index];
            return (0);
        }
        return (1);
    }

// int             
// EmulsionID::get_prev_in_phi(const IdentifierHash& id, IdentifierHash& prev) const
// {
//     unsigned short index = id;
//     if (index < m_prev_phi_wafer_vec.size()) {
//         if (m_prev_phi_wafer_vec[index] == NOT_VALID_HASH) return (1);
//         prev =  m_prev_phi_wafer_vec[index];
//         return (0);
//     }
//     return (1);
// }

// int             
// EmulsionID::get_next_in_phi(const IdentifierHash& id, IdentifierHash& next) const
// {
//     unsigned short index = id;
//     if (index < m_next_phi_wafer_vec.size()) {
//         if (m_next_phi_wafer_vec[index] == NOT_VALID_HASH) return (1);
//         next =  m_next_phi_wafer_vec[index];
//         return (0);
//     }
//     return (1);
// }

// int             
// EmulsionID::get_prev_in_eta(const IdentifierHash& id, IdentifierHash& prev) const
// {
//     unsigned short index = id;
//     if (index < m_prev_eta_wafer_vec.size()) {
//         if (m_prev_eta_wafer_vec[index] == NOT_VALID_HASH) return (1);
//         prev =  m_prev_eta_wafer_vec[index];
//         return (0);
//     }
//     return (1);
// }

// int
// EmulsionID::get_next_in_eta(const IdentifierHash& id, IdentifierHash& next) const
// {
//     unsigned short index = id;
//     if (index < m_next_eta_wafer_vec.size()) {
//         if (m_next_eta_wafer_vec[index] == NOT_VALID_HASH) return (1);
//         next =  m_next_eta_wafer_vec[index];
//         return (0);
//     }
//     return (1);
// }

int
EmulsionID::get_other_side  (const IdentifierHash& hashId, IdentifierHash& other) const
{
    if (m_dict) {
        // get max from dictionary
        Identifier id;
        IdContext film_context1 = film_context();
        if(!get_id(hashId, id, &film_context1)) {
            other = film(id) ? hashId - 1 : hashId + 1;
            return (0);
        }
    }
    return (1);
}

int
EmulsionID::init_neighbors(void)
{
    //
    // create a vector(s) to retrieve the hashes for compact ids for
    // base neighbors.
    //
    MsgStream log(m_msgSvc, "EmulsionID");

    m_prev_z_base_vec.resize(m_base_hash_max, NOT_VALID_HASH);
    m_next_z_base_vec.resize(m_base_hash_max, NOT_VALID_HASH);
    for (unsigned int i = 0; i < m_full_base_range.size(); i++)
    {
        const Range& range = m_full_base_range[i];
        const Range::field& module_field = range[m_MODULE_INDEX];
        const Range::field& base_field   = range[m_BASE_INDEX];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last  = range.factory_end();
        for (; first != last; ++first)
        {
            const ExpandedIdentifier& exp_id = (*first);
            ExpandedIdentifier::element_type previous_base;
            ExpandedIdentifier::element_type next_base;
            ExpandedIdentifier::element_type previous_module;
            ExpandedIdentifier::element_type next_module;
            bool pbase = base_field.get_previous(exp_id[m_BASE_INDEX], previous_base);
            bool nbase = base_field.get_next    (exp_id[m_BASE_INDEX], next_base);
            bool pmodule = module_field.get_previous(exp_id[m_MODULE_INDEX], previous_module);
            bool nmodule = module_field.get_next    (exp_id[m_MODULE_INDEX], next_module);

            IdContext  pcontext = base_context();

            IdentifierHash hash_id;
            Identifier originalId = base_id(exp_id[m_MODULE_INDEX],
                                             exp_id[m_BASE_INDEX]);

            if (get_hash(originalId, hash_id, &pcontext)) 
            {
                log << MSG::ERROR << " EmulsionID::init_neighbors - unable to get hash, exp/compact "
                    << show_to_string(originalId, &pcontext)
                    << " " << (std::string)m_full_base_range << endmsg;
                return (1);
            }

            // index for the subsequent arrays
            unsigned short index = hash_id;
            assert (hash_id < m_prev_z_base_vec.size());
            assert (hash_id < m_next_z_base_vec.size());
            
            if (pbase) {
                // Get previous base hash id
                ExpandedIdentifier expId = exp_id;
                expId[m_BASE_INDEX] = previous_base;
                Identifier id = base_id(expId[m_MODULE_INDEX],
                                         expId[m_BASE_INDEX]);

                if (get_hash(id, hash_id, &pcontext)) {
                    log << MSG::ERROR << " EmulsionID::init_neighbors - unable to get previous base hash, exp/compact " << id.getString() << " " 
                        << endmsg;
                    return (1);
                }
                m_prev_z_base_vec[index] = hash_id;
            }
            else if (pmodule)
            {
                ExpandedIdentifier expId = exp_id;
                expId[m_MODULE_INDEX] = previous_module;
                ExpandedIdentifier moduleId;
                moduleId.add(expId[m_NEUTRINO_INDEX]);
                moduleId.add(expId[m_EMULSION_INDEX]);
                moduleId.add(previous_module);
                Range prefix;
                MultiRange moduleBaseRange = m_dict->build_multirange(moduleId, prefix, "base");
                const Range::field& upstream_base_field = range[m_BASE_INDEX];
                if (upstream_base_field.has_maximum())
                {
                    expId[m_BASE_INDEX] = upstream_base_field.get_maximum();
                    Identifier id = base_id(expId[m_MODULE_INDEX],
                                             expId[m_BASE_INDEX]);
                    if (get_hash(id, hash_id, &pcontext)) {
                        log << MSG::ERROR << " EmulsionID::init_neighbors - unable to get last base hash from previous module, exp/compact " << id.getString() << " " 
                            << endmsg;
                        return (1);
                    }
                    m_prev_z_base_vec[index] = hash_id;
                }
                else
                {
                    log << MSG::ERROR << "EmulsionID::init_neighbors - unable to get base_max for previous module, exp/compact " << originalId.getString() << " "
                    << endmsg;
                    return (1);
                }
            }

            if (nbase) {
                // Get next base hash id
                ExpandedIdentifier expId = exp_id;
                expId[m_BASE_INDEX] = next_base;
                Identifier id = base_id(expId[m_MODULE_INDEX],
                                         expId[m_BASE_INDEX]);

                if (get_hash(id, hash_id, &pcontext)) {
                    log << MSG::ERROR << " EmulsionID::init_neighbors - unable to get next base hash, exp/compact " << id.getString() << " " 
                        << endmsg;
                    return (1);
                }
                m_next_z_base_vec[index] = hash_id;
            }
            else if (nmodule)
            {
                ExpandedIdentifier expId = exp_id;
                expId[m_MODULE_INDEX] = next_module;
                ExpandedIdentifier moduleId;
                moduleId.add(expId[m_NEUTRINO_INDEX]);
                moduleId.add(expId[m_EMULSION_INDEX]);
                moduleId.add(next_module);
                Range prefix;
                MultiRange moduleBaseRange = m_dict->build_multirange(moduleId, prefix, "base");
                const Range::field& downstream_base_field = range[m_BASE_INDEX];
                if (downstream_base_field.has_minimum())
                {
                    expId[m_BASE_INDEX] = downstream_base_field.get_minimum();
                    Identifier id = base_id(expId[m_MODULE_INDEX],
                                             expId[m_BASE_INDEX]);
                    if (get_hash(id, hash_id, &pcontext)) {
                        log << MSG::ERROR << " EmulsionID::init_neighbors - unable to get previous base hash from next module, exp/compact " << id.getString() << " " 
                            << endmsg;
                        return (1);
                    }
                    m_next_z_base_vec[index] = hash_id;
                }
                else
                {
                    log << MSG::ERROR << "EmulsionID::init_neighbors - unable to get base_min for next module, exp/compact " << originalId.getString() << " "
                    << endmsg;
                    return (1);
                }
            }

        }
    }

    // m_prev_phi_wafer_vec.resize(m_wafer_hash_max, NOT_VALID_HASH);
    // m_next_phi_wafer_vec.resize(m_wafer_hash_max, NOT_VALID_HASH);
    // m_prev_eta_wafer_vec.resize(m_wafer_hash_max, NOT_VALID_HASH);
    // m_next_eta_wafer_vec.resize(m_wafer_hash_max, NOT_VALID_HASH);

    // for (unsigned int i = 0; i < m_full_wafer_range.size(); ++i) {
    //     const Range& range = m_full_wafer_range[i];
    //     const Range::field& phi_field = range[m_PHI_MODULE_INDEX];
    //     const Range::field& eta_field = range[m_ETA_MODULE_INDEX];
    //     Range::const_identifier_factory first = range.factory_begin();
    //     Range::const_identifier_factory last  = range.factory_end();
    //     for (; first != last; ++first) {
    //         const ExpandedIdentifier& exp_id = (*first);
    //         ExpandedIdentifier::element_type previous_phi;
    //         ExpandedIdentifier::element_type next_phi;
    //         ExpandedIdentifier::element_type previous_eta;
    //         ExpandedIdentifier::element_type next_eta;
    //         bool pphi = phi_field.get_previous(exp_id[m_PHI_MODULE_INDEX], previous_phi);
    //         bool nphi = phi_field.get_next    (exp_id[m_PHI_MODULE_INDEX], next_phi);
    //         bool peta = eta_field.get_previous(exp_id[m_ETA_MODULE_INDEX], previous_eta);
    //         bool neta = eta_field.get_next    (exp_id[m_ETA_MODULE_INDEX], next_eta);

    //         IdContext      wcontext = wafer_context();
            
    //         // First get primary hash id
    //         IdentifierHash hash_id;
    //         Identifier id = wafer_id(exp_id[m_BARREL_EC_INDEX],
    //                                  exp_id[m_LAYER_DISK_INDEX], 
    //                                  exp_id[m_PHI_MODULE_INDEX],
    //                                  exp_id[m_ETA_MODULE_INDEX],
    //                                  exp_id[m_SIDE_INDEX]);
    //         if (get_hash(id, hash_id, &wcontext)) {
    //             log << MSG::ERROR << " EmulsionID::init_neighbors - unable to get hash, exp/compact "
    //                 << show_to_string(id, &wcontext)
    //                 << " " << (std::string)m_full_wafer_range << endmsg;
    //             return (1);
    //         }

    //         // index for the subsequent arrays
    //         unsigned short index = hash_id;
    //         assert (hash_id < m_prev_phi_wafer_vec.size());
    //         assert (hash_id < m_next_phi_wafer_vec.size());
    //         assert (hash_id < m_prev_eta_wafer_vec.size());
    //         assert (hash_id < m_next_eta_wafer_vec.size());
            
    //         if (pphi) {
    //             // Get previous phi hash id
    //             ExpandedIdentifier expId = exp_id;
    //             expId[m_PHI_MODULE_INDEX] = previous_phi;
    //             Identifier id = wafer_id(expId[m_BARREL_EC_INDEX],
    //                                      expId[m_LAYER_DISK_INDEX], 
    //                                      expId[m_PHI_MODULE_INDEX],
    //                                      expId[m_ETA_MODULE_INDEX],
    //                                      expId[m_SIDE_INDEX]);
    //             if (get_hash(id, hash_id, &wcontext)) {
    //                 log << MSG::ERROR << " EmulsionID::init_neighbors - unable to get previous phi hash, exp/compact " << id.getString() << " " 
    //                     << endmsg;
    //                 return (1);
    //             }
    //             m_prev_phi_wafer_vec[index] = hash_id;
    //         }
            
    //         if (nphi) {
    //             // Get next phi hash id
    //             ExpandedIdentifier expId = exp_id;
    //             expId[m_PHI_MODULE_INDEX] = next_phi;
    //             Identifier id = wafer_id(expId[m_BARREL_EC_INDEX],
    //                                      expId[m_LAYER_DISK_INDEX], 
    //                                      expId[m_PHI_MODULE_INDEX],
    //                                      expId[m_ETA_MODULE_INDEX],
    //                                      expId[m_SIDE_INDEX]);
    //             if (get_hash(id, hash_id, &wcontext)) {
    //                 log << MSG::ERROR << " EmulsionID::init_neighbors - unable to get next phi hash, exp/compact " << id.getString() << 
    //                     " " << MSG::hex << id.getString() << MSG::dec << endmsg;
    //                 return (1);
    //             }
    //             m_next_phi_wafer_vec[index] = hash_id;
    //         }
            
    //         if (peta) {
    //             // Get previous eta hash id
    //             ExpandedIdentifier expId = exp_id;
    //             expId[m_ETA_MODULE_INDEX] = previous_eta;
    //             Identifier id = wafer_id(expId[m_BARREL_EC_INDEX],
    //                                      expId[m_LAYER_DISK_INDEX], 
    //                                      expId[m_PHI_MODULE_INDEX],
    //                                      expId[m_ETA_MODULE_INDEX],
    //                                      expId[m_SIDE_INDEX]);
    //             if (get_hash(id, hash_id, &wcontext)) {
    //                 log << MSG::ERROR << " EmulsionID::init_neighbors - unable to get previous eta hash, exp/compact " << id.getString() 
    //                     << " " << std::endl;
    //                 return (1);
    //             }
    //             m_prev_eta_wafer_vec[index] = hash_id;
    //         }
            
    //         if (neta) {
    //             // Get next eta hash id
    //             ExpandedIdentifier expId = exp_id;
    //             expId[m_ETA_MODULE_INDEX] = next_eta;
    //             Identifier id = wafer_id(expId[m_BARREL_EC_INDEX],
    //                                      expId[m_LAYER_DISK_INDEX], 
    //                                      expId[m_PHI_MODULE_INDEX],
    //                                      expId[m_ETA_MODULE_INDEX],
    //                                      expId[m_SIDE_INDEX]);
    //             if (get_hash(id, hash_id, &wcontext)) {
    //                 log << MSG::ERROR << " EmulsionID::init_neighbors - unable to get next eta hash, exp/compact " << id.getString() 
    //                     << " " << endmsg;
    //                 return (1);
    //             }
    //             m_next_eta_wafer_vec[index] = hash_id;
    //         }
            

//          std::cout << " EmulsionID::init_neighbors "
//                    << " phi, previous, next " << id[m_PHI_MODULE_INDEX]
//                    << " " << pphi
//                    << " " << previous_phi
//                    << " " << nphi
//                    << " " << next_phi
//                    << " eta, previous, next " << id[m_ETA_MODULE_INDEX]
//                    << " " << peta
//                    << " " << previous_eta
//                    << " " << neta
//                    << " " << next_eta
//                    << " id " << (std::string)(*first) 
//                    << std::endl;
    //     }
    // }
    return (0);
}



int     
EmulsionID::initLevelsFromDict()
{


    MsgStream log(m_msgSvc, "EmulsionID");
    if(!m_dict) {
        log << MSG::ERROR << " EmulsionID::initLevelsFromDict - dictionary NOT initialized " << endmsg;
        return (1);
    }
    
    // Find out which identifier field corresponds to each level. Use
    // names to find each field/leve.

    m_NEUTRINO_INDEX               = 999;
    m_EMULSION_INDEX                = 999;
    m_MODULE_INDEX             = 999;
    m_BASE_INDEX               = 999;
    m_FILM_INDEX                 = 999;    

    // Save index to a Emulsion region for unpacking
    ExpandedIdentifier id; 
    id << neutrino_field_value() << emulsion_field_value();
    if (m_dict->find_region(id, m_emulsion_region_index)) {
        log << MSG::ERROR << "EmulsionID::initLevelsFromDict - unable to find emulsion region index: id, reg "  
            << (std::string)id << " " << m_emulsion_region_index
            << endmsg;
        return (1);
    }

    // Find a Emulsion region
    IdDictField* field = m_dict->find_field("subdet");
    if (field) {
        m_NEUTRINO_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "EmulsionID::initLevelsFromDict - unable to find 'subdet' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("part");
    if (field) {
        m_EMULSION_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "EmulsionID::initLevelsFromDict - unable to find 'part' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("module");
    if (field) {
        m_MODULE_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "EmulsionID::initLevelsFromDict - unable to find 'module' field "   << endmsg;
        return (1);
    }
    field = m_dict->find_field("base");
    if (field) {
        m_BASE_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR<< "EmulsionID::initLevelsFromDict - unable to find 'base' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("film");
    if (field) {
        m_FILM_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "EmulsionID::initLevelsFromDict - unable to find 'film' field " << endmsg;    
        return (1);
    }
    
    // Set the field implementations

    const IdDictRegion& region = *m_dict->m_regions[m_emulsion_region_index];

    m_neutrino_impl      = region.m_implementation[m_NEUTRINO_INDEX]; 
    m_emulsion_impl       = region.m_implementation[m_EMULSION_INDEX]; 
    m_module_impl    = region.m_implementation[m_MODULE_INDEX]; 
    m_base_impl      = region.m_implementation[m_BASE_INDEX];
    m_film_impl        = region.m_implementation[m_FILM_INDEX]; 

    if (m_msgSvc) {
        log << MSG::DEBUG << "decode index and bit fields for each level: " << endmsg;
        log << MSG::DEBUG << "neutrino    "  << m_neutrino_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << "emulsion     "  << m_emulsion_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "module  "  << m_module_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "base    "  << m_base_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "film      "  << m_film_impl.show_to_string() << endmsg; 
    }
    else {
        std::cout << " DEBUG decode index and bit fields for each level: " << std::endl;
        std::cout << " DEBUG neutrino    "  << m_neutrino_impl.show_to_string() << std::endl;
        std::cout << " DEBUG emulsion     "  << m_emulsion_impl.show_to_string() << std::endl; 
        std::cout << " DEBUG module  "  << m_module_impl.show_to_string() << std::endl; 
        std::cout << " DEBUG base    "  << m_base_impl.show_to_string() << std::endl;
        std::cout << " DEBUG film      "  << m_film_impl.show_to_string() << std::endl; 
    }
    
    std::cout << "neutrino "  << m_neutrino_impl.decode_index() << " " 
              <<   (std::string)m_neutrino_impl.ored_field() << " " 
              << std::hex    << m_neutrino_impl.mask() << " " 
              << m_neutrino_impl.zeroing_mask() << " " 
              << std::dec    << m_neutrino_impl.shift() << " "
              << m_neutrino_impl.bits() << " "
              << m_neutrino_impl.bits_offset()
              << std::endl;
    std::cout << "emulsion"     << m_emulsion_impl.decode_index() << " " 
              <<   (std::string)m_emulsion_impl.ored_field() << " " 
              << std::hex    << m_emulsion_impl.mask() << " " 
              << m_emulsion_impl.zeroing_mask() << " " 
              << std::dec    << m_emulsion_impl.shift() << " "
              << m_emulsion_impl.bits() << " "
              << m_emulsion_impl.bits_offset()
              << std::endl;
    std::cout << "module"<< m_module_impl.decode_index() << " " 
              <<   (std::string)m_module_impl.ored_field() << " " 
              << std::hex    << m_module_impl.mask() << " " 
              << m_module_impl.zeroing_mask() << " " 
              << std::dec    << m_module_impl.shift() << " "
              << m_module_impl.bits() << " "
              << m_module_impl.bits_offset()
              << std::endl;
    std::cout << "base"    << m_base_impl.decode_index() << " " 
              <<   (std::string)m_base_impl.ored_field() << " " 
              << std::hex    << m_base_impl.mask() << " " 
              << m_base_impl.zeroing_mask() << " " 
              << std::dec    << m_base_impl.shift() << " "
              << m_base_impl.bits() << " "
              << m_base_impl.bits_offset()
              << std::endl;
    std::cout << "film"   << m_film_impl.decode_index() << " " 
              <<   (std::string)m_film_impl.ored_field() << " " 
              << std::hex    << m_film_impl.mask() << " " 
              << m_film_impl.zeroing_mask() << " " 
              << std::dec    << m_film_impl.shift() << " "
              << m_film_impl.bits() << " "
              << m_film_impl.bits_offset()
              << std::endl;

    return (0);
}

EmulsionID::size_type       
EmulsionID::base_hash_max (void) const
{
    return m_base_hash_max;
}

EmulsionID::size_type       
EmulsionID::film_hash_max (void) const
{
    return m_film_hash_max;
}

EmulsionID::const_id_iterator       EmulsionID::base_begin             (void) const
{
    return (m_base_vec.begin());
}

EmulsionID::const_id_iterator       EmulsionID::base_end               (void) const
{
    return (m_base_vec.end());
}

EmulsionID::const_expanded_id_iterator      EmulsionID::film_begin     (void) const
{
    return (m_full_film_range.factory_begin());
}

EmulsionID::const_expanded_id_iterator      EmulsionID::film_end       (void) const
{
    return (m_full_film_range.factory_end());
}

// From hash get Identifier
int     
EmulsionID::get_id          (const IdentifierHash& hash_id,
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
        if (m_BASE_INDEX == end) {
            if (hash_id < (unsigned int)(m_base_vec.end() - m_base_vec.begin())) {
                id = m_base_vec[hash_id];
                result = 0;
            }
        }
        else if (m_FILM_INDEX == end) {
            // Do not know how to calculate strip id from hash yet!!
            std::cout << "Do not know how to calculate film id from hash yet!!" << std::endl;
        }
    }
    return (result);
}

void
EmulsionID::get_expanded_id (const Identifier& id,
                         ExpandedIdentifier& exp_id,
                         const IdContext* context) const
{
    exp_id.clear();
    exp_id << neutrino_field_value()
           << emulsion_field_value()
           << module(id)
           << base(id);
    if(!context || context->end_index() == m_FILM_INDEX) 
    {
       	exp_id << film(id);
    }
}

int     
EmulsionID::get_hash        (const Identifier& id, 
                         IdentifierHash& hash_id,
                         const IdContext* context) const
{

    // Get the hash code from either a vec (for base) or calculate
    // it (films). For the former, we convert to compact and call
    // get_hash again. For the latter, we calculate the hash from the
    // Identifier.

    int result = 1;
    hash_id = 0;
    size_t begin = (context) ? context->begin_index(): 0;
    size_t end   = (context) ? context->end_index()  : 0; 
    if (0 == begin) {
        // No hashes yet for ids with prefixes
        if (m_BASE_INDEX  == end) {
            hash_id = base_hash(id);
            if (hash_id.is_valid()) result = 0;
        }
        else if (context && context->end_index() == m_FILM_INDEX) {
            // Must calculate for strip hash
            ExpandedIdentifier new_id;
            get_expanded_id(id, new_id);
            hash_id =  m_full_film_range.cardinalityUpTo(new_id);
            result = 0;
        }
    }
    return (result);
}


void    
EmulsionID::test_base_packing      (void) const
{
    MsgStream log(m_msgSvc, "EmulsionID");

    if (m_dict) {
        
        int nids = 0;
        int nerr = 0;
        IdContext context = base_context();
        const_id_iterator first = m_base_vec.begin();
        const_id_iterator last  = m_base_vec.end();
        for (; first != last; ++first, ++nids) {
            Identifier id = (*first);
            ExpandedIdentifier exp_id;
            get_expanded_id(id, exp_id, &context);
            Identifier new_id = base_id(exp_id[m_MODULE_INDEX],
                                         exp_id[m_BASE_INDEX]);
            if (id != new_id) {
                log << MSG::ERROR << "EmulsionID::test_base_packing: new and old compacts not equal. New/old/expanded ids " 
                    << MSG::hex << show_to_string(id) << " " << show_to_string(new_id) << " " << MSG::dec 
                    << (std::string)exp_id << endmsg;
                nerr++;
                continue;
            }
            // check module id
            if (!exp_id[m_BASE_INDEX]) {
                
                Identifier new_id1 = module_id(exp_id[m_MODULE_INDEX]);
                if (id != new_id1) {
                    log << MSG::ERROR << "EmulsionID::test_base_packing: new and old module ids not equal. New/old/expanded ids " 
                        << MSG::hex << show_to_string(id) << " " << show_to_string(new_id1) << " " << MSG::dec 
                        << (std::string)exp_id << endmsg;
                    nerr++;
                    continue;
                }
            }
        }

        if (m_msgSvc) { 
            log << MSG::DEBUG << "EmulsionID::test_base_packing: tested base and module ids. nids, errors " 
                << nids << " " << nerr << endmsg;
        }
        else {
            std::cout << " DEBUG EmulsionID::test_base_packing: tested base and module ids. nids, errors " 
                      << nids << " " << nerr << std::endl;
        }
        
        nids = 0;
        context = film_context();
        const_expanded_id_iterator      first_emulsion = film_begin();  
        const_expanded_id_iterator      last_emulsion  = film_end();
        for (; first_emulsion != last_emulsion; ++first_emulsion, ++nids) {
            // if (nids%10000 != 1) continue;
            const ExpandedIdentifier& exp_id = *first_emulsion;
            ExpandedIdentifier new_exp_id;

            Identifier id = base_id(exp_id[m_MODULE_INDEX],
                                     exp_id[m_BASE_INDEX]);
            get_expanded_id(id, new_exp_id, &context);
            if (exp_id[0] != new_exp_id[0] ||
                exp_id[1] != new_exp_id[1] ||
                exp_id[2] != new_exp_id[2] ||
                exp_id[3] != new_exp_id[3])
            {
                log << MSG::ERROR << "EmulsionID::test_base_packing: new and old ids not equal. New/old/compact ids "
                    << (std::string)new_exp_id << " " << (std::string)exp_id
                    << " " << show_to_string(id) << endmsg;
                continue;
            }

            Identifier filmid	;
	        Identifier filmid1	;
           	filmid = film_id ( 
                       exp_id[m_MODULE_INDEX],
					   exp_id[m_BASE_INDEX],
					   exp_id[m_FILM_INDEX]);

    	   	filmid1 = film_id (	    
                        module(filmid),
                        base(filmid),
                        film(filmid));

            if (filmid != filmid1) {
                log << MSG::ERROR << "EmulsionID::test_base_packing: new and old pixel ids not equal. New/old ids "
                    << " " << show_to_string(filmid1) << " " 
                    << show_to_string(filmid) << endmsg;
            }
        }

        if (m_msgSvc) {
            log << MSG::DEBUG << "EmulsionID::test_base_packing: Successful tested " 
                << nids << " ids. " 
                << endmsg;
        }
        else {
            std::cout << " DEBUG EmulsionID::test_base_packing: Successful tested " 
                      << nids << " ids. " 
                      << std::endl;
        }
    }
    else {
        log << MSG::ERROR << "EmulsionID::test_base_packing: Unable to test base packing - no dictionary has been defined. " 
            << endmsg;
    }
}

