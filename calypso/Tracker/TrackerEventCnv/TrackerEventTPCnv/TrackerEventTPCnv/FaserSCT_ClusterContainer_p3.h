/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_CLUSTERCONTAINER_P3_H
#define FASERSCT_CLUSTERCONTAINER_P3_H

/*

Author: Davide Costanzo

*/

#include <vector>
#include <string>
#include "TrackerEventTPCnv/TrackerPrepRawData/FaserSCT_Cluster_p3.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/TrackerPRD_Collection_p2.h"
#include "Identifier/Identifier.h"


namespace Tracker{
class FaserSCT_ClusterContainer_p3   
{
 public:
/// Default constructor
  FaserSCT_ClusterContainer_p3 ();
  //private:
  std::vector<Tracker::TrackerPRD_Collection_p2>  m_collections;
  std::vector<Tracker::FaserSCT_Cluster_p3>       m_rawdata;
// The delta identifiers of the PRD:
  //std::vector<unsigned short>                 m_prdDeltaId;
  std::vector<Identifier::diff_type>               m_prdDeltaId;
};


// inlines

inline
FaserSCT_ClusterContainer_p3::FaserSCT_ClusterContainer_p3 () {}
}

#endif
