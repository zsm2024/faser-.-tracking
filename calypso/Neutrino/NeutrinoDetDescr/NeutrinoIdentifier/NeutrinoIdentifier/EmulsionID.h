/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NEUTRINOIDENTIFIER_EMULSIONID_H
#define NEUTRINOIDENTIFIER_EMULSIONID_H
/**
 * @file EmulsionID.h
 *
 * @brief This is an Identifier helper class for the Emulsion
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
 **  @class EmulsionID
 **  
 **  @brief This is an Identifier helper class for the Emulsion
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
 **    module           0 to 72             73 modules
 **    base             0 to 9              10 bases per module
 **    film side        0 to 1              2 films per base
 **
 ** @endverbatim
 **
 */
class EmulsionID : public FaserDetectorID
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
    EmulsionID(void);
    virtual ~EmulsionID(void) = default;
    //@}
        
    /// @name Creators for base ids and side ids
    //@{
    /// For a single module
    Identifier  module_id ( int module ) const;
    Identifier  module_id ( int module,
                             bool checks) const;

    /// For a module from a base id
    Identifier  module_id ( const Identifier& base_id ) const;

    /// For a single base
    Identifier  base_id ( int module, 
                           int base ) const;
    Identifier  base_id ( int module, 
                           int base,
                           bool checks) const;

    /// For a single base from a film id
    Identifier  base_id ( const Identifier& film_id ) const;

    /// From hash - optimized
    Identifier  base_id ( IdentifierHash base_hash ) const;
    Identifier  film_id ( IdentifierHash film_hash ) const;

    /// For an individual film
    Identifier  film_id ( int module, 
                         int base, 
                         int film ) const; 

    Identifier  film_id ( int module, 
                         int base, 
                         int film,
                         bool check ) const; 

    Identifier  film_id ( const Identifier& base_id, 
                         int film ) const;

    //@}


    /// @name Hash table maximum sizes
    //@{
    size_type   base_hash_max          (void) const;
    size_type   film_hash_max            (void) const;
    //@}

    /// @name Access to all ids
    //@{
    /// Iterators over full set of ids. Base iterator is sorted
    const_id_iterator   base_begin                     (void) const;
    const_id_iterator   base_end                       (void) const;
    /// For film ids, only expanded id iterators are available. Use
    /// following "film_id" method to obtain a compact identifier
    const_expanded_id_iterator  film_begin             (void) const;  
    const_expanded_id_iterator  film_end               (void) const;
    //@}
    

    /// @name Optimized accessors  - ASSUMES id IS a veto id, i.e. NOT other
    //@{
    /// base hash from id - optimized
    IdentifierHash      base_hash      (Identifier base_id) const;

    IdentifierHash      film_hash      (Identifier film_id) const;

    /// Values of different levels (failure returns 0)
    int         module       (const Identifier& id) const;  
    int         base         (const Identifier& id) const; 
    int         film         (const Identifier& id) const; 

    /// Max/Min values for each field (-999 == failure)
    int         module_max  (const Identifier& id) const;
    int         base_max    (const Identifier& id) const;
    int         film_max    (const Identifier& id) const;
    //@}

    /// @name navigation
    //@{
        /// Previous base in z
        int get_prev_in_z(const IdentifierHash& id, IdentifierHash& prev) const;
        /// Next base in z
        int get_next_in_z(const IdentifierHash& id, IdentifierHash& next) const;
    // /// Previous wafer hash in phi (return == 0 for neighbor found)
    // int         get_prev_in_phi (const IdentifierHash& id, IdentifierHash& prev) const;
    // /// Next wafer hash in phi (return == 0 for neighbor found)
    // int         get_next_in_phi (const IdentifierHash& id, IdentifierHash& next) const;
    // /// Previous wafer hash in eta (return == 0 for neighbor found)
    // int         get_prev_in_eta (const IdentifierHash& id, IdentifierHash& prev) const;
    // /// Next wafer hash in eta (return == 0 for neighbor found)
    // int         get_next_in_eta (const IdentifierHash& id, IdentifierHash& next) const;
        /// Film hash on other side
        int         get_other_side  (const IdentifierHash& id, IdentifierHash& other) const;
    
    // // To check for when phi wrap around may be needed, use
    // bool        is_phi_module_max(const Identifier& id) const;
    // /// For the barrel
    // bool        is_eta_module_min(const Identifier& id) const;
    // /// For the barrel
    // bool        is_eta_module_max(const Identifier& id) const;
    //@}

    /// @name contexts to distinguish base id from film id
    //@{
    IdContext   base_context             (void) const;
    IdContext   film_context             (void) const;
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
    Identifier film_id_offset(const Identifier& base,
                             Identifier::diff_type offset) const;

    /// @name interaction with id dictionary
    //@{
    /// Create film Identifier from expanded id, which is returned by the
    /// id_iterators
    Identifier          film_id        (const ExpandedIdentifier& film_id) const;

    /// Create expanded id from compact id (return == 0 for OK)
    void                get_expanded_id (const Identifier& id,
                                         ExpandedIdentifier& exp_id,
                                         const IdContext* context = 0) const;

    /// Initialization from the identifier dictionary
    virtual int         initialize_from_dictionary(const IdDictMgr& dict_mgr);

    /// Tests of packing
    void        test_base_packing      (void) const;
    //@}
    
private:
        
    enum {NOT_VALID_HASH        = 64000};

    typedef std::vector<Identifier>     id_vec;
    typedef id_vec::const_iterator      id_vec_it;
    typedef std::vector<unsigned short> hash_vec;
    typedef hash_vec::const_iterator    hash_vec_it;

    void base_id_checks ( int module, 
                          int base ) const; 

    void film_id_checks ( int module, 
                          int base, 
                          int film ) const;


    int         initLevelsFromDict(void);

    int         init_hashes(void);

    int         init_neighbors(void);

    // Temporary method for adapting an identifier for the MultiRange
    // check - MR is missing the InnerDetector level
    // Identifier  idForCheck      (const Identifier& id) const;

    size_type                   m_emulsion_region_index;
    size_type                   m_NEUTRINO_INDEX;
    size_type                   m_EMULSION_INDEX;
    size_type                   m_MODULE_INDEX;
    size_type                   m_BASE_INDEX;
    size_type                   m_FILM_INDEX;

    const IdDictDictionary*     m_dict;
    MultiRange                  m_full_base_range;
    MultiRange                  m_full_film_range;
    size_type                   m_base_hash_max;
    size_type                   m_film_hash_max;
    // Range::field                m_barrel_field;
    id_vec                      m_base_vec;
    id_vec                      m_film_vec;
    hash_vec                    m_prev_z_base_vec;
    hash_vec                    m_next_z_base_vec;
    // hash_vec                    m_prev_phi_wafer_vec;
    // hash_vec                    m_next_phi_wafer_vec;
    // hash_vec                    m_prev_eta_wafer_vec;
    // hash_vec                    m_next_eta_wafer_vec;   
    // bool 			m_hasRows	;

    IdDictFieldImplementation   m_neutrino_impl	;
    IdDictFieldImplementation   m_emulsion_impl	;
    IdDictFieldImplementation   m_module_impl	;
    IdDictFieldImplementation   m_base_impl	;
    IdDictFieldImplementation   m_film_impl	;
};
    

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>

/////////////////////////////////////////////////////////////////////////////
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
/////////////////////////////////////////////////////////////////////////////

//using the macros below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF(EmulsionID, 89852815, 1)

//----------------------------------------------------------------------------
inline Identifier  
EmulsionID::module_id ( int module, 
                        bool checks) const
{
    
    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_neutrino_impl.pack       (neutrino_field_value(),  result);
    m_emulsion_impl.pack       (emulsion_field_value(),  result);
    m_module_impl.pack         (module,                  result);
    // Do checks
    if(checks) 
    {
        base_id_checks ( module, 0 );
    }

    return result;
}

inline Identifier  
EmulsionID::module_id ( int module ) const 
{
  return module_id (module, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier  
EmulsionID::module_id ( const Identifier& base_id ) const
{
    Identifier result(base_id);
    //  Reset the base and film fields
    m_base_impl.reset(result);
    m_film_impl.reset(result);
    return (result);
}

//----------------------------------------------------------------------------
inline Identifier
EmulsionID::base_id ( int module,  
                      int base, 
                      bool checks) const
{
    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_neutrino_impl.pack   (neutrino_field_value(), result);
    m_emulsion_impl.pack   (emulsion_field_value(),  result);
    m_module_impl.pack     (module,             result);
    m_base_impl.pack       (base,               result);

    // Do checks
    if(checks) 
    {
        base_id_checks ( module, base );
    }
    return result;
}

inline Identifier
EmulsionID::base_id ( int module,  
                      int base ) const
{
  return base_id (module, base, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier
EmulsionID::base_id ( const Identifier& film_id ) const
{
    Identifier result(film_id);
    // reset the film field
    m_film_impl.reset(result);
    return (result);
}

//----------------------------------------------------------------------------
inline Identifier  EmulsionID::base_id ( IdentifierHash base_hash ) const
{
    return (m_base_vec[base_hash]);
}

inline Identifier  EmulsionID::film_id ( IdentifierHash film_hash ) const
{
    return (m_film_vec[film_hash]);
}

//----------------------------------------------------------------------------
inline IdentifierHash      EmulsionID::base_hash      (Identifier base_id) const 
{
    // MsgStream log(m_msgSvc, "EmulsionID");
    // log << MSG::VERBOSE << "m_plate_vec size: " << m_plate_vec.size() << endmsg;
    // log << MSG::VERBOSE << "input id = " << plate_id << endmsg;
    // for (size_t i = 0; i < m_plate_vec.size(); i++)
    // {
    //     log << MSG::VERBOSE << "Hash = " <<  i << " : ID = " << m_plate_vec[i] << endmsg;
    // }
    id_vec_it it = std::lower_bound(m_base_vec.begin(), 
                                    m_base_vec.end(), 
                                    base_id);
    // Require that base_id matches the one in vector
    if (it != m_base_vec.end() && base_id == (*it)) {
        return (it - m_base_vec.begin());
    }
    IdentifierHash result;
    return (result); // return hash in invalid state
}

inline IdentifierHash      EmulsionID::film_hash      (Identifier film_id) const 
{
    // MsgStream log(m_msgSvc, "EmulsionID");
    // log << MSG::VERBOSE << "m_plate_vec size: " << m_plate_vec.size() << endmsg;
    // log << MSG::VERBOSE << "input id = " << plate_id << endmsg;
    // for (size_t i = 0; i < m_plate_vec.size(); i++)
    // {
    //     log << MSG::VERBOSE << "Hash = " <<  i << " : ID = " << m_plate_vec[i] << endmsg;
    // }
    id_vec_it it = std::lower_bound(m_film_vec.begin(), 
                                    m_film_vec.end(), 
                                    film_id);
    // Require that base_id matches the one in vector
    if (it != m_film_vec.end() && film_id == (*it)) {
        return (it - m_film_vec.begin());
    }
    IdentifierHash result;
    return (result); // return hash in invalid state
}




//----------------------------------------------------------------------------
inline Identifier
EmulsionID::film_id ( int module,  
                 int base, 
                 int film,
                 bool checks) const
{
    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_neutrino_impl.pack    (neutrino_field_value(), result);
    m_emulsion_impl.pack    (emulsion_field_value(),  result);
    m_module_impl.pack      (module,             result);
    m_base_impl.pack        (base,               result);
    m_film_impl.pack        (film,                 result);

    // Do checks
    if(checks) {
        film_id_checks ( module, base, film );
    }
    return result;
}

inline Identifier
EmulsionID::film_id ( int module,  
                 int base, 
                 int film ) const
{
  return film_id (module, base, film, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier               
EmulsionID::film_id        (const ExpandedIdentifier& id) const
{
    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_neutrino_impl.pack    (neutrino_field_value(),    result);
    m_emulsion_impl.pack    (emulsion_field_value(),     result);
    m_module_impl.pack      (id[m_MODULE_INDEX],    result);
    m_base_impl.pack        (id[m_BASE_INDEX],      result);
    m_film_impl.pack        (id[m_FILM_INDEX],        result);

    // Do checks
    if(m_do_checks) 
    {
       	film_id_checks ( id[m_MODULE_INDEX],  
                          id[m_BASE_INDEX], 
                       	  id[m_FILM_INDEX]);    
    }
    return result;
}

//----------------------------------------------------------------------------
inline Identifier  
EmulsionID::film_id ( const Identifier& base_id, int film ) const
{
	// Build identifier
    Identifier result(base_id);
  
    // Reset strip and then add in value
    m_film_impl.reset   (result);
 	m_film_impl.pack    (film, result);
  
    if(m_do_checks)
    {          
	    film_id_checks ( module(result), 
		           		base(result), 
                        film );
	}
	return result;
}

//----------------------------------------------------------------------------
inline Identifier::diff_type
EmulsionID::calc_offset(const Identifier& base, const Identifier& target) const
{
  Identifier::diff_type tval = static_cast<Identifier::diff_type>(target.get_compact() >> base_bit());
  Identifier::diff_type bval = static_cast<Identifier::diff_type>(base.get_compact() >> base_bit());
  return (tval - bval);
}

//----------------------------------------------------------------------------
inline Identifier
EmulsionID::film_id_offset(const Identifier& base,
                        Identifier::diff_type offset) const
{
  Identifier::value_type bval = base.get_compact() >> base_bit();
  return Identifier((bval + offset) << base_bit());
}

//----------------------------------------------------------------------------
inline int
EmulsionID::base_bit ( void ) const
{
  int base = static_cast<int>(m_film_impl.shift()); // lowest field base
  return (base > 9) ? 9 : base;
  // max base is 9 so we can still read old strip id's and differences
  // from non-SLHC releases.
}

//----------------------------------------------------------------------------
inline IdContext        
EmulsionID::base_context           (void) const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_BASE_INDEX));
}

//----------------------------------------------------------------------------
inline IdContext        
EmulsionID::film_context   (void) const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_FILM_INDEX));
}

//----------------------------------------------------------------------------
inline int 
EmulsionID::module       (const Identifier& id) const
{
    return (m_module_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int 
EmulsionID::base     (const Identifier& id) const
{
    return (m_base_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int 
EmulsionID::film           (const Identifier& id) const
{
    return (m_film_impl.unpack(id));
}


#endif // NEUTRINOIDENTIFIER_EMULSIONID_H
