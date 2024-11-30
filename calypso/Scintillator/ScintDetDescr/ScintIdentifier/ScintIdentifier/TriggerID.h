/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCINTIDENTIFIER_TRIGGERID_H
#define SCINTIDENTIFIER_TRIGGERID_H
/**
 * @file TriggerID.h
 *
 * @brief This is an Identifier helper class for the Trigger
 *  subdetector. This class is a factory for creating compact
 *  Identifier objects and IdentifierHash or hash ids. And it also
 *  allows decoding of these ids.
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "FaserDetDescr/FaserDetectorID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Range.h"
#include "Identifier/IdHelper.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <string>
#include <assert.h>
#include <algorithm>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class IdDictDictionary;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 **  @class TriggerID
 **  
 **  @brief This is an Identifier helper class for the Trigger
 **  subdetector. This class is a factory for creating compact
 **  Identifier objects and IdentifierHash or hash ids. And it also
 **  allows decoding of these ids.
 **
 **  Definition and the range of values for the levels of the
 **  identifier are:
 **
 ** @verbatim
 **    element           range              meaning
 **    -------           -----              -------
 **
 **    station          0                   single longitudinal location
 **    plate            0 - 1               two plates per station
 **    pmt              0 - 1               two pmts per plate
 **
 ** @endverbatim
 **
 */
class TriggerID : public FaserDetectorID
{
public:
        
    /// @name public typedefs
    //@{
    typedef Identifier::size_type                       size_type; 
    typedef std::vector<Identifier>::const_iterator     const_id_iterator;
    typedef MultiRange::const_identifier_factory        const_expanded_id_iterator;
    //@}

    /// @name strutors
    //@{
    TriggerID(void);
    virtual ~TriggerID(void) = default;
    //@}
        
    /// @name Creators for plate ids and pmt ids
    //@{
    /// For a single station
    Identifier  station_id ( int station ) const;
    Identifier  station_id ( int station,
                             bool checks) const;

    /// For a station from a plate id
    Identifier  station_id ( const Identifier& plate_id ) const;

    /// For a single plate
    Identifier  plate_id ( int station, 
                           int plate ) const;
    Identifier  plate_id ( int station, 
                           int plate,
                           bool checks) const;

    /// For a single plate from a pmt id
    Identifier  plate_id ( const Identifier& pmt_id ) const;

    /// From hash - optimized
    Identifier  plate_id ( IdentifierHash plate_hash ) const;

    /// For an individual pmt
    Identifier  pmt_id ( int station, 
                         int plate, 
                         int pmt ) const; 

    Identifier  pmt_id ( int station, 
                         int plate, 
                         int pmt,
                         bool check ) const; 

    Identifier  pmt_id ( const Identifier& plate_id, 
                         int pmt ) const;

    //@}


    /// @name Hash table maximum sizes
    //@{
    size_type   plate_hash_max          (void) const;
    size_type   pmt_hash_max            (void) const;
    //@}

    /// @name Access to all ids
    //@{
    /// Iterators over full set of ids. Plate iterator is sorted
    const_id_iterator   plate_begin                     (void) const;
    const_id_iterator   plate_end                       (void) const;
    /// For pmt ids, only expanded id iterators are available. Use
    /// following "pmt_id" method to obtain a compact identifier
    const_expanded_id_iterator  pmt_begin             (void) const;  
    const_expanded_id_iterator  pmt_end               (void) const;
    //@}
    

    /// @name Optimized accessors  - ASSUMES id IS a trigger id, i.e. NOT other
    //@{
    /// wafer hash from id - optimized
    IdentifierHash      plate_hash      (Identifier plate_id) const;

    /// Values of different levels (failure returns 0)
    int         station       (const Identifier& id) const;  
    int         plate         (const Identifier& id) const; 
    int         pmt           (const Identifier& id) const; 

    /// Max/Min values for each field (-999 == failure)
    int         station_max  (const Identifier& id) const;
    int         plate_max    (const Identifier& id) const;
    int         pmt_max      (const Identifier& id) const;
    //@}

    /// @name module navigation
    //@{
        /// Previous plate in z
        int get_prev_in_z(const IdentifierHash& id, IdentifierHash& prev) const;
        /// Next plate in z
        int get_next_in_z(const IdentifierHash& id, IdentifierHash& next) const;
    // /// Previous wafer hash in phi (return == 0 for neighbor found)
    // int         get_prev_in_phi (const IdentifierHash& id, IdentifierHash& prev) const;
    // /// Next wafer hash in phi (return == 0 for neighbor found)
    // int         get_next_in_phi (const IdentifierHash& id, IdentifierHash& next) const;
    // /// Previous wafer hash in eta (return == 0 for neighbor found)
    // int         get_prev_in_eta (const IdentifierHash& id, IdentifierHash& prev) const;
    // /// Next wafer hash in eta (return == 0 for neighbor found)
    // int         get_next_in_eta (const IdentifierHash& id, IdentifierHash& next) const;
    // /// Wafer hash on other side
    // int         get_other_side  (const IdentifierHash& id, IdentifierHash& other) const;
    
    // // To check for when phi wrap around may be needed, use
    // bool        is_phi_module_max(const Identifier& id) const;
    // /// For the barrel
    // bool        is_eta_module_min(const Identifier& id) const;
    // /// For the barrel
    // bool        is_eta_module_max(const Identifier& id) const;
    //@}

    /// @name contexts to distinguish plate id from pixel id
    //@{
    IdContext   plate_context           (void) const;
    IdContext   pmt_context             (void) const;
    //@}

    /// @name methods from abstract interface - slower than opt version
    //@{
    /// Create compact id from hash id (return == 0 for OK)
    virtual int         get_id          (const IdentifierHash& hash_id,
                                         Identifier& id,
                                         const IdContext* context = 0) const;
    
    /// Create hash id from compact id (return == 0 for OK)
    virtual int         get_hash        (const Identifier& id, 
                                         IdentifierHash& hash_id,
                                         const IdContext* context = 0) const;
    //@}

    /// Return the lowest bit position used in the channel id
    int                 base_bit        (void) const;

    /// Calculate a channel offset between the two identifiers.
    Identifier::diff_type calc_offset(const Identifier& base,
                                      const Identifier& target) const;

    /// Create an identifier with a given base and channel offset
    Identifier pmt_id_offset(const Identifier& base,
                             Identifier::diff_type offset) const;

    /// @name interaction with id dictionary
    //@{
    /// Create strip Identifier from expanded id, which is returned by the
    /// id_iterators
    Identifier          pmt_id        (const ExpandedIdentifier& pmt_id) const;

    /// Create expanded id from compact id (return == 0 for OK)
    void                get_expanded_id (const Identifier& id,
                                         ExpandedIdentifier& exp_id,
                                         const IdContext* context = 0) const;

    /// Initialization from the identifier dictionary
    virtual int         initialize_from_dictionary(const IdDictMgr& dict_mgr);

    /// Tests of packing
    void        test_plate_packing      (void) const;
    //@}
    
private:
        
    enum {NOT_VALID_HASH        = 64000};

    typedef std::vector<Identifier>     id_vec;
    typedef id_vec::const_iterator      id_vec_it;
    typedef std::vector<unsigned short> hash_vec;
    typedef hash_vec::const_iterator    hash_vec_it;

    void plate_id_checks ( int station, 
                           int plate ) const; 

    void pmt_id_checks ( int station, 
                         int plate, 
                         int pmt ) const;


    int         initLevelsFromDict(void);

    int         init_hashes(void);

    int         init_neighbors(void);

    // Temporary method for adapting an identifier for the MultiRange
    // check - MR is missing the InnerDetector level
    // Identifier  idForCheck      (const Identifier& id) const;

    size_type                   m_trigger_region_index;
    size_type                   m_SCINT_INDEX;
    size_type                   m_TRIGGER_INDEX;
    size_type                   m_STATION_INDEX;
    size_type                   m_PLATE_INDEX;
    size_type                   m_PMT_INDEX;

    const IdDictDictionary*     m_dict;
    MultiRange                  m_full_plate_range;
    MultiRange                  m_full_pmt_range;
    size_type                   m_plate_hash_max;
    size_type                   m_pmt_hash_max;
    // Range::field                m_barrel_field;
    id_vec                      m_plate_vec;
    hash_vec                    m_prev_z_plate_vec;
    hash_vec                    m_next_z_plate_vec;
    // hash_vec                    m_prev_phi_wafer_vec;
    // hash_vec                    m_next_phi_wafer_vec;
    // hash_vec                    m_prev_eta_wafer_vec;
    // hash_vec                    m_next_eta_wafer_vec;   
    // bool 			m_hasRows	;

    IdDictFieldImplementation   m_scint_impl	;
    IdDictFieldImplementation   m_trigger_impl	;
    IdDictFieldImplementation   m_station_impl	;
    IdDictFieldImplementation   m_plate_impl	;
    IdDictFieldImplementation   m_pmt_impl	;
};
    

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>

/////////////////////////////////////////////////////////////////////////////
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
/////////////////////////////////////////////////////////////////////////////

//using the macros below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF(TriggerID, 58382802, 1)

//----------------------------------------------------------------------------
inline Identifier  
TriggerID::station_id ( int station, 
                     bool checks) const
{
    
    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_scint_impl.pack       (scint_field_value(),    result);
    m_trigger_impl.pack     (trigger_field_value(),  result);
    m_station_impl.pack     (station,                result);
    // Do checks
    if(checks) 
    {
        plate_id_checks ( station, 0 );
    }

    return result;
}

inline Identifier  
TriggerID::station_id ( int station ) const 
{
  return station_id (station, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier  
TriggerID::station_id ( const Identifier& plate_id ) const
{
    Identifier result(plate_id);
    //  Reset the plate and pmt fields
    m_plate_impl.reset(result);
    m_pmt_impl.reset(result);
    return (result);
}

//----------------------------------------------------------------------------
inline Identifier
TriggerID::plate_id ( int station,  
                   int plate, 
                   bool checks) const
{
    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_scint_impl.pack    (scint_field_value(),    result);
    m_trigger_impl.pack  (trigger_field_value(),  result);
    m_station_impl.pack  (station,                result);
    m_plate_impl.pack    (plate,                  result);

    // Do checks
    if(checks) 
    {
        plate_id_checks ( station, plate );
    }
    return result;
}

inline Identifier
TriggerID::plate_id ( int station,  
                   int plate ) const
{
  return plate_id (station, plate, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier
TriggerID::plate_id ( const Identifier& pmt_id ) const
{
    Identifier result(pmt_id);
    // reset the pmt field
    m_pmt_impl.reset(result);
    return (result);
}

//----------------------------------------------------------------------------
inline Identifier  TriggerID::plate_id ( IdentifierHash plate_hash ) const
{
    return (m_plate_vec[plate_hash]);
}

//----------------------------------------------------------------------------
inline IdentifierHash      TriggerID::plate_hash      (Identifier plate_id) const 
{
    id_vec_it it = std::lower_bound(m_plate_vec.begin(), 
                                    m_plate_vec.end(), 
                                    plate_id);
    // Require that plate_id matches the one in vector
    if (it != m_plate_vec.end() && plate_id == (*it)) {
        return (it - m_plate_vec.begin());
    }
    IdentifierHash result;
    return (result); // return hash in invalid state
}

//----------------------------------------------------------------------------
inline Identifier
TriggerID::pmt_id ( int station,  
                 int plate, 
                 int pmt,
                 bool checks) const
{
    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_scint_impl.pack    (scint_field_value(),    result);
    m_trigger_impl.pack  (trigger_field_value(),  result);
    m_station_impl.pack  (station,                result);
    m_plate_impl.pack    (plate,                  result);
    m_pmt_impl.pack      (pmt,                    result);

    // Do checks
    if(checks) {
        pmt_id_checks ( station, plate, pmt );
    }
    return result;
}

inline Identifier
TriggerID::pmt_id ( int station,  
                 int plate, 
                 int pmt ) const
{
  return pmt_id (station, plate, pmt, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier               
TriggerID::pmt_id        (const ExpandedIdentifier& id) const
{
    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_scint_impl.pack    (scint_field_value(),       result);
    m_trigger_impl.pack  (trigger_field_value(),     result);
    m_station_impl.pack  (id[m_STATION_INDEX],       result);
    m_plate_impl.pack    (id[m_PLATE_INDEX],         result);
    m_pmt_impl.pack      (id[m_PMT_INDEX],           result);

    // Do checks
    if(m_do_checks) 
    {
       	pmt_id_checks ( id[m_STATION_INDEX],  
                          id[m_PLATE_INDEX], 
                       	  id[m_PMT_INDEX]);    
    }
    return result;
}

//----------------------------------------------------------------------------
inline Identifier  
TriggerID::pmt_id ( const Identifier& plate_id, int pmt ) const
{
	// Build identifier
    Identifier result(plate_id);
  
    // Reset strip and then add in value
    m_pmt_impl.reset   (result);
 	m_pmt_impl.pack    (pmt, result);
  
    if(m_do_checks)
    {          
	    pmt_id_checks ( station(result), 
		           		plate(result), 
                        pmt );
	}
	return result;
}

//----------------------------------------------------------------------------
inline Identifier::diff_type
TriggerID::calc_offset(const Identifier& base, const Identifier& target) const
{
  Identifier::diff_type tval = static_cast<Identifier::diff_type>(target.get_compact() >> base_bit());
  Identifier::diff_type bval = static_cast<Identifier::diff_type>(base.get_compact() >> base_bit());
  return (tval - bval);
}

//----------------------------------------------------------------------------
inline Identifier
TriggerID::pmt_id_offset(const Identifier& base,
                        Identifier::diff_type offset) const
{
  Identifier::value_type bval = base.get_compact() >> base_bit();
  return Identifier((bval + offset) << base_bit());
}

//----------------------------------------------------------------------------
inline int
TriggerID::base_bit ( void ) const
{
  int base = static_cast<int>(m_pmt_impl.shift()); // lowest field base
  return (base > 32) ? 32 : base;
  // max base is 32 so we can still read old strip id's and differences
  // from non-SLHC releases.
}

//----------------------------------------------------------------------------
inline IdContext        
TriggerID::plate_context           (void) const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_PLATE_INDEX));
}

//----------------------------------------------------------------------------
inline IdContext        
TriggerID::pmt_context   (void) const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_PMT_INDEX));
}

//----------------------------------------------------------------------------
inline int 
TriggerID::station       (const Identifier& id) const
{
    return (m_station_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int 
TriggerID::plate     (const Identifier& id) const
{
    return (m_plate_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int 
TriggerID::pmt           (const Identifier& id) const
{
    return (m_pmt_impl.unpack(id));
}


#endif // SCINTIDENTIFIER_TRIGGERID_H
