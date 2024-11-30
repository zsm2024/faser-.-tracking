/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloDetectorElementCollection.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef CALOREADOUTGEOMETRY_CALODETECTORELEMENTCOLLECTION_H
#define CALOREADOUTGEOMETRY_CALODETECTORELEMENTCOLLECTION_H

#include <vector>

class IdentifierHash;

namespace CaloDD {

    class CaloDetectorElement;

    /** @class CaloDetectorElementCollection
      
       Class to hold the CaloDetectorElement objects to be put in the detector store

       @author Grant Gorfine
       @author Dave Casper
    */

    class CaloDetectorElementCollection : public std::vector<CaloDetectorElement *> {
     public:
      ~CaloDetectorElementCollection();
      const CaloDetectorElement* getDetectorElement(const IdentifierHash& hash) const;
    };

} // namespace CaloDD

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( CaloDD::CaloDetectorElementCollection , 1131534083 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( CaloDD::CaloDetectorElementCollection, 1117233743 );

#endif // CALOREADOUTGEOMETRY_CALODETECTORELEMENTCOLLECTION_H
