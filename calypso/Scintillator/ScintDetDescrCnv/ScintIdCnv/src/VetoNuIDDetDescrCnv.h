/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Scint DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

#ifndef SCINTIDCNV_VETONUIDDETDESCRCNV_H
#define SCINTIDCNV_VETONUIDDETDESCRCNV_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class VetoNuID;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>


/**
 **  This class is a converter for the VetoNuID an IdHelper which is
 **  stored in the detector store. This class derives from
 **  DetDescrConverter which is a converter of the DetDescrCnvSvc.
 **
 **/

class VetoNuIDDetDescrCnv: public DetDescrConverter {

public:
    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    static long storageType();
    static const CLID& classID();

    VetoNuIDDetDescrCnv(ISvcLocator* svcloc);

private:
    /// The helper - only will create it once
    VetoNuID*       m_vetonuId;

    /// File to be read for Scint ids
    std::string   m_scintIDFileName;

    /// Tag of RDB record for Scint ids
    std::string   m_scintIdDictTag;

    /// Internal Scint id tag
    std::string   m_scintIDTag;

    /// Whether or not 
    bool          m_doChecks;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // SCINTIDCNV_VETONUIDDETDESCRCNV_H
