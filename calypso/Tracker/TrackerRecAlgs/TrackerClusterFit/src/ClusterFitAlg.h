/*
   Copyright (C) 2021 CERN for the benefit of the FASER collaboration
   */

/**   @file ClusterFitAlg
 *   Header file for the ClusterFitAlg class (an Algorithm).
 *   @author Dave Casper
 *   @date 28 February 2021
 */

#ifndef FASERCLUSTERFIT_CLUSTERFITALG_H
#define FASERCLUSTERFIT_CLUSTERFITALG_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthHistogramming.h"

////Next contains a typedef so cannot be fwd declared
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerPrepRawData/TrackerClusterContainer.h"
#include "Identifier/IdentifierHash.h"
#include "xAODFaserTrigger/FaserTriggerData.h"
#include "TrkTrack/TrackCollection.h"

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

//STL
#include <map>
#include <string>

class FaserSCT_ID;
class ISvcLocator;
class StatusCode;
class TH1;
class TTree;
namespace Trk
{
    class TrackStateOnSurface;
}

namespace Tracker
{

/**
 *    @class SCT_Clusterization
 *    @brief Form clusters from SCT Raw Data Objects
 *    The class loops over an RDO grouping strips and creating collections of clusters, subsequently recorded in StoreGate
 *    Uses SCT_ConditionsTools to determine which strips to include.
 */
class ClusterFitAlg : public AthReentrantAlgorithm, AthHistogramming
 {
  public:
    /// Constructor with parameters:
    ClusterFitAlg(const std::string& name, ISvcLocator* pSvcLocator);

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
    const ServiceHandle<ITHistSvc>& histSvc() const;

  private:
    /**    @name Disallow default instantiation, copy, assignment */
    //@{
    ClusterFitAlg() = delete;
    ClusterFitAlg(const ClusterFitAlg&) = delete;
    ClusterFitAlg &operator=(const ClusterFitAlg&) = delete;
    //@}
    struct clusterInfo
    {
        const FaserSCT_Cluster* cluster;
        double sinAlpha;
        double cosAlpha;
        double u;
        double z;
        double sigmaSq;
    };
    
    struct layerCombo
    {
        const clusterInfo* stereoPlus;  // no ownership
        const clusterInfo* stereoMinus;
        double sums[15];

        void initialize()
        {
            stereoPlus = nullptr;
            stereoMinus = nullptr;
            for (auto i = 0; i < 15; i++) sums[i] = 0.0;
        }

        void addCluster(const clusterInfo* cluster, double zCenter)
        {
            double sinA = cluster->sinAlpha;
            double cosA = cluster->cosAlpha;
            double    u = cluster->u;
            double    z = cluster->z - zCenter;
            double  sSq = cluster->sigmaSq;
            sums[0] += 1;
            sums[1] += sinA * sinA / sSq;
            sums[2] += sinA * cosA / sSq;
            sums[3] += cosA * cosA / sSq;
            sums[4] += z * sinA * sinA / sSq;
            sums[5] += z * sinA * cosA / sSq;
            sums[6] += z * cosA * cosA / sSq;
            sums[7] += z * z * sinA * sinA / sSq;
            sums[8] += z * z * sinA * cosA / sSq;
            sums[9] += z * z * cosA * cosA / sSq;
            sums[10] += u * sinA / sSq;
            sums[11] += u * cosA / sSq;
            sums[12] += u * z * sinA / sSq;
            sums[13] += u * z * cosA / sSq;
            sums[14] += u * u / sSq;
            if (sinA > 0)
            {
                stereoPlus = cluster;
            }
            else if (sinA < 0)
            {
                stereoMinus = cluster;
            }
         }

         void removeCluster(const clusterInfo* cluster, double zCenter)
         {
            double sinA = cluster->sinAlpha;
            double cosA = cluster->cosAlpha;
            double    u = cluster->u;
            double    z = cluster->z - zCenter;
            double  sSq = cluster->sigmaSq;
            sums[0] -= 1;
            sums[1] -= sinA * sinA / sSq;
            sums[2] -= sinA * cosA / sSq;
            sums[3] -= cosA * cosA / sSq;
            sums[4] -= z * sinA * sinA / sSq;
            sums[5] -= z * sinA * cosA / sSq;
            sums[6] -= z * cosA * cosA / sSq;
            sums[7] -= z * z * sinA * sinA / sSq;
            sums[8] -= z * z * sinA * cosA / sSq;
            sums[9] -= z * z * cosA * cosA / sSq;
            sums[10] -= u * sinA / sSq;
            sums[11] -= u * cosA / sSq;
            sums[12] -= u * z * sinA / sSq;
            sums[13] -= u * z * cosA / sSq;
            sums[14] -= u * u / sSq;
            if (sinA > 0)
            {
                stereoPlus = nullptr;
            }
            else if (sinA < 0)
            {
                stereoMinus = nullptr;
            }
         }         
    };

    StatusCode AddTrack(std::unique_ptr<TrackCollection>& tracks, 
                        const Eigen::Matrix< double, 4, 1 >& fitResult, 
                        const Eigen::Matrix< double, 4, 4>& fitCovariance,  
                        std::vector<const clusterInfo*>& fitClusters, 
                        double chi2, int ndof) const;

    Trk::TrackStateOnSurface* GetState( const Eigen::Matrix< double, 4, 1 >& fitResult, 
                                        const Eigen::Matrix< double, 4, 4 >& fitCovariance,  
                                        const FaserSCT_Cluster* fitCluster,
                                        int station ) const; 

    std::tuple<Eigen::Matrix<double, 4, 1>, 
               Eigen::Matrix<double, 4, 4>, 
               double> 
    ClusterFit(layerCombo* c0, layerCombo* c1, layerCombo* c2) const;

    void
    Residuals(std::vector<const clusterInfo*>& fitClusters) const;

    const FaserSCT_ID* m_idHelper;

    SG::ReadHandleKey<xAOD::FaserTriggerData> m_faserTriggerDataKey { this, "FaserTriggerDataKey", "FaserTriggerData", "ReadHandleKey for xAOD::FaserTriggerData" };
    SG::ReadHandleKey<FaserSCT_ClusterContainer> m_clusterContainerKey { this, "ClustersName", "SCT_ClusterContainer", "FaserSCT cluster container" };
    SG::WriteHandleKey<TrackCollection> m_trackCollection { this, "OutputCollection", "ClusterFit", "Output track collection name" };

    DoubleArrayProperty m_zCenter { this, "ZCenter", { -1452.2925, 47.7075 , 1237.7075, 2427.7075 }, "Global z position at which to reconstruct track parameters"};
    UnsignedIntegerProperty m_triggerMask { this, "TriggerMask", 0x0, "Trigger mask to analyze (0 = pass all)" };

    // Add property to skip reconstruction if cluster multiplicity is too large
    // Set to 0 for no limit
    UnsignedIntegerProperty m_clusterLimit { this, "ClusterLimit", 200, "Limit on cluster multiplicity to run algorithm" };

/// a handle on the Hist/TTree registration service
    ServiceHandle<ITHistSvc> m_histSvc;



    mutable TH1* m_chi2;
    mutable TTree* m_tree;
    mutable int   m_hash;
    mutable float m_residual;
    mutable float m_pull;
    mutable float m_refitChi2;

    mutable std::atomic<int> m_numberOfEvents{0};
    mutable std::atomic<int> m_numberOfTriggeredEvents{0};
    mutable std::atomic<int> m_numberOfClusterCollection{0};
    mutable std::atomic<int> m_numberOfCluster{0};
    mutable std::atomic<int> m_numberOfFits{0};
    mutable std::atomic<int> m_numberOfSkippedEvents{0};
};

// For the THistSvc
inline const ServiceHandle<ITHistSvc>& ClusterFitAlg::histSvc() const 
{
  return m_histSvc;
}

}
#endif // FASERCLUSTERFIT_CLUSTERFITALG_H
