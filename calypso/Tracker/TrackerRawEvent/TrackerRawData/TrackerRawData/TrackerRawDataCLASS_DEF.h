/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackerRawDataCLASS_DEF.h
//   Header file for class TrackerRawDataCLASS_DEF
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the CLASS_DEF for Containers and Collections
///////////////////////////////////////////////////////////////////
// Version 1.0 25/09/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRACKERRAWDATA_TRACKERRAWDATACLASS_DEF_H
#define TRACKERRAWDATA_TRACKERRAWDATACLASS_DEF_H

// Include all headers here - just the containers and collections are
// enough
#include "TrackerRawData/FaserSCT_RDO_Container.h"

#include "TrackerRawData/FaserSCT_RDO_Collection.h"

namespace {
    // Need this to read old data to create the dict entries
    DataVector<TrackerRawDataCollection<FaserSCT1_RawData> >     type2;
    DataVector<FaserSCT1_RawData>                                type4;
    TrackerRawDataCollection< FaserSCT1_RawData >                type6;
    TrackerRawDataCollection< FaserSCT_RDORawData >              type7;
}


#endif // TRACKERRAWDATA_TRACKERRAWDATACLASS_DEF_H
