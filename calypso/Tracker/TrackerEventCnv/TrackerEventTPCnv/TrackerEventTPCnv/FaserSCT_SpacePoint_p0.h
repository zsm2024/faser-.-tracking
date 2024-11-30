/*
 Copyright 2021 CERN for the benefit of the FASER collaboration
*/

#ifndef FASERSCT_SPACEPOINT_P0_H
#define FASERSCT_SPACEPOINT_P0_H
#include "Identifier/IdentifierHash.h"
#include "DataModelAthenaPool/ElementLink_p1.h"

class FaserSCT_SpacePoint_p0 {
 public:
  FaserSCT_SpacePoint_p0();
  friend class FaserSCT_SpacePointCnv_p0;

 private:
  unsigned int m_idHash0;
  unsigned int m_idHash1;
  unsigned long long m_id0;
  unsigned long long m_id1;

  float m_localpos_x;
  float m_localpos_y;
  float m_localcov00;
  float m_localcov01;
  float m_localcov11;

  float m_pos_x;
  float m_pos_y;
  float m_pos_z;

  float m_cov00;
  float m_cov01;
  float m_cov02;
  float m_cov11;
  float m_cov12;
  float m_cov22;
  ElementLinkInt_p1 m_link1;
  ElementLinkInt_p1 m_link2;
};

inline
FaserSCT_SpacePoint_p0::FaserSCT_SpacePoint_p0() :
    m_idHash0(0),
    m_idHash1(0),
    m_id0(0),
    m_id1(0),
    m_localpos_x(0),
    m_localpos_y(0),
    m_localcov00(0),
    m_localcov01(0),
    m_localcov11(0),
    m_pos_x(0),
    m_pos_y(0),
    m_pos_z(0),
    m_cov00(0),
    m_cov01(0),
    m_cov02(0),
    m_cov11(0),
    m_cov12(0),
    m_cov22(0)
{}

#endif  // SPACEPOINT_P0_H
