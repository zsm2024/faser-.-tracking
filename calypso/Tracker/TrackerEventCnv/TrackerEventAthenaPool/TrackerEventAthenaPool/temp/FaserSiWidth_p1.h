/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSI_WIDTH_P1_TRK_H
#define FASERSI_WIDTH_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   FaserSiWidth_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"


    class FaserSiWidth_p1
    {
   public:
      FaserSiWidth_p1() {}

      float 	x{};
      float 	y{};
      // Trk::LocalPosition_p1 *
      TPObjRef	m_phirzWidth;
    };
#endif // SI_WIDTH_TRK_H
