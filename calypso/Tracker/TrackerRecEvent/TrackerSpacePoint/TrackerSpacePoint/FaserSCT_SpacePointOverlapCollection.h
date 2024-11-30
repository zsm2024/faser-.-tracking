/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpacePointCollection.h
//   Header file for class SpacePointCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the overlap SPs, the other ones are in 
// the SpacePointCollection
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRACKERSPACEPOINT_FASERSCTSPACEPOINTOVERLAPCOLLECTION_H
#define TRACKERSPACEPOINT_FASERSCTSPACEPOINTOVERLAPCOLLECTION_H

// Base classes
#include "AthContainers/DataVector.h"
#include "GaudiKernel/DataObject.h"
//class SpacePoint;
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"

class FaserSCT_SpacePointOverlapCollection : public DataVector< Tracker::FaserSCT_SpacePoint >{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor 
  FaserSCT_SpacePointOverlapCollection();

  // Destructor:
  virtual ~FaserSCT_SpacePointOverlapCollection();


  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////


  
  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  //  FaserSCT_SpacePointOverlapCollection();
  FaserSCT_SpacePointOverlapCollection(const FaserSCT_SpacePointOverlapCollection&) = delete;
  FaserSCT_SpacePointOverlapCollection &operator=(const FaserSCT_SpacePointOverlapCollection&) = delete;

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  

};
///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(FaserSCT_SpacePointOverlapCollection,1114023127, 1)


#endif // TRKSPACEPOINT_SPACEPOINTOVERLAPCOLLECTION_H
