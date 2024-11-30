/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserRegion.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASERDETDESCR_FASERREGION_H
#define FASERDETDESCR_FASERREGION_H 1

#include <assert.h>

// use these macros to check a given FaserRegion for its validity
#define validFaserRegion(region) ( (region<FaserDetDescr::fNumFaserRegions) && (region>=FaserDetDescr::fFirstFaserRegion) )
#define assertFaserRegion(region) ( assert(validFaserRegion(region)) )

namespace FaserDetDescr {

 /** @enum FaserRegion
   
     A simple enum of FASER regions and sub-detectors.

   */

   enum FaserRegion {       
        // Unset
            fUndefinedFaserRegion = 0,
        // first Geometry element in enum, used in e.g. loops
            fFirstFaserRegion     = 1,
        // FASER Detector setup: geometrical ones
            fFaserNeutrino        = 1,
            fFaserScintillator    = 2,
            fFaserTracker         = 3,
            fFaserDipole          = 4,  // this means in the metal of the magnet, not the bore
            fFaserCalorimeter     = 5,
            fFaserCavern          = 6,
            fFaserTrench          = 7,  // i.e. the concrete around the trench
        // number of defined GeoIDs
            fNumFaserRegions      = 8
   };

} // end of namespace

#endif // FASERDETDESCR_FASERREGION
