/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERRAWDATACONTAINER_P2_H
#define TRACKERRAWDATACONTAINER_P2_H

/*

Persistent represenation of an TrackerRawDataContainer.
This is used for all the RDO conainers with base RDO objects.
The converter will take care of creating TRT/SCT/Pixel
RDO containers out of it. 

The assumption is that objects contained in a container
are all of the same type, eg containers are hompgeneous.
This persistent container should only be used for objects
that contain TrackerRawData_p2. 

Author: Davide Costanzo

*/

#include <vector>
#include <string>
#include "TrackerEventAthenaPool/TrackerRawData_p2.h"
#include "TrackerEventAthenaPool/TrackerRawDataCollection_p1.h"


class TrackerRawDataContainer_p2   
{
 public:
/// Default constructor
  TrackerRawDataContainer_p2 ();
  friend class FaserSCT1_RawDataContainerCnv_p2;
 private:
  std::vector<TrackerRawDataCollection_p1>  m_collections;
  std::vector<TrackerRawData_p2>            m_rawdata;
};


// inlines

inline
TrackerRawDataContainer_p2::TrackerRawDataContainer_p2 () {}

#endif
