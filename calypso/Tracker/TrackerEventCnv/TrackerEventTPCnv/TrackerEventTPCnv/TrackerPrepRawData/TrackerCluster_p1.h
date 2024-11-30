/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERCLUSTER_P1_TRK_H
#define TRACKERCLUSTER_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrackerCluster_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"


namespace Tracker
{
   class TrackerCluster_p1
    {
   public:
      TrackerCluster_p1() {}

      // base PrepRawData_p1
      TPObjRef		m_rawData;

      //FaserSiWidth_p1*
      TPObjRef		m_width;

    };
}

#endif // TRACKERCLUSTER_P1_TRK_H
