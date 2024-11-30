/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERPRD_CONTAINER_P1_H
#define TRACKERPRD_CONTAINER_P1_H 1

#include <vector>
#include <string>
#include "TrackerEventTPCnv/TrackerPrepRawData/TrackerPRD_Collection_p1.h"

namespace Tracker {
class TrackerPRD_Container_p1   
{
 public:
/// Default constructor
  TrackerPRD_Container_p1 ();
  //private:
  std::vector<Tracker::TrackerPRD_Collection_p1>  m_collections;
  std::vector<TPObjRef>             m_PRD;
};

} 


// inlines
 
inline Tracker::TrackerPRD_Container_p1::TrackerPRD_Container_p1() {}
 
#endif
