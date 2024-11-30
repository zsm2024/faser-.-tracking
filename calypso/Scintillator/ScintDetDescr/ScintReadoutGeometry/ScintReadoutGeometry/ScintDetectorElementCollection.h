/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ScintDetectorElementCollection.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef SCINTREADOUTGEOMETRY_SCINTDETECTORELEMENTCOLLECTION_H
#define SCINTREADOUTGEOMETRY_SCINTDETECTORELEMENTCOLLECTION_H

#include <vector>

class IdentifierHash;

namespace ScintDD {

    class ScintDetectorElement;

    /** @class ScintDetectorElementCollection
      
       Class to hold the ScintDetectorElement objects to be put in the detector store

       @author Grant Gorfine
       @author Dave Casper
    */

    class ScintDetectorElementCollection : public std::vector<ScintDetectorElement *> {
     public:
      ~ScintDetectorElementCollection();
      const ScintDetectorElement* getDetectorElement(const IdentifierHash& hash) const;
    };

} // namespace ScintDD

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( ScintDD::ScintDetectorElementCollection , 1090173979 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( ScintDD::ScintDetectorElementCollection, 1189378543 );

#endif // SCINTREADOUTGEOMETRY_SCINTDETECTORELEMENTCOLLECTION_H
