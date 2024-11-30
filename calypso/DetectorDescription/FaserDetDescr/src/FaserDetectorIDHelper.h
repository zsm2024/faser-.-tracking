/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 
 -----------------------------------------
 ***************************************************************************/

#ifndef SRC_FASERDETECTORIDHELPER_H
# define SRC_FASERDETECTORIDHELPER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "Identifier/Identifier.h"
#include "Identifier/IdContext.h"
#include "Identifier/IdHelper.h"
#include <string>
#include <vector>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class IdDictField;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class FaserDetectorIDHelper
{
public:
    
    enum ERRORS { UNDEFINED = 999 };


    FaserDetectorIDHelper(void);

    typedef Identifier::size_type 			size_type; 

   
    /// Initialization from the identifier dictionary
  int         initialize_from_dictionary(const IdDictMgr& dict_mgr);

    ~FaserDetectorIDHelper(void);
    
    size_type   emulsion_region_index();
    size_type   veto_region_index();
    size_type   trigger_region_index();
    size_type   preshower_region_index();
    size_type   sct_region_index();
    size_type   ecal_region_index();

    void        setMsgSvc(IMessageSvc* msgSvc) { m_msgSvc = msgSvc; }


private:


    /// Flag for slhc layout:
    bool                m_isSLHC;
    size_type   m_emulsion_region_index;
    size_type		m_veto_region_index;
    size_type		m_trigger_region_index;
    size_type		m_preshower_region_index;
    size_type   m_sct_region_index;
    size_type   m_ecal_region_index;
    bool		m_initialized;
    /// pointer to the message service
    IMessageSvc*        m_msgSvc;
    
};



//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
inline FaserDetectorIDHelper::size_type   FaserDetectorIDHelper::emulsion_region_index()
{return (m_emulsion_region_index);}       

inline FaserDetectorIDHelper::size_type   FaserDetectorIDHelper::veto_region_index()
{return (m_veto_region_index);}       

inline FaserDetectorIDHelper::size_type   FaserDetectorIDHelper::trigger_region_index()
{return (m_trigger_region_index);}         

inline FaserDetectorIDHelper::size_type   FaserDetectorIDHelper::preshower_region_index()
{return (m_preshower_region_index);}         

inline FaserDetectorIDHelper::size_type   FaserDetectorIDHelper::sct_region_index()
{return (m_sct_region_index);}         

inline FaserDetectorIDHelper::size_type   FaserDetectorIDHelper::ecal_region_index()
{return (m_ecal_region_index);}         


#endif // SRC_FASERDETECTORIDHELPER_H
