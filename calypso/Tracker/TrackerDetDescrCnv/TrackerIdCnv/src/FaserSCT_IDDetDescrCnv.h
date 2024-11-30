/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tracker DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: FaserSCT_IDDetDescrCnv.h,v 1.3 2007-01-01 10:47:18 dquarrie Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef TRACKERDETDESCRCNV_FASERSCT_IDDETDESCRCNV_H
#define TRACKERDETDESCRCNV_FASERSCT_IDDETDESCRCNV_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class FaserSCT_ID;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>


/**
 **  This class is a converter for the SCT_ID an IdHelper which is
 **  stored in the detector store. This class derives from
 **  DetDescrConverter which is a converter of the DetDescrCnvSvc.
 **
 **/

class FaserSCT_IDDetDescrCnv: public DetDescrConverter {

public:
    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    // Storage type and class ID (used by CnvFactory)
    static long storageType();
    static const CLID& classID();

    FaserSCT_IDDetDescrCnv(ISvcLocator* svcloc);

private:
    /// The helper - only will create it once
    FaserSCT_ID*       m_sctId;

    /// File to be read for Tracker ids
    std::string   m_trackerIDFileName;

    /// Tag of RDB record for Tracker ids
    std::string   m_trackerIdDictTag;

    /// Internal Tracker id tag
    std::string   m_trackerIDTag;

    /// Whether or not 
    bool          m_doChecks;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // TRACKERDETDESCRCNV_FASERSCT_IDDETDESCRCNV_H
