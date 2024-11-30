/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSI_WIDTH_P2_TRK_H
#define FASERSI_WIDTH_P2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   FaserSiWidth_p2.h
//
//-----------------------------------------------------------------------------
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPosition_p1.h"


namespace Tracker
{

class FaserSiWidthCnv_p2;

class FaserSiWidth_p2
  {
  public:
    FaserSiWidth_p2() {};
    friend class FaserSiWidthCnv_p2;
  private:
    unsigned int m_size{};
    Trk::LocalPosition_p1 m_phirzWidth;   
  };

}
#endif // FASERSI_WIDTH_TRK_H
