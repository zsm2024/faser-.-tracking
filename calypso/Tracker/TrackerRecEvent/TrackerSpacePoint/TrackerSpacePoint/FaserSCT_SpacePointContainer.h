/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpacePointContaine.h
//   Header file for class SpacePointContainer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Container for SpacePoints  collections for InnerDetector and Muons
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRACKERSPACEPOINT_FASERSCT_SPACEPOINTCONTAINER_H
#define TRACKERSPACEPOINT_FASERSCT_SPACEPOINTCONTAINER_H

// Base classes
#include "EventContainers/IdentifiableContainer.h"
//Needed Classes
#include "TrackerSpacePoint/FaserSCT_SpacePointCollection.h"


typedef EventContainers::IdentifiableCache< FaserSCT_SpacePointCollection > FaserSCT_SpacePointCache;

class FaserSCT_SpacePointContainer 
: public IdentifiableContainer<FaserSCT_SpacePointCollection>{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  FaserSCT_SpacePointContainer(unsigned int max);
  
  FaserSCT_SpacePointContainer(FaserSCT_SpacePointCache*);

  // Destructor:
  virtual ~FaserSCT_SpacePointContainer();

   /** return class ID */
  static const CLID& classID() 
    {
      static const CLID id = 1306592743; 
      return id; 
      // we do not know why using the traits does not work
      //return ClassID_traits<SpacePointContainer>::ID();
    }

   /** return class ID */
  virtual const CLID& clID() const
    {
      return classID();
    }


  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////



  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  FaserSCT_SpacePointContainer() = delete;
  FaserSCT_SpacePointContainer(const FaserSCT_SpacePointContainer&) = delete;
  FaserSCT_SpacePointContainer &operator=(const FaserSCT_SpacePointContainer&) = delete;
  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

};
///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(FaserSCT_SpacePointContainer,1306592743,1)


CLASS_DEF( FaserSCT_SpacePointCache , 180888152, 1 )


#endif // TRKSPACEPOINT_SPACEPOINTCONTAINER_H
