/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

#ifndef FaserSCT_CLUSTER_P3_TRK_H
#define FaserSCT_CLUSTER_P3_TRK_H

//-----------------------------------------------------------------------------
//
// file:   FaserSCT_Cluster_p3.h
//
//-----------------------------------------------------------------------------
#include "FaserSiWidth_p2.h"

class FaserSCT_Cluster_p3
{
  public:

    // type used for channel id differences
    typedef  short rdo_diff_type;

    FaserSCT_Cluster_p3(): m_localPos{0.}, m_mat00{0.}, m_mat01{0.}, m_mat11{0.}, m_hitsInThirdTimeBin{0} {}

    // replace this:
    std::vector<rdo_diff_type >	        m_rdoList;
    float                                     m_localPos;
    float 					m_mat00;
    float 					m_mat01;
    float 					m_mat11;
    uint16_t                                  m_hitsInThirdTimeBin;

    //SiWidth_p2
    FaserSiWidth_p2			m_width;

};

#endif // SCT_CLUSTER_P3_TRK_H
