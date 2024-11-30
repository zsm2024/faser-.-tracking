/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FaserSCT_CLUSTERCONTAINER_TLP1_H
#define FaserSCT_CLUSTERCONTAINER_TLP1_H




//-----------------------------------------------------------------------------
// InDetPrepRawData
//-----------------------------------------------------------------------------
#include "TrackerEventAthenaPool/TrackerPRD_Container_p1.h"
#include "TrackerEventAthenaPool/FaserSCT_Cluster_p3.h"
//#include "TrackerEventAthenaPool/TrackerCluster_p1.h"
#include "TrackerEventAthenaPool/FaserSiWidth_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepSymMatrix_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalPosition_p1.h"


// Token
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

   class FaserSCT_ClusterContainer_tlp1
   {
  public:
     FaserSCT_ClusterContainer_tlp1() {}
     TPCnvTokenList_p1		             m_tokenList;

// This is the basic info
     std::vector< TrackerPRD_Container_p1>  m_sctDCCont;
     std::vector< FaserSCT_Cluster_p3 >          m_sctClusters;
// This are the base classes
//     std::vector< TrackerCluster_p1 >          m_siClusters;
     std::vector< FaserSiWidth_p1 >          m_siWidths;
// This is info held by the base class
     std::vector< Trk::LocalPosition_p1>            m_locPos;
     std::vector< Trk::HepSymMatrix_p1>             m_errorMat;

   };


#endif
