/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_CLUSTERCONTAINER_TLP1_H
#define FASERSCT_CLUSTERCONTAINER_TLP1_H




//-----------------------------------------------------------------------------
// InDetPrepRawData
//-----------------------------------------------------------------------------
#include "TrackerEventTPCnv/TrackerPrepRawData/TrackerPRD_Container_p1.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/FaserSCT_Cluster_p1.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/TrackerCluster_p1.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/FaserSiWidth_p1.h"
#include "TrkEventTPCnv/TrkPrepRawData/PrepRawData_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPosition_p1.h"


// Token
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

namespace Tracker
{
   class FaserSCT_ClusterContainer_tlp1
   {
  public:
     FaserSCT_ClusterContainer_tlp1() {}
     TPCnvTokenList_p1		             m_tokenList;

// This is the basic info
     std::vector< Tracker::TrackerPRD_Container_p1>  m_sctDCCont;
     std::vector< Tracker::FaserSCT_Cluster_p1 >   m_sctClusters;
// This are the base classes
     std::vector< Tracker::TrackerCluster_p1 >     m_siClusters;
     std::vector< Tracker::FaserSiWidth_p1 >       m_siWidths;
     std::vector< Trk::PrepRawData_p1>             m_prepData;
// This is info held by the base class
     std::vector< Trk::LocalPosition_p1>           m_locPos;
     std::vector< Trk::HepSymMatrix_p1>            m_errorMat;

   };
}


#endif
