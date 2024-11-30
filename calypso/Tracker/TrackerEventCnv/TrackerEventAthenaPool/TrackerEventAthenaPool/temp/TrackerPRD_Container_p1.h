/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERPRD_CONTAINER_P1_H
#define TRACKERPRD_CONTAINER_P1_H 1

#include <vector>
#include <string>
#include "TrackerEventAthenaPool/TrackerPRD_Collection_p2.h"
#include "AthenaPoolUtilities/TPObjRef.h"

class TrackerPRD_Container_p1   
{
 public:
/// Default constructor
  TrackerPRD_Container_p1 ();
  //private:
  std::vector<TrackerPRD_Collection_p2>  m_collections;
  std::vector<TPObjRef>             m_PRD;
};


// inlines
 
inline TrackerPRD_Container_p1::TrackerPRD_Container_p1() {}
 
#endif
