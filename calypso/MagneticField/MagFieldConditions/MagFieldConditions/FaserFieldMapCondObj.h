/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MAGFIELDCONDITIONS_FASERFIELDMAPCONDOBJ
#define MAGFIELDCONDITIONS_FASERFIELDMAPCONDOBJ

// MagField includes
#include "AthenaKernel/CondCont.h" 
#include "MagFieldElements/FaserFieldMap.h"

class FaserFieldMapCondObj {

public:
    FaserFieldMapCondObj();

    ~FaserFieldMapCondObj();

    // access to field map 
    const MagField::FaserFieldMap* fieldMap () const;

    // setter
    void setFieldMap(std::unique_ptr<MagField::FaserFieldMap> fieldMap);
    
private:
    // field map 
    std::unique_ptr<MagField::FaserFieldMap> m_fieldMap;
};


#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( FaserFieldMapCondObj, 218239424, 1)
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF (FaserFieldMapCondObj, 16498296);


#endif // MAGFIELDCONDITIONS_FASERFIELDMAPCONDOBJ

