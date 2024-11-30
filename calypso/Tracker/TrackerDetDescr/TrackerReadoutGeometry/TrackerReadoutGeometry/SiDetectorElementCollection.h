/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiDetectorElementCollection.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef TRACKERREADOUTGEOMETRY_SIDETECTORELEMENTCOLLECTION_H
#define TRACKERREADOUTGEOMETRY_SIDETECTORELEMENTCOLLECTION_H

#include <vector>

class IdentifierHash;

namespace TrackerDD {

    class SiDetectorElement;

    /** @class SiDetectorElementCollection
      
       Class to hold the SiDetectorElement objects to be put in the detector store

       @author Grant Gorfine
    */

    class SiDetectorElementCollection : public std::vector<SiDetectorElement *> {
     public:
      ~SiDetectorElementCollection();
      const SiDetectorElement* getDetectorElement(const IdentifierHash& hash) const;
    };

} // namespace TrackerDD

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( TrackerDD::SiDetectorElementCollection , 1278082324 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( TrackerDD::SiDetectorElementCollection, 1336075348 );

#endif // TRACKERREADOUTGEOMETRY_SIDETECTORELEMENTCOLLECTION_H
