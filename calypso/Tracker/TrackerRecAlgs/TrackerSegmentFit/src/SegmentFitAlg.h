/*
   Copyright (C) 2021 CERN for the benefit of the FASER collaboration
   */

/**   @file SegmentFitAlg
 *   Header file for the SegmentFitAlg class (an Algorithm).
 *   @author Dave Casper
 *   @date 28 February 2021
 */

#ifndef FASERSEGMENTFIT_SEGMENTFITALG_H
#define FASERSEGMENTFIT_SEGMENTFITALG_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

////Next contains a typedef so cannot be fwd declared
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerPrepRawData/TrackerClusterContainer.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "Identifier/IdentifierHash.h"
#include "TrkTrack/TrackCollection.h"

//Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

//STL
#include <map>
#include <string>

//Boost
#include <boost/dynamic_bitset.hpp>

class ISvcLocator;
class StatusCode;
namespace Trk
{
    class TrackStateOnSurface;
}

namespace TrackerDD
{
    class SCT_DetectorManager;
}

namespace Tracker
{
    typedef boost::dynamic_bitset<> ClusterSet;

/**
 *    @class SCT_Clusterization
 *    @brief Form clusters from SCT Raw Data Objects
 *    The class loops over an RDO grouping strips and creating collections of clusters, subsequently recorded in StoreGate
 *    Uses SCT_ConditionsTools to determine which strips to include.
 */
class SegmentFitAlg : public AthReentrantAlgorithm
 {
  public:
    /// Constructor with parameters:
    SegmentFitAlg(const std::string& name, ISvcLocator* pSvcLocator);

    /**    @name Usual algorithm methods */
    //@{
    ///Retrieve the tools used and initialize variables
    virtual StatusCode initialize() override;
    ///Form segments and record them in StoreGate (detector store)
    virtual StatusCode execute(const EventContext& ctx) const override;
    ///Clean up and release the collection containers
    virtual StatusCode finalize() override;
    //Make this algorithm clonable.
    virtual bool isClonable() const override { return true; };
    //@}


  private:
    /**    @name Disallow default instantiation, copy, assignment */
    //@{
    SegmentFitAlg() = delete;
    SegmentFitAlg(const SegmentFitAlg&) = delete;
    SegmentFitAlg &operator=(const SegmentFitAlg&) = delete;
    //@}

    static constexpr size_t compatibilityMaxSize = 128; 

    struct clusterInfo
    {
        public:
        clusterInfo(uint theIndex, const FaserSCT_Cluster& theCluster, const FaserSCT_ID* idHelper)
        : index(theIndex)
        , cluster(theCluster)
        , compatible(nClusters)
        {  
            const TrackerDD::SiDetectorElement* elem = cluster.detectorElement();
            if (elem == nullptr)
            {
                view = 0;
                return;
            }
            Identifier id = elem->identify();
            double alpha = std::abs(asin(elem->sinStereo()));
            layer = idHelper->layer(id);
            eta = idHelper->eta_module(id); // -1 or +1
            etaIndex = (eta + 1)/2; // 0 or 1
            phi = idHelper->phi_module(id); // 0 to 3
            moduleIndex = ((etaIndex + phi)%2 == 1 ? phi : phi + 4); // 0 to 7
            switch (moduleIndex)
            {
                case 0:
                case 2:
                {
                    // info->sinAlpha = sin(alpha);
                    // info->cosAlpha = -cos(alpha);
                    sinAlpha = -sin(alpha);
                    cosAlpha = cos(alpha);
                    break;
                }
                case 1:
                case 3:
                {
                    sinAlpha = -sin(alpha);
                    cosAlpha = cos(alpha);
                    break;
                }
                case 4:
                case 6:
                {
                    // info->sinAlpha = -sin(alpha);
                    // info->cosAlpha = -cos(alpha);
                    sinAlpha = sin(alpha);
                    cosAlpha = cos(alpha);
                    break;
                }
                case 5:
                case 7:
                {
                    sinAlpha = sin(alpha);
                    cosAlpha = cos(alpha);
                    break;
                }
            }
            side = idHelper->side(id);
            if (side > 0) sinAlpha = - sinAlpha; // always reverse angle for back side wafer
            u        = cluster.globalPosition().y() * cosAlpha + cluster.globalPosition().x() * sinAlpha;
            z        = cluster.globalPosition().z();
            sigmaSq  = cluster.localCovariance()(0,0);
            if (sinAlpha * cosAlpha < 0)
            {
                view = -1;
                compatible.set(index, true);
            }
            else if (sinAlpha * cosAlpha > 0)
            {
                view = 1;
                compatible.set(index, true);
            }
            else
            {
                view = 0; // will be silently ignored
                compatible.set(index, false);
            }
            yCenter = cluster.globalPosition().y();
            edge = false;
            const std::vector<Identifier>& rdoList = cluster.rdoList();
            for (auto id : rdoList)
            {
                int stripNumber = idHelper->strip(id);
                if (stripNumber < nEdgeStrips || stripNumber >= idHelper->strip_max(id) - nEdgeStrips + 1) 
                {
                    edge = true;
                    break;
                }
            }
            double pitch = elem->design().phiPitch();
            auto ends = elem->endsOfStrip(cluster.localPosition());
            if (abs(ends.first.x()) < abs(ends.second.x()))
            {
                yEnd = ends.first.y()/pitch;
            }
            else
            {
                yEnd = ends.second.y()/pitch;
            }
        }
        // Data members
        static int nEdgeStrips;
        static size_t nClusters; 
        uint index;
        const FaserSCT_Cluster& cluster;
        ClusterSet compatible;
        double sinAlpha;
        double cosAlpha;
        double u;
        double z;
        double sigmaSq;
        double yCenter;
        double yEnd;
        int layer;
        int moduleIndex;
        int side;
        int phi;
        int eta;
        int etaIndex;
        int view;
        bool edge;
    };
    typedef std::vector<std::shared_ptr<clusterInfo> > ClusterList;

    struct fitInfo
    {
        public:
        fitInfo(const ClusterList& theClusters, 
                std::vector<size_t> theCandidates)
        : clusters { theClusters }
        , clusterMask { clusterInfo::nClusters }
        , compatibleMask { clusterInfo::nClusters }
        , fitChi2 { 0 }
        , hasCovariance { false }
        { 
            for (auto n = 0; n < 15; n++) sums[n] = 0.0;
            compatibleMask.set();
            for (auto index : theCandidates)
            {
                addCluster(index);
            }
        }
        // fitInfo(const fitInfo& f)
        // : clusters { f.clusters }
        // , candidates { f.candidates }
        // , clusterMask { f.clusterMask }
        // , compatibleMask { f.compatibleMask }
        // , fitParams { f.fitParams }
        // , fitCovariance { f.fitCovariance }
        // , fitChi2 { f.fitChi2 }
        // , hasCovariance { f.hasCovariance }
        // {
        //     for (size_t i = 0; i < 15; i++)
        //     {
        //         sums[i] = f.sums[i];
        //     }
        // }
        void addCluster(size_t n)
        {
            candidates.push_back(n);
            clusterMask.set(n);
            compatibleMask &= clusters[n]->compatible;
            compatibleMask.set(n, false);
            double sinA = clusters[n]->sinAlpha;
            double cosA = clusters[n]->cosAlpha;
            double    u = clusters[n]->u;
            double    z = clusters[n]->z - zCenter;
            double  sSq = clusters[n]->sigmaSq;
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
            if (fitChi2 != 0 || hasCovariance)
            {
                fitParams.setZero();
                fitCovariance.setZero();
                fitChi2 = 0;
                hasCovariance = false;
            }            
        }
        HepGeom::Point3D<double> extrapolateFit(double z) const
        {
            return HepGeom::Point3D<double> { fitParams(0) + fitParams(2) * (z - zCenter), fitParams(1) + fitParams(3) * (z - zCenter), z };
        }
        const ClusterList& clusters;
        std::vector<size_t> candidates;
        ClusterSet clusterMask;
        ClusterSet compatibleMask;
        double sums[15];
        static double zCenter;
        Eigen::Matrix<double, 4, 1> fitParams;
        Eigen::Matrix<double, 4, 4> fitCovariance;
        double                      fitChi2;
        bool hasCovariance;
    };

    typedef std::vector<std::shared_ptr<fitInfo> > FitList;

    typedef std::unordered_map<ClusterSet, std::shared_ptr<fitInfo> > FitMap;
    typedef std::unordered_set<ClusterSet> MaskSet;
    
    typedef SG::ReadHandle<FaserSCT_ClusterContainer> ClusterContainerHandle;
    StatusCode reconstructStation(ClusterContainerHandle clusters, int index, std::unique_ptr<TrackCollection>& tracks) const;
    size_t stationOccupancy(ClusterContainerHandle clusters, int station) const;
    double findCenter(int station) const;
    ClusterList tabulateClusters(ClusterContainerHandle clusters, int station) const;
    void checkCompatibility(ClusterList& clusters) const;
    FitList enumerateSeeds(const ClusterList& clusters) const;
    void findGoodSeeds(FitList& allSeeds, FitMap& goodSeeds, MaskSet& badSeeds) const;
    void fitClusters(fitInfo& fit) const;
    bool checkFit(const fitInfo& fit) const;
    FitMap enumerateFits(const FitMap& seeds, const MaskSet& veto) const;
    FitMap checkFitQuality(const FitMap& fits) const;
    FitMap selectFits(const FitMap& fits) const;
    bool vetoFit(const MaskSet& badSeeds, const ClusterSet& hypothesis) const;
    int getStation(const Tracker::FaserSCT_ClusterCollection* collection) const;
    
    // struct layerCombo
    // {
    //     const clusterInfo* stereoPlus;  // no ownership
    //     const clusterInfo* stereoMinus;
    //     double sums[15];

    //     void initialize()
    //     {
    //         stereoPlus = nullptr;
    //         stereoMinus = nullptr;
    //         for (auto i = 0; i < 15; i++) sums[i] = 0.0;
    //     }

    //     void addCluster(const clusterInfo* cluster, double zCenter)
    //     {
    //         double sinA = cluster->sinAlpha;
    //         double cosA = cluster->cosAlpha;
    //         double    u = cluster->u;
    //         double    z = cluster->z - zCenter;
    //         double  sSq = cluster->sigmaSq;
    //         sums[0] += 1;
    //         sums[1] += sinA * sinA / sSq;
    //         sums[2] += sinA * cosA / sSq;
    //         sums[3] += cosA * cosA / sSq;
    //         sums[4] += z * sinA * sinA / sSq;
    //         sums[5] += z * sinA * cosA / sSq;
    //         sums[6] += z * cosA * cosA / sSq;
    //         sums[7] += z * z * sinA * sinA / sSq;
    //         sums[8] += z * z * sinA * cosA / sSq;
    //         sums[9] += z * z * cosA * cosA / sSq;
    //         sums[10] += u * sinA / sSq;
    //         sums[11] += u * cosA / sSq;
    //         sums[12] += u * z * sinA / sSq;
    //         sums[13] += u * z * cosA / sSq;
    //         sums[14] += u * u / sSq;
    //         if (sinA > 0)
    //         {
    //             stereoPlus = cluster;
    //         }
    //         else if (sinA < 0)
    //         {
    //             stereoMinus = cluster;
    //         }
    //      }

    //      void removeCluster(const clusterInfo* cluster, double zCenter)
    //      {
    //         double sinA = cluster->sinAlpha;
    //         double cosA = cluster->cosAlpha;
    //         double    u = cluster->u;
    //         double    z = cluster->z - zCenter;
    //         double  sSq = cluster->sigmaSq;
    //         sums[0] -= 1;
    //         sums[1] -= sinA * sinA / sSq;
    //         sums[2] -= sinA * cosA / sSq;
    //         sums[3] -= cosA * cosA / sSq;
    //         sums[4] -= z * sinA * sinA / sSq;
    //         sums[5] -= z * sinA * cosA / sSq;
    //         sums[6] -= z * cosA * cosA / sSq;
    //         sums[7] -= z * z * sinA * sinA / sSq;
    //         sums[8] -= z * z * sinA * cosA / sSq;
    //         sums[9] -= z * z * cosA * cosA / sSq;
    //         sums[10] -= u * sinA / sSq;
    //         sums[11] -= u * cosA / sSq;
    //         sums[12] -= u * z * sinA / sSq;
    //         sums[13] -= u * z * cosA / sSq;
    //         sums[14] -= u * u / sSq;
    //         if (sinA > 0)
    //         {
    //             stereoPlus = nullptr;
    //         }
    //         else if (sinA < 0)
    //         {
    //             stereoMinus = nullptr;
    //         }
    //      }         
    // };

    StatusCode AddTrack(std::unique_ptr<TrackCollection>& tracks, 
                        const std::shared_ptr<fitInfo>& theFit) const;

    Trk::TrackStateOnSurface* GetState( const Eigen::Matrix< double, 4, 1 >& fitResult, 
                                        const Eigen::Matrix< double, 4, 4 >& fitCovariance,  
                                        const FaserSCT_Cluster* fitCluster ) const; 

    // void
    // Residuals(std::vector<const clusterInfo*>& fitClusters) const;

    const FaserSCT_ID* m_idHelper;
    const TrackerDD::SCT_DetectorManager* m_detMgr {nullptr};

    SG::ReadHandleKey<FaserSCT_ClusterContainer> m_clusterContainerKey { this, "ClustersName", "SCT_ClusterContainer", "FaserSCT cluster container" };
    SG::WriteHandleKey<TrackCollection> m_trackCollection { this, "OutputCollection", "SegmentFit", "Output track collection name" };

    UnsignedIntegerProperty m_minClustersPerStation { this, "MinClusters", 4, "Minimum number of clusters allowed in a station" };
    UnsignedIntegerProperty m_maxClustersPerStation { this, "MaxClusters", 36, "Maximum number of clusters allowed in a station" };
    BooleanProperty m_allowEtaOverlap { this, "EtaOverlap", true, "Allow overlapping clusters in the same face with neighboring eta" };
    BooleanProperty m_allowPhiOverlap { this, "PhiOverlap", true, "Allow overlapping edge clusters in the same face with neighboring phi" };
    IntegerProperty m_edgeStrips { this, "EdgeStrips", 32, "Number of strips from either edge of sensor considered for overlap" };
    DoubleProperty m_waferTolerance { this, "WaferTolerance", 3 * Gaudi::Units::mm, "Wafer boundary tolerance for extrapolated fit." };
    DoubleProperty m_yResidualCut { this, "ResidualCut", 3 * Gaudi::Units::mm, "Cluster y tolerance for compatibility with extrapolated fit." };
    DoubleProperty m_tanThetaCut { this, "TanThetaCut", 0.0, "Maximum accepted tangent of track angle from beam axis; 0 means no cut."};
    DoubleProperty m_tanThetaXZCut { this, "TanThetaXZCut", 0.0, "Maximum accepted tangent of track angle from beam axis in x direction; 0 means no cut."};
    DoubleProperty m_reducedChi2Cut { this, "ReducedChi2Cut", 10.0, "Maximum accepted chi^2 per degree of freedom for final fits; 0 means no cut." };
    DoubleProperty m_sharedHitFraction { this, "SharedHitFraction", -1., "Fraction of hits which are allowed to be shared between two fits." };
    UnsignedIntegerProperty m_minClustersPerFit { this, "MinClustersPerFit", 4, "Minimum number of clusters a fit has to have." };

    mutable std::atomic<int> m_numberOfEvents{0};
    mutable std::atomic<int> m_numberExcessOccupancy{0};
    mutable std::atomic<int> m_numberLowOccupancy{0};
    mutable std::atomic<int> m_numberOfGoodStations{0};
    mutable std::atomic<int> m_numberOfClusterCollection{0};
    mutable std::atomic<int> m_numberOfClusters{0};
    mutable std::atomic<int> m_numberOfRule1Pairs{0};
    mutable std::atomic<int> m_numberOfRule2Pairs{0};
    mutable std::atomic<int> m_numberOfRule3Pairs{0};
    mutable std::atomic<int> m_numberOfRule4Pairs{0};
    mutable std::atomic<int> m_numberOfSeeds{0};
    mutable std::atomic<int> m_numberOfGoodSeeds{0};
    mutable std::atomic<int> m_numberOfFits{0};
    mutable std::atomic<int> m_numberOfGoodFits{0};
    mutable std::atomic<int> m_numberOfSelectedFits{0};
};

inline int SegmentFitAlg::getStation(const Tracker::FaserSCT_ClusterCollection* collection) const
{
    return m_idHelper->station(m_idHelper->wafer_id(collection->identifyHash()));
}

}
#endif // FASERSEGMENTFIT_SEGMENTFITALG_H
