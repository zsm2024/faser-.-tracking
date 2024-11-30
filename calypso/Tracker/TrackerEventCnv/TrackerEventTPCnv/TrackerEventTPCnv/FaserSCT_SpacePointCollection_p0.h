/*
 Copyright 2021 CERN for the benefit of the FASER collaboration
*/

#ifndef FASERSCT_SPACEPOINTCOLLECTION_P0_H
#define FASERSCT_SPACEPOINTCOLLECTION_P0_H

#include <vector>

class FaserSCT_SpacePointCollection_p0 {
 public:
  FaserSCT_SpacePointCollection_p0();
  friend class FaserSCT_SpacePointClusterCnv_p0;

  unsigned int m_idHash;
  unsigned long long m_id;
  unsigned int m_begin;
  unsigned int m_end;
  unsigned short m_size;
};

inline
FaserSCT_SpacePointCollection_p0::FaserSCT_SpacePointCollection_p0() : m_idHash(0),
m_id(0),
m_begin(0),
m_end(0),
m_size(0){ }

#endif  // SPACEPOINTCOLLECTION_P0_H
