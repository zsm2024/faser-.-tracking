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

#ifndef TRACKERCLUSTERCOLLECTION_H
#define TRACKERCLUSTERCOLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "TrackerRawData/TrackerRawDataCollection.h"
#include "TrackerPrepRawData/TrackerCluster.h"

// Containers
// numbers obtained using clid.db
namespace Tracker
{
typedef TrackerRawDataCollection<TrackerCluster> TrackerClusterCollection;

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const TrackerClusterCollection& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const TrackerClusterCollection& coll);
}
CLASS_DEF(Tracker::TrackerClusterCollection, 1130122227, 1)

#endif // TRACKERRAWDATA_PREPRAWDATACLASS_DEF_H
