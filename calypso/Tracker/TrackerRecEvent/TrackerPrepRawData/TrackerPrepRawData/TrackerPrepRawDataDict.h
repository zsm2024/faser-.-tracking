// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERPREPRAWDATADICT_H
#define TRACKERPREPRAWDATADICT_H 1

//#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"

#include "TrackerPrepRawData/TrackerCluster.h"
#include "TrackerPrepRawData/TrackerClusterCollection.h"
#include "TrackerPrepRawData/TrackerClusterContainer.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrkPrepRawData/PrepRawDataCollection.h"
#include "Identifier/Identifier.h"
#include "AthLinks/ElementLink.h"

/** helper class to help reflex/gccxml to discover the right types
 */
template<class T>
struct PrdWrapper
{
  typedef typename T::const_iterator prd_iterator;
};

#define EXPAND_PRD(T, suffix)						\
  T ## Container m_prd_ ## suffix ;					\
  PrdWrapper<T ## Container> m_prd_wrapper_ ## suffix ;			\
  PrdWrapper<T ## Container> ::prd_iterator m_prd_citr_ ## suffix ;	\
  ::EventContainers::IdentifiableCache< Trk::PrepRawDataCollection< T > > m_dummy_dlv_ ## suffix ; \
  T ## Container ::const_iterator m_dummy_iter_ ## suffix  
  
namespace Tracker {
    struct TrackerPrepRawDataDict_Dummy
    {
      DataVector<Identifier>                          m_identifierVector;
      DataVector< Tracker::TrackerClusterCollection >          m_dummyDVSiClusColl;
      DataVector< Tracker::FaserSCT_ClusterCollection >        m_dummyDVSCTClusColl;
      TrackerClusterContainer                         m_dummySiContainer;
      
      EXPAND_PRD(Tracker::TrackerCluster, si_cluster);
      EXPAND_PRD(Tracker::FaserSCT_Cluster, sct_cluster);

      ElementLink<Tracker::FaserSCT_ClusterContainer> m_dummyel_sct_cluster;
    };
}

#undef EXPAND_DLV

#endif



