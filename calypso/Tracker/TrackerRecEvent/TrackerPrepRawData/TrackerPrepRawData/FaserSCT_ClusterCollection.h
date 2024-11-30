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

#ifndef FASERSCT_CLUSTERCOLLECTION_H
#define FASERSCT_CLUSTERCOLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "TrkPrepRawData/PrepRawDataCollection.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"

// Containers
// numbers obtained using clid.db
namespace Tracker {
typedef Trk::PrepRawDataCollection< FaserSCT_Cluster > FaserSCT_ClusterCollection;

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const FaserSCT_ClusterCollection& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const FaserSCT_ClusterCollection& coll);
}
CLASS_DEF(Tracker::FaserSCT_ClusterCollection, 1122353123, 1)

#endif // TRKPREPRAWDATA_PREPRAWDATACLASS_DEF_H
