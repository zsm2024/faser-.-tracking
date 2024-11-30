/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

#ifndef FASERSCT_SPACEPOINTCONTAINER_P0_H
#define FASERSCT_SPACEPOINTCONTAINER_P0_H

#include <vector>
#include "TrackerEventTPCnv/FaserSCT_SpacePoint_p0.h"
#include "TrackerEventTPCnv/FaserSCT_SpacePointCollection_p0.h"

class FaserSCT_SpacePointContainer_p0 {
 public:
  FaserSCT_SpacePointContainer_p0();
  friend class FaserSCT_SpacePointContainerCnv_p0;
 private:
  std::vector<FaserSCT_SpacePoint_p0> m_spacepoints;
  std::vector<FaserSCT_SpacePointCollection_p0> m_spacepoint_collections;
};

inline FaserSCT_SpacePointContainer_p0::FaserSCT_SpacePointContainer_p0() {
  m_spacepoints.clear();
  m_spacepoint_collections.clear();
}

#endif  // SPACEPOINTCONTAINER_P0_H
