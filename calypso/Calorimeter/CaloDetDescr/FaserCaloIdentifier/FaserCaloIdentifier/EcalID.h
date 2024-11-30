/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOIDENTIFIER_ECALID_H
#define CALOIDENTIFIER_ECALID_H
/**
 * @file EcalID.h
 *
 * @brief This is an Identifier helper class for the Ecal
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
 **  @class EcalID
 **  
 **  @brief This is an Identifier helper class for the Ecal
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
 **    row              0 - 1               vertical location of module (bottom, top)
 **    module           0 - 1               horizontal location of module in row (left, right)
 **    pmt              0                   single pmt per module
 **
 ** @endverbatim
 **
 */
class EcalID : public FaserDetectorID
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
    EcalID(void);
    virtual ~EcalID(void) = default;
    //@}
        
    /// @name Creators for module ids and pmt ids
    //@{
    /// For a single row
    Identifier  row_id ( int row ) const;
    Identifier  row_id ( int row,
                         bool checks) const;

    /// For a row from a module id
    Identifier  row_id ( const Identifier& module_id ) const;

    /// For a single module
    Identifier  module_id ( int row, 
                            int module ) const;
    Identifier  module_id ( int row, 
                            int module,
                            bool checks) const;

    /// For a single module from a pmt id
    Identifier  module_id ( const Identifier& pmt_id ) const;

    /// From hash - optimized
    Identifier  module_id ( IdentifierHash module_hash ) const;

    /// For an individual pmt
    Identifier  pmt_id ( int row, 
                         int module, 
                         int pmt ) const; 

    Identifier  pmt_id ( int row, 
                         int module, 
                         int pmt,
                         bool check ) const; 

    Identifier  pmt_id ( const Identifier& module_id, 
                         int pmt ) const;

    //@}


    /// @name Hash table maximum sizes
    //@{
    size_type   module_hash_max         (void) const;
    size_type   pmt_hash_max            (void) const;
    //@}

    /// @name Access to all ids
    //@{
    /// Iterators over full set of ids. Module iterator is sorted
    const_id_iterator   module_begin                     (void) const;
    const_id_iterator   module_end                       (void) const;
    /// For pmt ids, only expanded id iterators are available. Use
    /// following "pmt_id" method to obtain a compact identifier
    const_expanded_id_iterator  pmt_begin             (void) const;  
    const_expanded_id_iterator  pmt_end               (void) const;
    //@}
    

    /// @name Optimized accessors  - ASSUMES id IS an Ecal id, i.e. NOT other
    //@{
    /// module hash from id - optimized
    IdentifierHash      module_hash      (Identifier module_id) const;

    /// Values of different levels (failure returns 0)
    int         row       (const Identifier& id) const;  
    int         module    (const Identifier& id) const; 
    int         pmt       (const Identifier& id) const; 

    /// Max/Min values for each field (-999 == failure)
    int         row_max      (const Identifier& id) const;
    int         module_max   (const Identifier& id) const;
    int         pmt_max      (const Identifier& id) const;
    //@}

    /// @name module navigation
    //@{
        // /// Previous in z
        // int get_prev_in_z(const IdentifierHash& id, IdentifierHash& prev) const;
        // /// Next in z
        // int get_next_in_z(const IdentifierHash& id, IdentifierHash& next) const;
        /// Previous module hash in phi (return == 0 for neighbor found)
        int get_prev_in_phi (const IdentifierHash& id, IdentifierHash& prev) const;
        /// Next module hash in phi (return == 0 for neighbor found)
        int get_next_in_phi (const IdentifierHash& id, IdentifierHash& next) const;
        /// Previous module hash in eta (return == 0 for neighbor found)
        int get_prev_in_eta (const IdentifierHash& id, IdentifierHash& prev) const;
        // /// Next module hash in eta (return == 0 for neighbor found)
        int get_next_in_eta (const IdentifierHash& id, IdentifierHash& next) const;
    // /// Wafer hash on other side
    // int         get_other_side  (const IdentifierHash& id, IdentifierHash& other) const;
    
    // Check limits
    bool        is_phi_module_max(const Identifier& id) const;
    bool        is_phi_module_min(const Identifier& id) const;
    bool        is_eta_module_min(const Identifier& id) const;
    bool        is_eta_module_max(const Identifier& id) const;
    //@}

    /// @name contexts to distinguish module id from pmt id
    //@{
    IdContext   module_context          (void) const;
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
    void        test_module_packing      (void) const;
    //@}
    
private:
        
    enum {NOT_VALID_HASH        = 64000};

    typedef std::vector<Identifier>     id_vec;
    typedef id_vec::const_iterator      id_vec_it;
    typedef std::vector<unsigned short> hash_vec;
    typedef hash_vec::const_iterator    hash_vec_it;

    void module_id_checks ( int row, 
                            int module ) const; 

    void pmt_id_checks ( int row, 
                         int module, 
                         int pmt ) const;


    int         initLevelsFromDict(void);

    int         init_hashes(void);

    int         init_neighbors(void);

    // Temporary method for adapting an identifier for the MultiRange
    // check - MR is missing the InnerDetector level
    // Identifier  idForCheck      (const Identifier& id) const;

    size_type                   m_ecal_region_index;
    size_type                   m_CALO_INDEX;
    size_type                   m_ECAL_INDEX;
    size_type                   m_ROW_INDEX;
    size_type                   m_MODULE_INDEX;
    size_type                   m_PMT_INDEX;

    const IdDictDictionary*     m_dict;
    MultiRange                  m_full_module_range;
    MultiRange                  m_full_pmt_range;
    size_type                   m_module_hash_max;
    size_type                   m_pmt_hash_max;
    // Range::field                m_barrel_field;
    id_vec                      m_module_vec;
    // hash_vec                    m_prev_z_plate_vec;
    // hash_vec                    m_next_z_plate_vec;
    hash_vec                    m_prev_phi_module_vec;
    hash_vec                    m_next_phi_module_vec;
    hash_vec                    m_prev_eta_module_vec;
    hash_vec                    m_next_eta_module_vec;   
    // bool 			m_hasRows	;

    IdDictFieldImplementation   m_calo_impl	;
    IdDictFieldImplementation   m_ecal_impl	;
    IdDictFieldImplementation   m_row_impl	;
    IdDictFieldImplementation   m_module_impl	;
    IdDictFieldImplementation   m_pmt_impl	;
};
    

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>

/////////////////////////////////////////////////////////////////////////////
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
/////////////////////////////////////////////////////////////////////////////

//using the macros below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF(EcalID, 113753346, 1)

//----------------------------------------------------------------------------
inline Identifier  
EcalID::row_id ( int row, 
                 bool checks) const
{
    
    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_calo_impl.pack        (calo_field_value(), result);
    m_ecal_impl.pack        (ecal_field_value(),  result);
    m_row_impl.pack         (row,             result);
    // Do checks
    if(checks) 
    {
        module_id_checks ( row, 0 );
    }

    return result;
}

inline Identifier  
EcalID::row_id ( int row ) const 
{
  return row_id (row, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier  
EcalID::row_id ( const Identifier& module_id ) const
{
    Identifier result(module_id);
    //  Reset the module and pmt fields
    m_module_impl.reset(result);
    m_pmt_impl.reset(result);
    return (result);
}

//----------------------------------------------------------------------------
inline Identifier
EcalID::module_id ( int row,  
                   int module, 
                   bool checks) const
{
    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_calo_impl.pack     (calo_field_value(), result);
    m_ecal_impl.pack     (ecal_field_value(), result);
    m_row_impl.pack      (row,                result);
    m_module_impl.pack   (module,             result);

    // Do checks
    if(checks) 
    {
        module_id_checks ( row, module );
    }
    return result;
}

inline Identifier
EcalID::module_id ( int row,  
                    int module ) const
{
  return module_id (row, module, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier
EcalID::module_id ( const Identifier& pmt_id ) const
{
    Identifier result(pmt_id);
    // reset the pmt field
    m_pmt_impl.reset(result);
    return (result);
}

//----------------------------------------------------------------------------
inline Identifier  EcalID::module_id ( IdentifierHash module_hash ) const
{
    return (m_module_vec[module_hash]);
}

//----------------------------------------------------------------------------
inline IdentifierHash      EcalID::module_hash      (Identifier module_id) const 
{
    // MsgStream log(m_msgSvc, "EcalID");
    // log << MSG::VERBOSE << "m_module_vec size: " << m_module_vec.size() << endmsg;
    // log << MSG::VERBOSE << "input id = " << module_id << endmsg;
    // for (size_t i = 0; i < m_module_vec.size(); i++)
    // {
    //     log << MSG::VERBOSE << "Hash = " <<  i << " : ID = " << m_module_vec[i] << endmsg;
    // }
    id_vec_it it = std::lower_bound(m_module_vec.begin(), 
                                    m_module_vec.end(), 
                                    module_id);
    // Require that module_id matches the one in vector
    if (it != m_module_vec.end() && module_id == (*it)) {
        return (it - m_module_vec.begin());
    }
    IdentifierHash result;
    return (result); // return hash in invalid state
}

//----------------------------------------------------------------------------
inline Identifier
EcalID::pmt_id ( int row,  
                 int module, 
                 int pmt,
                 bool checks) const
{
    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_calo_impl.pack     (calo_field_value(),  result);
    m_ecal_impl.pack     (ecal_field_value(),  result);
    m_row_impl.pack      (row,                 result);
    m_module_impl.pack   (module,              result);
    m_pmt_impl.pack      (pmt,                 result);

    // Do checks
    if(checks) {
        pmt_id_checks ( row, module, pmt );
    }
    return result;
}

inline Identifier
EcalID::pmt_id ( int row,  
                 int module, 
                 int pmt ) const
{
  return pmt_id (row, module, pmt, do_checks());
}

//----------------------------------------------------------------------------
inline Identifier               
EcalID::pmt_id        (const ExpandedIdentifier& id) const
{
    // Build identifier
    Identifier result((Identifier::value_type)0);

    // Pack fields independently
    m_calo_impl.pack     (calo_field_value(),    result);
    m_ecal_impl.pack     (ecal_field_value(),    result);
    m_row_impl.pack      (id[m_ROW_INDEX],       result);
    m_module_impl.pack   (id[m_MODULE_INDEX],    result);
    m_pmt_impl.pack      (id[m_PMT_INDEX],       result);

    // Do checks
    if(m_do_checks) 
    {
       	pmt_id_checks ( id[m_ROW_INDEX],  
                        id[m_MODULE_INDEX], 
                       	id[m_PMT_INDEX]);    
    }
    return result;
}

//----------------------------------------------------------------------------
inline Identifier  
EcalID::pmt_id ( const Identifier& module_id, int pmt ) const
{
	// Build identifier
    Identifier result(module_id);
  
    // Reset pmt and then add in value
    m_pmt_impl.reset   (result);
 	m_pmt_impl.pack    (pmt, result);
  
    if(m_do_checks)
    {          
	    pmt_id_checks ( row(result), 
		           		module(result), 
                        pmt );
	}
	return result;
}

//----------------------------------------------------------------------------
inline Identifier::diff_type
EcalID::calc_offset(const Identifier& base, const Identifier& target) const
{
  Identifier::diff_type tval = static_cast<Identifier::diff_type>(target.get_compact() >> base_bit());
  Identifier::diff_type bval = static_cast<Identifier::diff_type>(base.get_compact() >> base_bit());
  return (tval - bval);
}

//----------------------------------------------------------------------------
inline Identifier
EcalID::pmt_id_offset(const Identifier& base,
                        Identifier::diff_type offset) const
{
  Identifier::value_type bval = base.get_compact() >> base_bit();
  return Identifier((bval + offset) << base_bit());
}

//----------------------------------------------------------------------------
inline int
EcalID::base_bit ( void ) const
{
  int base = static_cast<int>(m_pmt_impl.shift()); // lowest field base
  return (base > 32) ? 32 : base;
  // max base is 32 so we can still read old strip id's and differences
  // from non-SLHC releases.
}

//----------------------------------------------------------------------------
inline IdContext        
EcalID::module_context           (void) const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_MODULE_INDEX));
}

//----------------------------------------------------------------------------
inline IdContext        
EcalID::pmt_context   (void) const
{
    ExpandedIdentifier id;
    return (IdContext(id, 0, m_PMT_INDEX));
}

//----------------------------------------------------------------------------
inline int 
EcalID::row       (const Identifier& id) const
{
    return (m_row_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int 
EcalID::module     (const Identifier& id) const
{
    return (m_module_impl.unpack(id));
}

//----------------------------------------------------------------------------
inline int 
EcalID::pmt           (const Identifier& id) const
{
    return (m_pmt_impl.unpack(id));
}


#endif // CALOIDENTIFIER_ECALID_H
