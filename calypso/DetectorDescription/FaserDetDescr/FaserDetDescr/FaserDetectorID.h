/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERDETDESCR_FASERDETECTORID_H
#define FASERDETDESCR_FASERDETECTORID_H
/**
 * @file FaserDetectorID.h
 *
 * @brief This class provides an interface to generate or decode an
 *   identifier for the upper levels of the detector element
 *   hierarchy, i.e. Faser, the detector systems and detector
 *   subsystems. As well general "print out" methods are provide for
 *   all detector systems. All id helper classes derive from this
 *   class. This class also provides access to the version tags of the
 *   id dictionaries used by the helper.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * Modified for FASER
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "Identifier/Identifier.h"
#include "Identifier/IdContext.h"
#include "Identifier/IdHelper.h"
#include "Identifier/Range.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "FaserDetTechnology.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <string>
#include <vector>

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class IdDictDictionary;
class FaserDetectorIDHelper;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>


/**
 * @class FaserDetectorID
 *   
 * @brief This class provides an interface to generate or decode an
 *   identifier for the upper levels of the detector element
 *   hierarchy, i.e. Faser, the detector systems and detector
 *   subsystems. As well general "print out" methods are provide for
 *   all detector systems. All id helper classes derive from this
 *   class. This class also provides access to the version tags of the
 *   id dictionaries used by the helper.
 **/

class FaserDetectorID : public IdHelper
{
public:
    
    /// @name strutors
    //@{
    FaserDetectorID();
    FaserDetectorID(const FaserDetectorID& other);
    ~FaserDetectorID(void);
    FaserDetectorID& operator= (const FaserDetectorID& other);
    //@}
    
    /// @name Detector system ids
    //@{
    /// Detector systems:
    Identifier          neutrino        (void) const;
    Identifier          scint           (void) const;
    Identifier          tracker         (void) const;
    Identifier          calo            (void) const;
    //@}

    /// @name Neutrino subsystem ids
    //@{
    Identifier          emulsion        (void) const;
    //@}


    /// @name Scintillator subsystem ids
    //@{
    Identifier          veto            (void) const;
    Identifier          vetonu          (void) const;
    Identifier          trigger         (void) const;
    Identifier          preshower       (void) const;
    //@}

    /// @name Tracker subsystem ids
    //@{
    Identifier          sct             (void) const;
    //@}

    /// @name Calorimeter subsystem ids
    //@{
    Identifier          ecal            (void) const;
    //@}

    /// @name Contexts to provide id length - e.g. for use in generic decoding
    //@{
    /// IdContext (indicates id length) for detector systems
    IdContext           detsystem_context (void) const;

    /// IdContext (indicates id length) for sub-detector
    IdContext           subdet_context  (void) const;
    //@}


    /// @name Generic conversion between identifier and idhash
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

    /// @name Initialization and version name 
    //@{
    /// Initialization from the identifier dictionary
    virtual int         initialize_from_dictionary(const IdDictMgr& dict_mgr);

    // retrieve version of the dictionary
    virtual std::string   dictionaryVersion  (void) const;
    //@}


    /// @name Generic printing of identifiers
    //@{
    /// Short print out of any identifier (optionally provide
    /// separation character - default is '.'):
    void                show            (Identifier id, 
                                         const IdContext* context = 0,
                                         char sep = '.' ) const;
    /// or provide the printout in string form
    std::string         show_to_string  (Identifier id, 
                                         const IdContext* context = 0,
                                         char sep = '.'  ) const;

    /// Expanded print out of any identifier
    void                print           (Identifier id,
                                         const IdContext* context = 0) const;
    /// or provide the printout in string form
    std::string         print_to_string (Identifier id,
                                         const IdContext* context = 0) const;
    //@}

    /// @name  Test of an Identifier to see if it belongs to a particular detector (sub)system:
    //@{
    bool                is_neutrino     (Identifier id) const;
    bool                is_scint        (Identifier id) const;
    bool                is_tracker      (Identifier id) const;
    bool                is_calo         (Identifier id) const;
    bool                is_emulsion     (Identifier id) const;
    bool                is_veto         (Identifier id) const;
    bool                is_vetonu       (Identifier id) const;
    bool                is_trigger      (Identifier id) const;
    bool                is_preshower    (Identifier id) const;
    bool                is_sct          (Identifier id) const;
    bool                is_ecal         (Identifier id) const;
    //@}


    /// @name  Test of an Identifier to see if it belongs to a particular detector (sub)system (using expanded ids):
    //@{
    bool                is_neutrino     (const ExpandedIdentifier& id) const;
    bool                is_scint        (const ExpandedIdentifier& id) const;
    bool                is_tracker      (const ExpandedIdentifier& id) const;
    bool                is_calo         (const ExpandedIdentifier& id) const;
    bool                is_emulsion     (const ExpandedIdentifier& id) const;
    bool                is_veto         (const ExpandedIdentifier& id) const;
    bool                is_vetonu       (const ExpandedIdentifier& id) const;
    bool                is_trigger      (const ExpandedIdentifier& id) const;
    bool                is_preshower    (const ExpandedIdentifier& id) const;
    bool                is_sct          (const ExpandedIdentifier& id) const;
    bool                is_ecal         (const ExpandedIdentifier& id) const;
    //@}

    /// @name  Dictionary versioning: provide access to dictionary names and versions. Note that a helper may correspond to one or more id dictionary
    //@{
    ///  Dictionary names
    std::vector<std::string>  dict_names(void) const;
    ///  File names for subdet dictionaries
    std::vector<std::string>  file_names(void) const;
    ///  Version tags for subdet dictionaries
    std::vector<std::string>  dict_tags (void) const;
    //@}

    /// @name  Checks are performed by default in debug compilation and NOT in optimized compilation. One can switch or query this mode for any idHelper with the following methods:
    //@{
    virtual bool        do_checks       (void) const;
    virtual void        set_do_checks   (bool do_checks);
    //@}

    /// @name neighbours are initialized by default. One can switch or query this mode with the following methods:
    //@{
    virtual bool        do_neighbours       (void) const;
    virtual void        set_do_neighbours   (bool do_neighbours);
    //@}

    /// @name setting pointer to the MessageService
    //@{
    virtual void setMessageSvc  (IMessageSvc* msgSvc);
    //@}

    /// Set flag for suppressing informational output.
    void set_quiet (bool quiet);


protected:

    friend class FaserDetectorIDHelper;

    void                setDictVersion  (const IdDictMgr& dict_mgr, const std::string& name);

    std::string         to_range        (const ExpandedIdentifier& id) const;
    
    std::string         fix_barrel_ec   (const std::string& barrel_ec) const;

    /// Detector systems:
    ExpandedIdentifier          neutrino_exp        (void) const;
    ExpandedIdentifier          scint_exp           (void) const;
    ExpandedIdentifier          tracker_exp         (void) const;
    ExpandedIdentifier          calo_exp            (void) const;

    /// Neutrino:
    ExpandedIdentifier          emulsion_exp        (void) const;

    /// Scintillator:
    ExpandedIdentifier          veto_exp            (void) const;
    ExpandedIdentifier          vetonu_exp          (void) const;
    ExpandedIdentifier          trigger_exp         (void) const;
    ExpandedIdentifier          preshower_exp       (void) const;

    /// Tracker:
    ExpandedIdentifier          sct_exp             (void) const;

    /// Calorimeter:
    ExpandedIdentifier          ecal_exp            (void) const;

    /// Provide efficient access to individual field values, for
    /// subclass idhelpers
    int                 neutrino_field_value     () const;
    int                 scint_field_value        () const;     
    int                 tracker_field_value      () const;       
    int                 calo_field_value         () const;
    int                 emulsion_field_value     () const;
    int                 veto_field_value         () const;     
    int                 vetonu_field_value       () const;     
    int                 trigger_field_value      () const;       
    int                 preshower_field_value    () const;       
    int                 sct_field_value          () const;
    int                 ecal_field_value         () const;

    /// Register the file and tag names for a particular IdDict
    /// dictionary
    int                 register_dict_tag        (const IdDictMgr& dict_mgr,
                                                  const std::string& dict_name);

    /// Test whether an idhelper should be reinitialized based on the
    /// change of tags 
    bool                reinitialize             (const IdDictMgr& dict_mgr);

    /// Flag for subclasses to know whether or not to perform
    /// checks. In general, this is set to false in optimized mode.
    mutable bool        m_do_checks;
    /// Flag for subclasses to know whether or not to perform
    /// neighbour initialization
    mutable bool        m_do_neighbours;

    /// pointer to the message service
    IMessageSvc*        m_msgSvc;

    /// If true, suppress DEBUG/INFO messages.
    bool m_quiet;


    /// List of dictionary names used by this helper
    std::vector<std::string>  m_dict_names;

    /// List of dictionary file names used by this helper
    std::vector<std::string>  m_file_names;

    /// List of dictionary versions used by this helper
    std::vector<std::string>  m_dict_tags;
private:

    typedef Identifier::size_type                       size_type; 
    
    // Identifiear numbering:
    enum        ID   { 
                        FASER_ID		= static_cast<Identifier::value_type>(0),
                        MAX_BIT 		= Identifier::MAX_BIT,
            	        ALL_BITS		= Identifier::ALL_BITS };

    int                 initLevelsFromDict(const IdDictMgr& dict_mgr);

    // dictionary version
    std::string         m_dict_version;
    bool                m_is_initialized_from_dict;
    size_type           m_DET_INDEX;
    size_type           m_SUBDET_INDEX;
    int                 m_NEUTRINO_ID;     
    int                 m_SCINT_ID;     
    int                 m_TRACKER_ID;       
    int                 m_CALO_ID;
    int                 m_EMULSION_ID;
    int                 m_VETO_ID;     
    int                 m_VETONU_ID;     
    int                 m_TRIGGER_ID;       
    int                 m_PRESHOWER_ID;       
    int                 m_SCT_ID;
    int                 m_ECAL_ID;

    /// Flag for slhc layout:
    bool                m_isSLHC;

    IdDictDictionary*   m_faser_dict;
    IdDictDictionary*   m_neutrino_dict;
    IdDictDictionary*   m_scint_dict;
    IdDictDictionary*   m_tracker_dict;
    IdDictDictionary*   m_calo_dict;
    FaserDetectorIDHelper* m_helper;
    IdDictFieldImplementation m_det_impl;
    IdDictFieldImplementation m_neutrino_part_impl;
    IdDictFieldImplementation m_scint_part_impl;
    IdDictFieldImplementation m_tracker_part_impl;
    IdDictFieldImplementation m_calo_part_impl;
};



//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>

//using the macros below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF(FaserDetectorID, 125694213 , 1)

/////////////////////////////////////////////////////////////////////////////
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
/////////////////////////////////////////////////////////////////////////////

inline ExpandedIdentifier          
FaserDetectorID::neutrino_exp           (void) const
{
    ExpandedIdentifier result;
    return (result << m_NEUTRINO_ID);
}


inline ExpandedIdentifier          
FaserDetectorID::scint_exp           (void) const
{
    ExpandedIdentifier result;
    return (result << m_SCINT_ID);
}

inline ExpandedIdentifier          
FaserDetectorID::tracker_exp             (void) const
{
    ExpandedIdentifier result;
    return (result << m_TRACKER_ID);  
}

inline ExpandedIdentifier          
FaserDetectorID::calo_exp(void) const
{
    ExpandedIdentifier result;
    return (result << m_CALO_ID);
}

inline ExpandedIdentifier          
FaserDetectorID::emulsion_exp           (void) const
{
    ExpandedIdentifier result(neutrino_exp());
    return (result << m_EMULSION_ID);
}

inline ExpandedIdentifier          
FaserDetectorID::veto_exp           (void) const
{
    ExpandedIdentifier result(scint_exp());
    return (result << m_VETO_ID);
}

inline ExpandedIdentifier          
FaserDetectorID::vetonu_exp           (void) const
{
    ExpandedIdentifier result(scint_exp());
    return (result << m_VETONU_ID);
}

inline ExpandedIdentifier          
FaserDetectorID::trigger_exp             (void) const
{
    ExpandedIdentifier result(scint_exp());
    return (result << m_TRIGGER_ID);  
}

inline ExpandedIdentifier          
FaserDetectorID::preshower_exp             (void) const
{
    ExpandedIdentifier result(scint_exp());
    return (result << m_PRESHOWER_ID);  
}

inline ExpandedIdentifier          
FaserDetectorID::sct_exp                   (void) const
{
    ExpandedIdentifier result(tracker_exp());
    return (result << m_SCT_ID);  
}

inline ExpandedIdentifier          
FaserDetectorID::ecal_exp                (void) const
{
    ExpandedIdentifier result(calo_exp());
    return (result << m_ECAL_ID);  
}

inline int                 
FaserDetectorID::neutrino_field_value     () const {return (m_NEUTRINO_ID);}     

inline int                 
FaserDetectorID::scint_field_value        () const {return (m_SCINT_ID);}     

inline int                 
FaserDetectorID::tracker_field_value      () const {return (m_TRACKER_ID);}

inline int                 
FaserDetectorID::calo_field_value         () const {return (m_CALO_ID);}

inline int                 
FaserDetectorID::emulsion_field_value     () const {return (m_EMULSION_ID);}     

inline int                 
FaserDetectorID::veto_field_value         () const {return (m_VETO_ID);}     

inline int                 
FaserDetectorID::vetonu_field_value       () const {return (m_VETONU_ID);}     

inline int                 
FaserDetectorID::trigger_field_value      () const {return (m_TRIGGER_ID);}       

inline int                 
FaserDetectorID::preshower_field_value    () const {return (m_PRESHOWER_ID);}       

inline int                 
FaserDetectorID::sct_field_value          () const {return (m_SCT_ID);}       

inline int                 
FaserDetectorID::ecal_field_value         () const {return (m_ECAL_ID);}       

inline bool               
FaserDetectorID::is_neutrino             (Identifier id) const
{
    return (m_det_impl.unpack(id) == m_NEUTRINO_ID);
}

inline bool               
FaserDetectorID::is_scint                (Identifier id) const
{
    return (m_det_impl.unpack(id) == m_SCINT_ID);
}

inline bool               
FaserDetectorID::is_tracker              (Identifier id) const
{
    return (m_det_impl.unpack(id) == m_TRACKER_ID);
}

inline bool                
FaserDetectorID::is_calo                 (Identifier id) const
{
    return (m_det_impl.unpack(id) == m_CALO_ID);
}

inline bool               
FaserDetectorID::is_emulsion     (Identifier id) const
{
    bool result = false;
    if(is_neutrino(id)) {
        result = (m_neutrino_part_impl.unpack(id) == m_EMULSION_ID);
    }
    return result;
}

inline bool               
FaserDetectorID::is_veto       (Identifier id) const
{
    bool result = false;
    if(is_scint(id)) {
        result = (m_scint_part_impl.unpack(id) == m_VETO_ID);
    }
    return result;
}

inline bool               
FaserDetectorID::is_vetonu     (Identifier id) const
{
    bool result = false;
    if(is_scint(id)) {
        result = (m_scint_part_impl.unpack(id) == m_VETONU_ID);
    }
    return result;
}

inline bool               
FaserDetectorID::is_trigger     (Identifier id) const
{
    bool result = false;
    if(is_scint(id)) { 
	result = (m_scint_part_impl.unpack(id) == m_TRIGGER_ID);
    }
    return result;
}

inline bool               
FaserDetectorID::is_preshower         (Identifier id) const
{
    bool result = false;
    if(is_scint(id)) {
        result = (m_scint_part_impl.unpack(id) == m_PRESHOWER_ID);
    }
    return result;
}

inline bool               
FaserDetectorID::is_sct              (Identifier id) const
{
    bool result = false;
    if(is_tracker(id)) {
        result = (m_tracker_part_impl.unpack(id) == m_SCT_ID);
    }
    return result;
}

inline bool               
FaserDetectorID::is_ecal             (Identifier id) const
{
    bool result = false;
    if(is_calo(id)) {
        result = (m_calo_part_impl.unpack(id) == m_ECAL_ID);
    }
    return result;
}

#endif // FASERDETDESCR_FASERDETECTORID_H
