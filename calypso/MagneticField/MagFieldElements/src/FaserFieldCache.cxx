/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// FaserFieldCache.cxx
//
// Local cache for magnetic field (based on MagFieldServices/FieldFieldSvcTLS.h)
//
// R.D.Schaffer -at- cern.ch
//
#include "MagFieldElements/FaserFieldCache.h"

/// Constructor
MagField::FaserFieldCache::FaserFieldCache(double scale,
                                           const FaserFieldMap* fieldMap)
    :
    // temporary flag
    m_scale(scale),
    // set field service
    m_fieldMap(fieldMap)

{

}
    

