/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackerRawDataContainer.h
//   Header file for class TrackerRawDataContainer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Container for Raw Data Objects collections for Pixel, SCT and TRT
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRACKERRAWDATA_TRACKERRAWDATACONTAINER_H
#define TRACKERRAWDATA_TRACKERRAWDATACONTAINER_H

// Base classes
#include "EventContainers/IdentifiableContainer.h"
//Needed Classes
#include "TrackerRawData/TrackerRawDataCollection.h"
#include "AthenaKernel/CLASS_DEF.h"

template<class CollectionT>
class TrackerRawDataContainer 
: public IdentifiableContainer<CollectionT>{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  TrackerRawDataContainer(unsigned int max);

  TrackerRawDataContainer(EventContainers::IdentifiableCache<CollectionT>*);
  
  // Destructor:
  virtual ~TrackerRawDataContainer();

   /** return class ID */
   static const CLID& classID() 
   {
     return ClassID_traits< TrackerRawDataContainer <CollectionT> > ::ID();
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

  TrackerRawDataContainer(const TrackerRawDataContainer&);
  TrackerRawDataContainer &operator=(const TrackerRawDataContainer&);

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

};
// member functions that use Collection T
#include"TrackerRawData/TrackerRawDataContainer.icc"
///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

#endif // TRACKERRAWDATA_TRACKERRAWDATACONTAINER_H
