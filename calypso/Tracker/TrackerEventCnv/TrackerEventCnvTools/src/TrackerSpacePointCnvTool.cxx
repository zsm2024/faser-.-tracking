/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */

#include "TrackerEventCnvTools/TrackerSpacePointCnvTool.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"

#include <vector>
#include <cassert>
#include <iostream>

Tracker::TrackerSpacePointCnvTool::TrackerSpacePointCnvTool(const std::string& t,
    const std::string& n,
    const IInterface*  p )
  :
    AthAlgTool(t,n,p)
{
}

StatusCode Tracker::TrackerSpacePointCnvTool::initialize() {

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  ATH_CHECK( m_sctClusContName.initialize() );

  return sc;

}

void Tracker::TrackerSpacePointCnvTool::recreateSpacePoint( Tracker::FaserSCT_SpacePoint *sp, const Identifier& id1, const Identifier& id2, const IdentifierHash& idhash1, const IdentifierHash& idhash2 ) const {
  auto clusters= sctClusterLinks( id1,idhash1, id2,idhash2);
  sp->setClusList(clusters);
  return;
}

std::pair<const Tracker::FaserSCT_Cluster*, const Tracker::FaserSCT_Cluster*> 
Tracker::TrackerSpacePointCnvTool::sctClusterLinks( const Identifier& id1,  const IdentifierHash & idHash1, const Identifier& id2,  const IdentifierHash& idHash2  ) const {
  const Tracker::FaserSCT_Cluster* clus1=nullptr;
  const Tracker::FaserSCT_Cluster* clus2=nullptr;
  // need to retrieve pointers to collections
  // retrieve Pixel cluster container

  // obviously this can be optimised! EJWM
  SG::ReadHandle<Tracker::FaserSCT_ClusterContainer> h_sctClusCont(m_sctClusContName);
  if (!h_sctClusCont.isValid()) {
    ATH_MSG_ERROR("FaserSCT Cluster container not found at "<<m_sctClusContName);
  return std::make_pair(clus1,clus2);
  } else {
    ATH_MSG_DEBUG("FaserSCT Cluster Container found" );
  }
  const Tracker::FaserSCT_ClusterCollection *ptr1 = h_sctClusCont->indexFindPtr(idHash1);
  const Tracker::FaserSCT_ClusterCollection *ptr2 = h_sctClusCont->indexFindPtr(idHash2);
  // if we find PRD, then recreate link
  if (ptr1!=nullptr) {
    //loop though collection to find matching PRD.
    Tracker::FaserSCT_ClusterCollection::const_iterator collIt    = ptr1->begin();
    Tracker::FaserSCT_ClusterCollection::const_iterator collItEnd = ptr1->end();
    // there MUST be a faster way to do this!!
    for ( ; collIt!=collItEnd; collIt++) {
      if ( (*collIt)->identify()==id1 ) 
	clus1= &(**collIt);
    }
  }
  // if we find PRD, then recreate link
  if (ptr2!=nullptr) {
    //loop though collection to find matching PRD.
    Tracker::FaserSCT_ClusterCollection::const_iterator collIt    = ptr2->begin();
    Tracker::FaserSCT_ClusterCollection::const_iterator collItEnd = ptr2->end();
    // there MUST be a faster way to do this!!
    for ( ; collIt!=collItEnd; collIt++) {
      if ( (*collIt)->identify()==id2 ) 
	clus2= &(**collIt);
    }
  }
  if(clus1==nullptr||clus2==nullptr)
    ATH_MSG_DEBUG("No matching PRD found" );
  return std::make_pair(clus1,clus2);
}


