// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackerSpacePointFormation_TRACKERSPACEPOINTFINDER_H
#define TrackerSpacePointFormation_TRACKERSPACEPOINTFINDER_H

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "AthContainers/DataVector.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerPrepRawData/TrackerClusterContainer.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "SiElementPropertiesTable.h"
#include "FaserSiSpacePointTool/TrackerSpacePointMakerTool.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointOverlapCollection.h"

#include <string>

class FaserSCT_ID;


/**
@class TrackerSpacePointMakerAlg
 This version of PointFinder uses SiCluster to find space points in
 the pixel and SCT detectors. The algorithm retrieves all clusters from 
 StoreGate using SG::ReadHandle(Key) and uses the begin-end iterators to iterate over the 
 clusters. It then has to see whether the next cluster is a pixel or 
 SCT cluster. Pixel space points are obtained directly from the clusters. 
 To make  SCT space points we also need the IdentifiableContainer for the 
 SCT clusters, since we need random access to find overlapping detectors on the
 stereo layer. Space points are made by combining clusters from pairs of 
 overlapping detectors. The user can choose just to process the phi wafer and 
 its opposite on the stereo layer, or also to consider overlaps with the 
 four nearest neighbours of the opposite wafer.   
 The space points are wrtten to Storegate as SpacePointContainer for Pixels 
 and non-overlapping SCT space points and as SpacePointOverlapCollection for 
 the overlapping ones.
  

 Job Options:
 Process pixels:  default true
 Process SCTs: default true
 Process overlaps: default true
 SCT IdentifiableContainer name: default "SCT_Clusters"
 SCT SpacePointSet name: default "SCT_SpacePoints"


 Fatal Errors:
 neither pixels nor SCTs selected
 SCTs selected and no name for SCT Identifiable Collection
 No name for SpacePointSet
 Failure to retrieve event
 StoreGate failure.

 Possible errors for which processing continues:
 No clusters found for event
 Implementation of SCT_Neighbours is FaserSiElementPropertiesTable,
 is prepared by FaserSiElementPropertiesTableCondAlg
 and is stored in StoreGate using SG::WriteCondHandle(Key). 
*/
namespace Tracker
{
  class TrackerSpacePointFinder:public AthReentrantAlgorithm {

  public:
  
    /**
     * @name AthReentrantAlgorithm methods
     */
    //@{
    TrackerSpacePointFinder(const std::string& name,
			      ISvcLocator* pSvcLocator);
    
    virtual ~TrackerSpacePointFinder() = default;
    
    virtual StatusCode initialize() override;
    
    virtual StatusCode execute (const EventContext& ctx) const override;

    virtual StatusCode finalize() override;

    /** Make this algorithm clonable. */
    virtual bool isClonable() const override { return true; };
    //@}

  private:
    /// This is a temporary object to aid reentrant coding
    struct SPFCache{
      Amg::Vector3D vertex;
      const EventContext& ctx;
      const Tracker::FaserSCT_ClusterContainer* SCTCContainer;
      SPFCache(const EventContext& inctx) : ctx(inctx), SCTCContainer(nullptr) { }
    };

    // methods

    /**
     * @name Disallow constructor without parameters, copy constructor, assignment operator
     */
    //@{
    TrackerSpacePointFinder() = delete;
    TrackerSpacePointFinder(const TrackerSpacePointFinder&) =delete;
    TrackerSpacePointFinder &operator=(const TrackerSpacePointFinder&) = delete;
    //@}

    /**
     * @name Main SCT space point methods
     */
    //@{
    void addSCT_SpacePoints
      (const Tracker::FaserSCT_ClusterCollection* next,
       const TrackerDD::SiElementPropertiesTable* properties,
       const TrackerDD::SiDetectorElementCollection* elements,
       FaserSCT_SpacePointCollection* spacepointCollection, FaserSCT_SpacePointOverlapCollection* spacepointOverlapCollection, SPFCache&) const; 

    void checkForSCT_Points
      (const Tracker::FaserSCT_ClusterCollection* clusters1,
       const IdentifierHash id2,
       const TrackerDD::SiDetectorElementCollection* elements,
       double minDiff, double maxDiff,
       FaserSCT_SpacePointCollection* spacepointCollection, bool overlapColl, FaserSCT_SpacePointOverlapCollection* spacepointOverlapCollection, SPFCache&) const; 

    void checkForSCT_Points
      (const Tracker::FaserSCT_ClusterCollection* clusters1, 
       const IdentifierHash id2,
       const TrackerDD::SiDetectorElementCollection* elements,
       double min1, double max1,
       double min2, double max2, FaserSCT_SpacePointOverlapCollection* spacepointOverlapCollection, SPFCache&) const;
    //@}

    // data members

    /**
     * @name Input data using SG::ReadHandleKey
     */
    //@{
    SG::ReadHandleKey<Tracker::FaserSCT_ClusterContainer>  m_Sct_clcontainerKey{this, "SCT_ClustersName", "SCT clContainer"};
    //@}

    /**
     * @name Input condition data using SG::ReadCondHandleKey
     */
    //@{
    /// For beam spot
//    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
    /// To get SCT neighbours
    SG::ReadCondHandleKey<TrackerDD::SiElementPropertiesTable> m_SCTPropertiesKey{this, "SCTPropertiesKey", "SCT_ElementPropertiesTable", "Key of input SiElementPropertiesTable for SCT"};
    /// For SCT alignment
    SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
    //@}

    /**
     * @name Output data using SG::WriteHandleKey
     */
    //@{
    SG::WriteHandleKey<FaserSCT_SpacePointContainer> m_SpacePointContainer_SCTKey{this, "SpacePointsSCTName", "SCT_SpacePoints", "SpacePoint SCT container"};
    SG::WriteHandleKey<FaserSCT_SpacePointOverlapCollection> m_spacepointoverlapCollectionKey{this, "SpacePointsOverlapName", "OverlapSpacePoints", "Space Point Overlap collection"};
    //@}

    /**
     * @name Caches for HLT using SG::UpdateHandleKey
     */
    //@{
    SG::UpdateHandleKey<FaserSCT_SpacePointCache> m_SpacePointCache_SCTKey;
    //@}

    /**
     * @name ToolHandle
     */
    //@{
    ToolHandle< TrackerSpacePointMakerTool > m_SiSpacePointMakerTool{this, "SpacePointFinderTool", "TrackerSpacePointMakerTool"};
    ToolHandle<GenericMonitoringTool> m_monTool{this, "monTool", "", "Monitoring tool"};
    //@}

    /**
     * @name Id helpers
     */
    //@{
    const FaserSCT_ID* m_idHelper{nullptr};
    //@}

    /**
     * @name Configuration flags
     */
    //@{
    bool m_selectSCTs;
    bool m_overlap; //!< process all overlapping SCT pairs if true.
    bool m_allClusters; //!< process all clusters without limits if true.

    bool m_cachemode{false}; //!< used for online MT counters
    //@}

    /**
     * @name Cut parameters
     * @brief The following are ranges within which clusters must lie to
     * make a spacepoint. Opposite and eta neighbours
     * clusters must lie within range of each other. 
     * Phi clusters must lie in region of each wafer separately.
     */
    //@{
    float m_overlapLimitOpposite;  //!< overlap limit for opposite-neighbours.
    float m_overlapLimitPhi;       //!< overlap limit for phi-neighbours.
    float m_overlapLimitEtaMin;    //!< low overlap limit for eta-neighbours.
    float m_overlapLimitEtaMax;    //!< high overlap limit for eta-neighbours.
    //@}

    /**
     * @name Beam spot override
     */
    //@{
//    bool m_overrideBS;
    float m_xVertex;
    float m_yVertex;
    float m_zVertex;
    //@}

    /**
     * @name Counters
     * Use mutable to be updated in const methods.
     * AthReentrantAlgorithm is const during event processing.
     * Use std::atomic to be multi-thread safe.
     */
    //@{
    mutable std::atomic<int> m_numberOfEvents{0};
    mutable std::atomic<int> m_numberOfSCT{0};
    mutable std::atomic<int> m_sctCacheHits{0};
    mutable std::atomic<int> m_numberOfClusterCollection{0};
    mutable std::atomic<int> m_numberOfCluster{0};
    mutable std::atomic<int> m_numberOfSP{0};
    mutable std::atomic<int> m_numberOfSPCollection{0};
    //@}

  };
}

#endif // TrackerSpacePointFormation_TRACKERSPACEPOINTFINDER_H
