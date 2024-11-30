/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERRSI_WIDTH_CNV_P2_H
#define FASERRSI_WIDTH_CNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   FaserSiWidthCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "TrackerPrepRawData/FaserSiWidth.h"
#include "TrackerEventAthenaPool/FaserSiWidth_p2.h"

#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"

class MsgStream;


    class FaserSiWidthCnv_p2
       : public T_AthenaPoolTPCnvBase<FaserSiWidth, FaserSiWidth_p2>
    {
    public:
      FaserSiWidthCnv_p2()  {}

      void persToTrans( const FaserSiWidth_p2 *, FaserSiWidth *, MsgStream & );
      void transToPers( const FaserSiWidth *, FaserSiWidth_p2 *, MsgStream & );

    private:
      LocalPositionCnv_p1	m_localPosCnv;
            
    };
#endif // SI_WIDTH_CNV_P1_H
