/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Scintillator identifier package
 -------------------------------------------
***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>
#include "GaudiKernel/MsgStream.h"

#include "ScintIdentifier/VetoNuID.h"
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


VetoNuID::VetoNuID(void)
        :
        m_vetonu_region_index(0),
        m_SCINT_INDEX(0),
        m_VETONU_INDEX(1),
        m_STATION_INDEX(2),
        m_PLATE_INDEX(3),
        m_PMT_INDEX(4),
        m_dict(0),
        m_plate_hash_max(0),
        m_pmt_hash_max(0)
{
}

void
VetoNuID::plate_id_checks ( int station,  
                          int plate ) const
{

    // Check that id is within allowed range

    // Fill expanded id
    ExpandedIdentifier id;
    id << scint_field_value() << vetonu_field_value()
       << station << plate;

    if (!m_full_plate_range.match(id)) {  // module range check is sufficient
        MsgStream log(m_msgSvc, "VetoNuID");
        log << MSG::ERROR << " VetoNuID::plate_id result is NOT ok. ID, range "
            << (std::string)id <<  " " << (std::string)m_full_plate_range << endmsg;
    }
}

void
VetoNuID::pmt_id_checks ( int station,  
                        int plate, 
                        int pmt) const
{

    // Check that id is within allowed range

    // Fill expanded id
    ExpandedIdentifier id;
    id << scint_field_value() << vetonu_field_value()
       << station << plate << pmt;

    if (!m_full_pmt_range.match(id)) {  
        MsgStream log(m_msgSvc, "VetoNuID");
        log << MSG::ERROR << " VetoNuID::pmt_id result is NOT ok. ID, range "
            << (std::string)id << " " << (std::string)m_full_pmt_range << std::endl;
    }
}

int 
VetoNuID::station_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext plate_context1 = plate_context();
    get_expanded_id(id, expId, &plate_context1);
    for (unsigned int i = 0; i < m_full_plate_range.size(); ++i) {
        const Range& range = m_full_plate_range[i];
        if (range.match(expId)) {
            const Range::field& station_field = range[m_STATION_INDEX];
            if (station_field.has_maximum()) {
                return (station_field.get_maximum());
            }
        }
    }
    return (-999);  // default
}

int     
VetoNuID::pmt_max       (const Identifier& id) const
{
    ExpandedIdentifier expId;
    IdContext station_context(expId, 0, m_STATION_INDEX);
    get_expanded_id(id, expId, &station_context);
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
VetoNuID::plate_max(const Identifier& id) const
{
    // get max from dictionary
    ExpandedIdentifier expId;
    IdContext plate_context1 = plate_context();
    get_expanded_id(id, expId, &plate_context1);
    for (unsigned int i = 0; i < m_full_plate_range.size(); ++i) {
        const Range& range = m_full_plate_range[i];
        if (range.match(expId)) {
            const Range::field& plate_field = range[m_PLATE_INDEX];
            if (plate_field.has_maximum()) {
                return (plate_field.get_maximum());
            }
        }
    }
    return -1;
}

int
VetoNuID::initialize_from_dictionary(const IdDictMgr& dict_mgr)
{
    MsgStream log(m_msgSvc, "VetoNuID");
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
    if (register_dict_tag(dict_mgr, "Scintillator")) return(1);

    m_dict = dict_mgr.find_dictionary ("Scintillator"); 
    if(!m_dict) {
        log << MSG::ERROR << " VetoNuID::initialize_from_dict - cannot access Scintillator dictionary " << endmsg;
        return 1;
    }

    // Initialize the field indices
    if(initLevelsFromDict()) return (1);

    //
    // Build multirange for the valid set of identifiers
    //


    // Find value for the field Scintillator
    const IdDictDictionary* faserDict = dict_mgr.find_dictionary ("FASER"); 
    int scintField   = -1;
    if (faserDict->get_label_value("subdet", "Scintillator", scintField)) {
        log << MSG::ERROR << "Could not get value for label 'Scintillator' of field 'subdet' in dictionary " 
            << faserDict->m_name
            << endmsg;
        return (1);
    }

    // Find value for the field VetoNu
    int vetonuField   = -1;
    if (m_dict->get_label_value("part", "VetoNu", vetonuField)) {
        log << MSG::ERROR << "Could not get value for label 'VetoNu' of field 'part' in dictionary " 
            << m_dict->m_name
            << endmsg;
        return (1);
    }
    if (m_msgSvc) {
        log << MSG::DEBUG << " VetoNuID::initialize_from_dict " 
            << "Found field values: VetoNu "  
            << vetonuField
            << std::endl;
    }
    else {
        std::cout << " DEBUG VetoNuID::initialize_from_dict " 
                  << "Found field values: VetoNu "  
                  << vetonuField
                  << std::endl;
    }
    
    // Set up id for region and range prefix
    ExpandedIdentifier region_id;
    region_id.add(scintField);
    region_id.add(vetonuField);
    Range prefix;
    m_full_plate_range = m_dict->build_multirange(region_id, prefix, "plate");
    m_full_pmt_range = m_dict->build_multirange(region_id, prefix);

    // Setup the hash tables
    if(init_hashes()) return (1);

    // Setup hash tables for finding neighbors
    if(init_neighbors()) return (1);
    
    if (m_msgSvc) {
        log << MSG::INFO << " VetoNuID::initialize_from_dict "  << endmsg;
        log << MSG::DEBUG  
            << "Plate range -> " << (std::string)m_full_plate_range
            <<   endmsg;
        log << MSG::DEBUG
            << "Pmt range -> " << (std::string)m_full_pmt_range
            << endmsg;
    }
    else {
        std::cout << " INFO VetoNuID::initialize_from_dict "  << std::endl;
        std::cout << " DEBUG  Plate range -> " << (std::string)m_full_plate_range
                  <<   std::endl;
        std::cout << " DEBUG Pmt range -> " << (std::string)m_full_pmt_range
                  << std::endl;
    }
    
    return 0;
}

int
VetoNuID::init_hashes(void)
{

    //
    // create a vector(s) to retrieve the hashes for compact ids. For
    // the moment, we implement a hash for plates but NOT for pmts
    //
    MsgStream log(m_msgSvc, "VetoNuID");
    // plate hash
    m_plate_hash_max = m_full_plate_range.cardinality();
    m_plate_vec.resize(m_plate_hash_max);
    unsigned int nids = 0;
    std::set<Identifier> ids;
    for (unsigned int i = 0; i < m_full_plate_range.size(); ++i) {
        const Range& range = m_full_plate_range[i];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last  = range.factory_end();
        for (; first != last; ++first) {
            const ExpandedIdentifier& exp_id = (*first);
            Identifier id = plate_id(exp_id[m_STATION_INDEX],
                                     exp_id[m_PLATE_INDEX]); 
            if(!(ids.insert(id)).second) {
                log << MSG::ERROR << " VetoNuID::init_hashes "
                    << " Error: duplicated id for plate id. nid " << nids
                    << " compact id " << id.getString()
                    << " id " << (std::string)exp_id << endmsg;
                return (1);
            }
            nids++;
        }
    }
    if(ids.size() != m_plate_hash_max) {
        log << MSG::ERROR << " VetoNuID::init_hashes "
            << " Error: set size NOT EQUAL to hash max. size " << ids.size()
            << " hash max " << m_plate_hash_max 
            << endmsg;
        return (1);
    }

    nids = 0;
    std::set<Identifier>::const_iterator first = ids.begin();
    std::set<Identifier>::const_iterator last  = ids.end();
    for (; first != last && nids < m_plate_vec.size(); ++first) {
        m_plate_vec[nids] = (*first);
        nids++;
    }

    // pmt hash - we do not keep a vec for the pmts
    m_pmt_hash_max = m_full_pmt_range.cardinality();

    return (0);
}

    int
    VetoNuID::get_prev_in_z(const IdentifierHash& id, IdentifierHash& prev) const
    {
        unsigned short index = id;
        if (index < m_prev_z_plate_vec.size())
        {
            if (m_prev_z_plate_vec[index] == NOT_VALID_HASH) return (1);
            prev = m_prev_z_plate_vec[index];
            return (0);
        }
        return (1);
    }

    int
    VetoNuID::get_next_in_z(const IdentifierHash& id, IdentifierHash& next) const
    {
        unsigned short index = id;
        if (index < m_next_z_plate_vec.size())
        {
            if (m_next_z_plate_vec[index] == NOT_VALID_HASH) return (1);
            next = m_next_z_plate_vec[index];
            return (0);
        }
        return (1);
    }

// int             
// VetoNuID::get_prev_in_phi(const IdentifierHash& id, IdentifierHash& prev) const
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
// VetoNuID::get_next_in_phi(const IdentifierHash& id, IdentifierHash& next) const
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
// VetoNuID::get_prev_in_eta(const IdentifierHash& id, IdentifierHash& prev) const
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
// VetoNuID::get_next_in_eta(const IdentifierHash& id, IdentifierHash& next) const
// {
//     unsigned short index = id;
//     if (index < m_next_eta_wafer_vec.size()) {
//         if (m_next_eta_wafer_vec[index] == NOT_VALID_HASH) return (1);
//         next =  m_next_eta_wafer_vec[index];
//         return (0);
//     }
//     return (1);
// }

// int
// VetoNuID::get_other_side  (const IdentifierHash& hashId, IdentifierHash& other) const
// {
//     if (m_dict) {
//         // get max from dictionary
//         Identifier id;
//         IdContext wafer_context1 = wafer_context();
//         if(!get_id(hashId, id, &wafer_context1)) {
//             other = side(id) ? hashId - 1 : hashId + 1;
//             return (0);
//         }
//     }
//     return (1);
// }

int
VetoNuID::init_neighbors(void)
{
    //
    // create a vector(s) to retrieve the hashes for compact ids for
    // plate neighbors.
    //
    MsgStream log(m_msgSvc, "VetoNuID");

    m_prev_z_plate_vec.resize(m_plate_hash_max, NOT_VALID_HASH);
    m_next_z_plate_vec.resize(m_plate_hash_max, NOT_VALID_HASH);
    for (unsigned int i = 0; i < m_full_plate_range.size(); i++)
    {
        const Range& range = m_full_plate_range[i];
        const Range::field& station_field = range[m_STATION_INDEX];
        const Range::field& plate_field   = range[m_PLATE_INDEX];
        Range::const_identifier_factory first = range.factory_begin();
        Range::const_identifier_factory last  = range.factory_end();
        for (; first != last; ++first)
        {
            const ExpandedIdentifier& exp_id = (*first);
            ExpandedIdentifier::element_type previous_plate;
            ExpandedIdentifier::element_type next_plate;
            ExpandedIdentifier::element_type previous_station;
            ExpandedIdentifier::element_type next_station;
            bool pplate = plate_field.get_previous(exp_id[m_PLATE_INDEX], previous_plate);
            bool nplate = plate_field.get_next    (exp_id[m_PLATE_INDEX], next_plate);
            bool pstation = station_field.get_previous(exp_id[m_STATION_INDEX], previous_station);
            bool nstation = station_field.get_next    (exp_id[m_STATION_INDEX], next_station);

            IdContext  pcontext = plate_context();

            IdentifierHash hash_id;
            Identifier originalId = plate_id(exp_id[m_STATION_INDEX],
                                             exp_id[m_PLATE_INDEX]);

            if (get_hash(originalId, hash_id, &pcontext)) 
            {
                log << MSG::ERROR << " VetoNuID::init_neighbors - unable to get hash, exp/compact "
                    << show_to_string(originalId, &pcontext)
                    << " " << (std::string)m_full_plate_range << endmsg;
                return (1);
            }

            // index for the subsequent arrays
            unsigned short index = hash_id;
            assert (hash_id < m_prev_z_plate_vec.size());
            assert (hash_id < m_next_z_plate_vec.size());
            
            if (pplate) {
                // Get previous plate hash id
                ExpandedIdentifier expId = exp_id;
                expId[m_PLATE_INDEX] = previous_plate;
                Identifier id = plate_id(expId[m_STATION_INDEX],
                                         expId[m_PLATE_INDEX]);

                if (get_hash(id, hash_id, &pcontext)) {
                    log << MSG::ERROR << " VetoNuID::init_neighbors - unable to get previous plate hash, exp/compact " << id.getString() << " " 
                        << endmsg;
                    return (1);
                }
                m_prev_z_plate_vec[index] = hash_id;
            }
            else if (pstation)
            {
                ExpandedIdentifier expId = exp_id;
                expId[m_STATION_INDEX] = previous_station;
                ExpandedIdentifier stationId;
                stationId.add(expId[m_SCINT_INDEX]);
                stationId.add(expId[m_VETONU_INDEX]);
                stationId.add(previous_station);
                Range prefix;
                MultiRange stationPlateRange = m_dict->build_multirange(stationId, prefix, "plate");
                const Range::field& upstream_plate_field = range[m_PLATE_INDEX];
                if (upstream_plate_field.has_maximum())
                {
                    expId[m_PLATE_INDEX] = upstream_plate_field.get_maximum();
                    Identifier id = plate_id(expId[m_STATION_INDEX],
                                             expId[m_PLATE_INDEX]);
                    if (get_hash(id, hash_id, &pcontext)) {
                        log << MSG::ERROR << " VetoNuID::init_neighbors - unable to get last plate hash from previous station, exp/compact " << id.getString() << " " 
                            << endmsg;
                        return (1);
                    }
                    m_prev_z_plate_vec[index] = hash_id;
                }
                else
                {
                    log << MSG::ERROR << "VetoNuID::init_neighbors - unable to get plate_max for previous station, exp/compact " << originalId.getString() << " "
                    << endmsg;
                    return (1);
                }
            }

            if (nplate) {
                // Get next plate hash id
                ExpandedIdentifier expId = exp_id;
                expId[m_PLATE_INDEX] = next_plate;
                Identifier id = plate_id(expId[m_STATION_INDEX],
                                         expId[m_PLATE_INDEX]);

                if (get_hash(id, hash_id, &pcontext)) {
                    log << MSG::ERROR << " VetoNuID::init_neighbors - unable to get next plate hash, exp/compact " << id.getString() << " " 
                        << endmsg;
                    return (1);
                }
                m_next_z_plate_vec[index] = hash_id;
            }
            else if (nstation)
            {
                ExpandedIdentifier expId = exp_id;
                expId[m_STATION_INDEX] = next_station;
                ExpandedIdentifier stationId;
                stationId.add(expId[m_SCINT_INDEX]);
                stationId.add(expId[m_VETONU_INDEX]);
                stationId.add(next_station);
                Range prefix;
                MultiRange stationPlateRange = m_dict->build_multirange(stationId, prefix, "plate");
                const Range::field& downstream_plate_field = range[m_PLATE_INDEX];
                if (downstream_plate_field.has_minimum())
                {
                    expId[m_PLATE_INDEX] = downstream_plate_field.get_minimum();
                    Identifier id = plate_id(expId[m_STATION_INDEX],
                                             expId[m_PLATE_INDEX]);
                    if (get_hash(id, hash_id, &pcontext)) {
                        log << MSG::ERROR << " VetoNuID::init_neighbors - unable to get previous plate hash from next station, exp/compact " << id.getString() << " " 
                            << endmsg;
                        return (1);
                    }
                    m_next_z_plate_vec[index] = hash_id;
                }
                else
                {
                    log << MSG::ERROR << "VetoNuID::init_neighbors - unable to get plate_min for next station, exp/compact " << originalId.getString() << " "
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
    //             log << MSG::ERROR << " VetoNuID::init_neighbors - unable to get hash, exp/compact "
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
    //                 log << MSG::ERROR << " VetoNuID::init_neighbors - unable to get previous phi hash, exp/compact " << id.getString() << " " 
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
    //                 log << MSG::ERROR << " VetoNuID::init_neighbors - unable to get next phi hash, exp/compact " << id.getString() << 
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
    //                 log << MSG::ERROR << " VetoNuID::init_neighbors - unable to get previous eta hash, exp/compact " << id.getString() 
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
    //                 log << MSG::ERROR << " VetoNuID::init_neighbors - unable to get next eta hash, exp/compact " << id.getString() 
    //                     << " " << endmsg;
    //                 return (1);
    //             }
    //             m_next_eta_wafer_vec[index] = hash_id;
    //         }
            

//          std::cout << " VetoNuID::init_neighbors "
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
VetoNuID::initLevelsFromDict()
{


    MsgStream log(m_msgSvc, "VetoNuID");
    if(!m_dict) {
        log << MSG::ERROR << " VetoNuID::initLevelsFromDict - dictionary NOT initialized " << endmsg;
        return (1);
    }
    
    // Find out which identifier field corresponds to each level. Use
    // names to find each field/leve.

    m_SCINT_INDEX               = 999;
    m_VETONU_INDEX                = 999;
    m_STATION_INDEX             = 999;
    m_PLATE_INDEX               = 999;
    m_PMT_INDEX                 = 999;    

    // Save index to a VetoNu region for unpacking
    ExpandedIdentifier id; 
    id << scint_field_value() << vetonu_field_value();
    if (m_dict->find_region(id, m_vetonu_region_index)) {
        log << MSG::ERROR << "VetoNuID::initLevelsFromDict - unable to find vetonu region index: id, reg "  
            << (std::string)id << " " << m_vetonu_region_index
            << endmsg;
        return (1);
    }

    // Find a VetoNu region
    IdDictField* field = m_dict->find_field("subdet");
    if (field) {
        m_SCINT_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "VetoNuID::initLevelsFromDict - unable to find 'subdet' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("part");
    if (field) {
        m_VETONU_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "VetoNuID::initLevelsFromDict - unable to find 'part' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("station");
    if (field) {
        m_STATION_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "VetoNuID::initLevelsFromDict - unable to find 'station' field "   << endmsg;
        return (1);
    }
    field = m_dict->find_field("plate");
    if (field) {
        m_PLATE_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR<< "VetoNuID::initLevelsFromDict - unable to find 'plate' field "  << endmsg;
        return (1);
    }
    field = m_dict->find_field("pmt");
    if (field) {
        m_PMT_INDEX = field->m_index;
    }
    else {
        log << MSG::ERROR << "VetoNuID::initLevelsFromDict - unable to find 'pmt' field " << endmsg;    
        return (1);
    }
    
    // Set the field implementations

    const IdDictRegion& region = *m_dict->m_regions[m_vetonu_region_index];

    m_scint_impl      = region.m_implementation[m_SCINT_INDEX]; 
    m_vetonu_impl       = region.m_implementation[m_VETONU_INDEX]; 
    m_station_impl    = region.m_implementation[m_STATION_INDEX]; 
    m_plate_impl      = region.m_implementation[m_PLATE_INDEX];
    m_pmt_impl        = region.m_implementation[m_PMT_INDEX]; 

    if (m_msgSvc) {
        log << MSG::DEBUG << "decode index and bit fields for each level: " << endmsg;
        log << MSG::DEBUG << "scint    "  << m_scint_impl.show_to_string() << endmsg;
        log << MSG::DEBUG << "vetonu     "  << m_vetonu_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "station  "  << m_station_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "plate    "  << m_plate_impl.show_to_string() << endmsg; 
        log << MSG::DEBUG << "pmt      "  << m_pmt_impl.show_to_string() << endmsg; 
    }
    else {
        std::cout << " DEBUG decode index and bit fields for each level: " << std::endl;
        std::cout << " DEBUG scint    "  << m_scint_impl.show_to_string() << std::endl;
        std::cout << " DEBUG vetonu     "  << m_vetonu_impl.show_to_string() << std::endl; 
        std::cout << " DEBUG station  "  << m_station_impl.show_to_string() << std::endl; 
        std::cout << " DEBUG plate    "  << m_plate_impl.show_to_string() << std::endl;
        std::cout << " DEBUG pmt      "  << m_pmt_impl.show_to_string() << std::endl; 
    }
    
    std::cout << "scint "  << m_scint_impl.decode_index() << " " 
              <<   (std::string)m_scint_impl.ored_field() << " " 
              << std::hex    << m_scint_impl.mask() << " " 
              << m_scint_impl.zeroing_mask() << " " 
              << std::dec    << m_scint_impl.shift() << " "
              << m_scint_impl.bits() << " "
              << m_scint_impl.bits_offset()
              << std::endl;
    std::cout << "vetonu"     << m_vetonu_impl.decode_index() << " " 
              <<   (std::string)m_vetonu_impl.ored_field() << " " 
              << std::hex    << m_vetonu_impl.mask() << " " 
              << m_vetonu_impl.zeroing_mask() << " " 
              << std::dec    << m_vetonu_impl.shift() << " "
              << m_vetonu_impl.bits() << " "
              << m_vetonu_impl.bits_offset()
              << std::endl;
    std::cout << "station"<< m_station_impl.decode_index() << " " 
              <<   (std::string)m_station_impl.ored_field() << " " 
              << std::hex    << m_station_impl.mask() << " " 
              << m_station_impl.zeroing_mask() << " " 
              << std::dec    << m_station_impl.shift() << " "
              << m_station_impl.bits() << " "
              << m_station_impl.bits_offset()
              << std::endl;
    std::cout << "plate"    << m_plate_impl.decode_index() << " " 
              <<   (std::string)m_plate_impl.ored_field() << " " 
              << std::hex    << m_plate_impl.mask() << " " 
              << m_plate_impl.zeroing_mask() << " " 
              << std::dec    << m_plate_impl.shift() << " "
              << m_plate_impl.bits() << " "
              << m_plate_impl.bits_offset()
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

VetoNuID::size_type       
VetoNuID::plate_hash_max (void) const
{
    return m_plate_hash_max;
}

VetoNuID::size_type       
VetoNuID::pmt_hash_max (void) const
{
    return m_pmt_hash_max;
}

VetoNuID::const_id_iterator       VetoNuID::plate_begin             (void) const
{
    return (m_plate_vec.begin());
}

VetoNuID::const_id_iterator       VetoNuID::plate_end               (void) const
{
    return (m_plate_vec.end());
}

VetoNuID::const_expanded_id_iterator      VetoNuID::pmt_begin     (void) const
{
    return (m_full_pmt_range.factory_begin());
}

VetoNuID::const_expanded_id_iterator      VetoNuID::pmt_end       (void) const
{
    return (m_full_pmt_range.factory_end());
}

// From hash get Identifier
int     
VetoNuID::get_id          (const IdentifierHash& hash_id,
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
        if (m_PLATE_INDEX == end) {
            if (hash_id < (unsigned int)(m_plate_vec.end() - m_plate_vec.begin())) {
                id = m_plate_vec[hash_id];
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
VetoNuID::get_expanded_id (const Identifier& id,
                         ExpandedIdentifier& exp_id,
                         const IdContext* context) const
{
    exp_id.clear();
    exp_id << scint_field_value()
           << vetonu_field_value()
           << station(id)
           << plate(id);
    if(!context || context->end_index() == m_PMT_INDEX) 
    {
       	exp_id << pmt(id);
    }
}

int     
VetoNuID::get_hash        (const Identifier& id, 
                         IdentifierHash& hash_id,
                         const IdContext* context) const
{

    // Get the hash code from either a vec (for plate) or calculate
    // it (pmts). For the former, we convert to compact and call
    // get_hash again. For the latter, we calculate the hash from the
    // Identifier.

    int result = 1;
    hash_id = 0;
    size_t begin = (context) ? context->begin_index(): 0;
    size_t end   = (context) ? context->end_index()  : 0; 
    if (0 == begin) {
        // No hashes yet for ids with prefixes
        if (m_PLATE_INDEX  == end) {
            hash_id = plate_hash(id);
            if (hash_id.is_valid()) result = 0;
        }
        else if (context && context->end_index() == m_PMT_INDEX) {
            // Must calculate for strip hash
            ExpandedIdentifier new_id;
            get_expanded_id(id, new_id);
            hash_id =  m_full_pmt_range.cardinalityUpTo(new_id);
            result = 0;
        }
    }
    return (result);
}


void    
VetoNuID::test_plate_packing      (void) const
{
    MsgStream log(m_msgSvc, "VetoNuID");

    if (m_dict) {
        
        int nids = 0;
        int nerr = 0;
        IdContext context = plate_context();
        const_id_iterator first = m_plate_vec.begin();
        const_id_iterator last  = m_plate_vec.end();
        for (; first != last; ++first, ++nids) {
            Identifier id = (*first);
            ExpandedIdentifier exp_id;
            get_expanded_id(id, exp_id, &context);
            Identifier new_id = plate_id(exp_id[m_STATION_INDEX],
                                         exp_id[m_PLATE_INDEX]);
            if (id != new_id) {
                log << MSG::ERROR << "VetoNuID::test_plate_packing: new and old compacts not equal. New/old/expanded ids " 
                    << MSG::hex << show_to_string(id) << " " << show_to_string(new_id) << " " << MSG::dec 
                    << (std::string)exp_id << endmsg;
                nerr++;
                continue;
            }
            // check station id
            if (!exp_id[m_PLATE_INDEX]) {
                
                Identifier new_id1 = station_id(exp_id[m_STATION_INDEX]);
                if (id != new_id1) {
                    log << MSG::ERROR << "VetoNuID::test_plate_packing: new and old station ids not equal. New/old/expanded ids " 
                        << MSG::hex << show_to_string(id) << " " << show_to_string(new_id1) << " " << MSG::dec 
                        << (std::string)exp_id << endmsg;
                    nerr++;
                    continue;
                }
            }
        }

        if (m_msgSvc) { 
            log << MSG::DEBUG << "VetoNuID::test_plate_packing: tested plate and station ids. nids, errors " 
                << nids << " " << nerr << endmsg;
        }
        else {
            std::cout << " DEBUG VetoNuID::test_plate_packing: tested plate and station ids. nids, errors " 
                      << nids << " " << nerr << std::endl;
        }
        
        nids = 0;
        context = pmt_context();
        const_expanded_id_iterator      first_vetonu = pmt_begin();  
        const_expanded_id_iterator      last_vetonu  = pmt_end();
        for (; first_vetonu != last_vetonu; ++first_vetonu, ++nids) {
            // if (nids%10000 != 1) continue;
            const ExpandedIdentifier& exp_id = *first_vetonu;
            ExpandedIdentifier new_exp_id;

            Identifier id = plate_id(exp_id[m_STATION_INDEX],
                                     exp_id[m_PLATE_INDEX]);
            get_expanded_id(id, new_exp_id, &context);
            if (exp_id[0] != new_exp_id[0] ||
                exp_id[1] != new_exp_id[1] ||
                exp_id[2] != new_exp_id[2] ||
                exp_id[3] != new_exp_id[3])
            {
                log << MSG::ERROR << "VetoNuID::test_plate_packing: new and old ids not equal. New/old/compact ids "
                    << (std::string)new_exp_id << " " << (std::string)exp_id
                    << " " << show_to_string(id) << endmsg;
                continue;
            }

            Identifier pmtid	;
	        Identifier pmtid1	;
           	pmtid = pmt_id ( 
                       exp_id[m_STATION_INDEX],
					   exp_id[m_PLATE_INDEX],
					   exp_id[m_PMT_INDEX]);

    	   	pmtid1 = pmt_id (	    
                        station(pmtid),
                        plate(pmtid),
                        pmt(pmtid));

            if (pmtid != pmtid1) {
                log << MSG::ERROR << "VetoNuID::test_plate_packing: new and old pixel ids not equal. New/old ids "
                    << " " << show_to_string(pmtid1) << " " 
                    << show_to_string(pmtid) << endmsg;
            }
        }

        if (m_msgSvc) {
            log << MSG::DEBUG << "VetoNuID::test_plate_packing: Successful tested " 
                << nids << " ids. " 
                << endmsg;
        }
        else {
            std::cout << " DEBUG VetoNuID::test_plate_packing: Successful tested " 
                      << nids << " ids. " 
                      << std::endl;
        }
    }
    else {
        log << MSG::ERROR << "VetoNuID::test_plate_packing: Unable to test plate packing - no dictionary has been defined. " 
            << endmsg;
    }
}

