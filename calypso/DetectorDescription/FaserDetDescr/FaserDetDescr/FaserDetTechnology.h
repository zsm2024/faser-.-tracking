/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserDetTechnology.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASERDETDESCR_FASERDETTECHNOLOGY_H
#define FASERDETDESCR_FASERDETTECHNOLOGY_H 1

#include <assert.h>

// use these macros to check a given FaserDetTechnology for its validity

#define validFaserDetTechnology(detTechn) ( (detTechn<FaserDetDescr::fNumFaserDetTechnologies) && (detTechn>=FaserDetDescr::fFirstFaserDetTechnology) )
#define assertFaserDetTechnology(detTechn) ( assert(validFaserDetTechnology(detTechn)) )

namespace FaserDetDescr {

 /** @enum FaserDetTechnology
   
     A simple enum of FASER detector technologies.

   */

   enum FaserDetTechnology {       
        // Unset
            fUndefined                  = 0,
        // first Geometry element in enum, used in e.g. loops
            fFirstFaserDetTechnology    = 1,
        // Neutrino
            fFirstFaserNeutrinoTechnology = 1,
            fFaserEmulsion                = 1,
            fLastFaserNeutrinoTechnology  = 1,
        // Scintillator
            fFirstFaserScintillatorTechnology  = 2,
            fFaserVetoNu                       = 2,
            fFaserVeto                         = 3,
            fFaserTrigger                      = 4,
            fFaserPreshower                    = 5,
            fLastFaserScintillatorTechnology   = 5,
        // Tracker
            fFirstFaserTrackerTechnology = 6,
            fFaserSCT                    = 6,
            fLastFaserTrackerTechnology  = 6,
        // Calorimeter
            fFirstFaserCalorimeterTechnology    = 7,
            fFaserECAL                          = 7,
            fLastFaserCalorimeterTechnology     = 7,
        // number of defined detector technologies
            fNumFaserDetTechnologies    = 8
   };

} // end of namespace

#endif // FASERDETDESCR_FASERDETTECHNOLOGY
