/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */

#ifndef TRACKERSPACEPOINTCNVTOOL_H
#define TRACKERSPACEPOINTCNVTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

#include <utility>

class Identifier;
class IdentifierHash;


namespace Tracker {
  /**Helper tool uses to convert Tracker objects in generic tracking custom convertor TrkEventAthenaPool.

    See "mainpage" for discussion of jobOpts.
    */
  class TrackerSpacePointCnvTool :  public AthAlgTool
  {
    public:

      TrackerSpacePointCnvTool(const std::string&,const std::string&,const IInterface*);

      ~TrackerSpacePointCnvTool() = default;

      virtual StatusCode initialize() override;


      std::pair<const Tracker::FaserSCT_Cluster*, const Tracker::FaserSCT_Cluster*> 
	sctClusterLinks( const Identifier& id1,  const IdentifierHash& idHash1, const Identifier& id2,  const IdentifierHash& idHash2  ) const ;
      void recreateSpacePoint( Tracker::FaserSCT_SpacePoint *sp, const Identifier& id1, const Identifier& id2, const IdentifierHash& idhash1, const IdentifierHash& idhash2 ) const ;


    private:


      SG::ReadHandleKey<Tracker::FaserSCT_ClusterContainer>      m_sctClusContName{this, "SCT_ClusterContainer",     "SCT_Clusters",             "SCT Cluster container name"};   //!< location of container of sct clusters

  };

}
#endif // MOORETOTRACKTOOL_H
