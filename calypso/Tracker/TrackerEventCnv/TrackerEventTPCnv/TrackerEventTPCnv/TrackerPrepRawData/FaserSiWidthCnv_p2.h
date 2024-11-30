/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSI_WIDTH_CNV_P2_H
#define FASERSI_WIDTH_CNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   FaserSiWidthCnv_p2.h
//
//-----------------------------------------------------------------------------

#include "TrackerPrepRawData/FaserSiWidth.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/FaserSiWidth_p2.h"

#include "TrkEventTPCnv/TrkEventPrimitives/LocalPositionCnv_p1.h"

class MsgStream;

namespace Tracker
{

    class FaserSiWidthCnv_p2
       : public T_AthenaPoolTPCnvBase<Tracker::FaserSiWidth, Tracker::FaserSiWidth_p2>
    {
    public:
      FaserSiWidthCnv_p2()  {}

      void persToTrans( const Tracker::FaserSiWidth_p2 *, Tracker::FaserSiWidth *, MsgStream & );
      void transToPers( const Tracker::FaserSiWidth *, Tracker::FaserSiWidth_p2 *, MsgStream & );

    private:
      LocalPositionCnv_p1	m_localPosCnv;
            
    };
}
#endif // FASERSI_WIDTH_CNV_P1_H
