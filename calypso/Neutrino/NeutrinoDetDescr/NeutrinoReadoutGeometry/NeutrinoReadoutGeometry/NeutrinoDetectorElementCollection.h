/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NeutrinoDetectorElementCollection.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef NEUTRINOREADOUTGEOMETRY_NEUTRINODETECTORELEMENTCOLLECTION_H
#define NEUTRINOREADOUTGEOMETRY_NEUTRINODETECTORELEMENTCOLLECTION_H

#include <vector>

class IdentifierHash;

namespace NeutrinoDD {

    class NeutrinoDetectorElement;

    /** @class NeutrinoDetectorElementCollection
      
       Class to hold the NeutrinoDetectorElement objects to be put in the detector store

       @author Grant Gorfine
       @author Dave Casper
    */

    class NeutrinoDetectorElementCollection : public std::vector<NeutrinoDetectorElement *> {
     public:
      ~NeutrinoDetectorElementCollection();
      const NeutrinoDetectorElement* getDetectorElement(const IdentifierHash& hash) const;
    };

} // namespace NeutrinoDD

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( NeutrinoDD::NeutrinoDetectorElementCollection , 1277872151 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( NeutrinoDD::NeutrinoDetectorElementCollection, 1081260291 );

#endif // NEUTRINOREADOUTGEOMETRY_NEUTRINODETECTORELEMENTCOLLECTION_H
