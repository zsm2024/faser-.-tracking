/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Calo DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

#ifndef CALOIDCNV_ECALIDDETDESCRCNV_H
#define CALOIDCNV_ECALIDDETDESCRCNV_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class EcalID;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>


/**
 **  This class is a converter for the EcalID an IdHelper which is
 **  stored in the detector store. This class derives from
 **  DetDescrConverter which is a converter of the DetDescrCnvSvc.
 **
 **/

class EcalIDDetDescrCnv: public DetDescrConverter {

public:
    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    static long storageType();
    static const CLID& classID();

    EcalIDDetDescrCnv(ISvcLocator* svcloc);

private:
    /// The helper - only will create it once
    EcalID*       m_ecalId;

    /// File to be read for Scint ids
    std::string   m_caloIDFileName;

    /// Tag of RDB record for Scint ids
    std::string   m_caloIdDictTag;

    /// Internal Scint id tag
    std::string   m_caloIDTag;

    /// Whether or not 
    bool          m_doChecks;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // CALOIDCNV_ECALIDDETDESCRCNV_H
