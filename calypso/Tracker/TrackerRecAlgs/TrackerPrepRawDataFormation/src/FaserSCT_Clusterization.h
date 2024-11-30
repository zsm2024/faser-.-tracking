/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

/**   @file SCT_Clusterization
 *   Header file for the SCT_Clusterization class (an Algorithm).
 *   @author Paul Bell, Tommaso Lari, Shaun Roe, Carl Gwilliam
 *   @date 08 July 2008
 */

#ifndef FASERSCTCLUSTERIZATION_FASERSCT_CLUSTERIZATION_H
#define FASERSCTCLUSTERIZATION_FASERSCT_CLUSTERIZATION_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

////Next contains a typedef so cannot be fwd declared
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerPrepRawData/TrackerClusterContainer.h"
#include "TrackerRawData/FaserSCT_RDO_Container.h"
// #include "FaserSCT_ConditionsData/SCT_FlaggedCondData.h"

#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "FaserSiClusterizationTool/IFaserSCT_ClusteringTool.h"

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

//STL
#include <map>
#include <string>

class FaserSCT_ID;
class ISvcLocator;
class StatusCode;

namespace Tracker
{

/**
 *    @class SCT_Clusterization
 *    @brief Form clusters from SCT Raw Data Objects
 *    The class loops over an RDO grouping strips and creating collections of clusters, subsequently recorded in StoreGate
 *    Uses SCT_ConditionsTools to determine which strips to include.
 */
class FaserSCT_Clusterization : public AthReentrantAlgorithm {
  public:
    /// Constructor with parameters:
    FaserSCT_Clusterization(const std::string& name, ISvcLocator* pSvcLocator);

    /**    @name Usual algorithm methods */
    //@{
    ///Retrieve the tools used and initialize variables
    virtual StatusCode initialize() override;
    ///Form clusters and record them in StoreGate (detector store)
    virtual StatusCode execute(const EventContext& ctx) const override;
    ///Clean up and release the collection containers
    virtual StatusCode finalize() override;
    //Make this algorithm clonable.
    virtual bool isClonable() const override { return true; };
    //@}

  private:
    /**    @name Disallow default instantiation, copy, assignment */
    //@{
    FaserSCT_Clusterization() = delete;
    FaserSCT_Clusterization(const FaserSCT_Clusterization&) = delete;
    FaserSCT_Clusterization &operator=(const FaserSCT_Clusterization&) = delete;
    //@}

    ToolHandle<IFaserSCT_ClusteringTool> m_clusteringTool{this, "SCT_ClusteringTool", "FaserSCT_ClusteringTool"};
    ToolHandle<IInDetConditionsTool> m_pSummaryTool{this, "conditionsTool", "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool", "Tool to retrieve SCT conditions summary"};
    const FaserSCT_ID* m_idHelper;

    SG::ReadHandleKey<FaserSCT_RDO_Container> m_rdoContainerKey{this, "DataObjectName", "FaserSCT_RDOs", "FaserSCT RDOs"};
    SG::WriteHandleKey<FaserSCT_ClusterContainer> m_clusterContainerKey{this, "ClustersName", "SCT_ClusterContainer", "FaserSCT cluster container"};
    SG::WriteHandleKey<TrackerClusterContainer> m_clusterContainerLinkKey{this, "ClustersLinkName_", "SCT_ClusterContainer", "Tracker cluster container link name (don't set this)"};
    // SG::WriteHandleKey<FaserSCT_FlaggedCondData> m_flaggedCondDataKey{this, "SCT_FlaggedCondData", "SCT_FlaggedCondData", "SCT flagged conditions data"};
    SG::UpdateHandleKey<FaserSCT_ClusterContainerCache> m_clusterContainerCacheKey;

    UnsignedIntegerProperty m_maxFiredStrips{this, "maxFiredStrips", 384, "Threshold of number of fired strips per wafer"}; // 0 disables the per-wafer cut
    UnsignedIntegerProperty m_maxTotalOccupancyPercent{this, "maxTotalOccupancyInPercent", 100, "Threshold of percentage of fired strips for the whole SCT"}; // 100 disables the whole SCT cut
    BooleanProperty m_checkBadModules{this, "checkBadModules", false};

    mutable std::atomic<int> m_numberOfEvents{0};
    mutable std::atomic<int> m_numberOfRDOCollection{0};
    mutable std::atomic<int> m_numberOfRDO{0};
    mutable std::atomic<int> m_numberOfClusterCollection{0};
    mutable std::atomic<int> m_numberOfCluster{0};
};
}
#endif // INDETRIOMAKER_SCT_CLUSTERIZATION_H
