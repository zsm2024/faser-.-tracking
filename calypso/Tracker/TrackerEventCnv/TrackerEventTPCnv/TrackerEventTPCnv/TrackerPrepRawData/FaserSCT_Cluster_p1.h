/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_CLUSTER_P1_TRK_H
#define FASERSCT_CLUSTER_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   SCT_Cluster_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"

namespace Tracker
{
   class FaserSCT_Cluster_p1
   {
  public:
     FaserSCT_Cluster_p1() {}
     // base
     TPObjRef	m_siCluster;
   };
}

#endif // FASERSCT_CLUSTER_P1_TRK_H
