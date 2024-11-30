/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Neutrino DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

#ifndef NEUTRINOIDCNV_EMULSIONIDDETDESCRCNV_H
#define NEUTRINOIDCNV_EMULSIONIDDETDESCRCNV_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class EmulsionID;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>


/**
 **  This class is a converter for the EmulsionID an IdHelper which is
 **  stored in the detector store. This class derives from
 **  DetDescrConverter which is a converter of the DetDescrCnvSvc.
 **
 **/

class EmulsionIDDetDescrCnv: public DetDescrConverter {

public:
    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    static long storageType();
    static const CLID& classID();

    EmulsionIDDetDescrCnv(ISvcLocator* svcloc);

private:
    /// The helper - only will create it once
    EmulsionID*       m_emulsionId;

    /// File to be read for Neutrino ids
    std::string   m_neutrinoIDFileName;

    /// Tag of RDB record for Neutrino ids
    std::string   m_neutrinoIdDictTag;

    /// Internal Neutrino id tag
    std::string   m_neutrinoIDTag;

    /// Whether or not 
    bool          m_doChecks;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // NEUTRINOIDCNV_EMULSIONIDDETDESCRCNV_H
