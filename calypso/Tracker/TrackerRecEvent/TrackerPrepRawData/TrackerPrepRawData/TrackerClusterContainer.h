/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackerClusterContainer.h
//   Header file for class TrackerClusterContainer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRACKERPREPRAWDATA_TRACKERCLUSTERCONTAINER_H
#define TRACKERPREPRAWDATA_TRACKERCLUSTERCONTAINER_H

#include "AthenaKernel/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataContainer.h"
#include "TrackerPrepRawData/TrackerClusterCollection.h"

// Containers
// numbers obtained using clid.db
namespace Tracker {
typedef Trk::PrepRawDataContainer< TrackerClusterCollection > TrackerClusterContainer; 
}

CLASS_DEF(Tracker::TrackerClusterContainer,1281428575,1)



// // Base classes
// #include "EventContainers/IdentifiableContainer.h"

// namespace Tracker
// {
// template<class CollectionT>
// class TrackerClusterContainer 
//     : public IdentifiableContainer<CollectionT>{
    
//     ///////////////////////////////////////////////////////////////////
//     // Public methods:
//     ///////////////////////////////////////////////////////////////////
//     public:
    
//     /** Constructor with parameters:*/
//     TrackerClusterContainer(unsigned int max);
    
//     /** Constructor with External Cache*/
//     TrackerClusterContainer(EventContainers::IdentifiableCache<CollectionT>*);

//     /**default ctor - for POOL only*/
//     TrackerClusterContainer();
    
//     /** Destructor:*/
//     virtual ~TrackerClusterContainer();
    
//     /** return class ID */
//     static const CLID& classID();
    
//     /** return class ID */
//     virtual const CLID& clID() const;
    
//     private:
    
//     TrackerClusterContainer(const TrackerClusterContainer&);
//     TrackerClusterContainer &operator=(const TrackerClusterContainer&);

// };
// // member functions that use Collection T
// #include"TrackerPrepRawData/TrackerClusterContainer.icc"
// }

#endif // TRACKERPREPRAWDATA_TRACKERCLUSTERCONTAINER_H

