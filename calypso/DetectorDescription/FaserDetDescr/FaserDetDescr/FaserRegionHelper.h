/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserRegionHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASERDETDESCR_FASERREGIONHELPER_H
#define FASERDETDESCR_FASERREGIONHELPER_H 1

#include "FaserDetDescr/FaserRegion.h"

namespace FaserDetDescr {

 /** @class FaserRegionHelper
   
     A simple helper class (functional) to translate FaserDetDesc::FaserRegion enums
     into a string description.

   */

   class FaserRegionHelper {       
     public:
       static const char * getName( int region );
       static const char * getName( FaserDetDescr::FaserRegion region );

     private:
       FaserRegionHelper();

   };

} // end of namespace

#endif // FASERDETDESCR_FASERREGIONHELPER_H
