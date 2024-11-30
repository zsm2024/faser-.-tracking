/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MAGFIELDCONDITIONS_FASERFIELDCACHECONDOBJ
#define MAGFIELDCONDITIONS_FASERFIELDCACHECONDOBJ

// MagField includes
#include "AthenaKernel/CondCont.h" 
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldElements/FaserFieldCache.h"

// forward declarations
namespace MagField {
    class FaserFieldMap;
}


class FaserFieldCacheCondObj {

public:
    FaserFieldCacheCondObj();

    ~FaserFieldCacheCondObj();

    /** get B field cache for evaluation as a function of 2-d (solenoid) or 3-d (toroid) position. 
        Resets cache to an initialized state */
    inline void getInitializedCache (MagField::FaserFieldCache& cache) const;

    double dipoleFieldScaleFactor() const { return m_dipoleFieldScale; }

    /** set values for field scale and service to be able to build the cache **/
    bool initialize(double dipoleFieldScale,  
                    const MagField::FaserFieldMap* fieldMap);

private:
    /// Temporary flag for switching between 'old' and 'new' magField usage
    double                           m_dipoleFieldScale{1};
    const MagField::FaserFieldMap*   m_fieldMap{nullptr};
};

// inline function(s)

void
FaserFieldCacheCondObj::getInitializedCache (MagField::FaserFieldCache& cache) const
{

    // setup with field scale and magnetic field service for first access to field */
    cache = MagField::FaserFieldCache(m_dipoleFieldScale, m_fieldMap);
}


#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( FaserFieldCacheCondObj, 246495246, 1)
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF (FaserFieldCacheCondObj, 90336330);


#endif // MAGFIELDCONDITIONS_FASERFIELDCACHECONDOBJ

