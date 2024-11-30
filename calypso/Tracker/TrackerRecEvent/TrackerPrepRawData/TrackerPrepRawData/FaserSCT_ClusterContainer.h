/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PrepRawDataCLASS_DEF.h
//   Header file for class PrepRawDataCLASS_DEF
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the CLASS_DEF for Containers and Collections
///////////////////////////////////////////////////////////////////
// Version 1.0 25/09/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef FASERSCT_CLUSTERCONTAINER_H
#define FASERSCT_CLUSTERCONTAINER_H

#include "AthenaKernel/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataContainer.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "AthLinks/DeclareIndexingPolicy.h"

// Containers
// numbers obtained using clid.db
namespace Tracker
{
typedef Trk::PrepRawDataContainer< FaserSCT_ClusterCollection > FaserSCT_ClusterContainer;
typedef EventContainers::IdentifiableCache< FaserSCT_ClusterCollection > FaserSCT_ClusterContainerCache;
}
CLASS_DEF(Tracker::FaserSCT_ClusterContainer,1177445571,1)
CONTAINER_IS_IDENTCONT(Tracker::FaserSCT_ClusterContainer)

CLASS_DEF( Tracker::FaserSCT_ClusterContainerCache , 1224107226, 1 )

#endif // TRKPREPRAWDATA_PREPRAWDATACLASS_DEF_H
