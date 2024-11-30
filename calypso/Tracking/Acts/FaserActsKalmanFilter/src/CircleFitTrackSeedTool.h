#ifndef FASERACTSKALMANFILTER_CIRCLEFITTRACKSEEDTOOL_H
#define FASERACTSKALMANFILTER_CIRCLEFITTRACKSEEDTOOL_H

#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "FaserActsKalmanFilter/ITrackSeedTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/Surface.h"

// #include "TrackerSimData/TrackerSimDataCollection.h"
// #include "GeneratorObjects/McEventCollection.h"
#include <array>
#include <memory>
#include <string>
#include <vector>
#include <boost/dynamic_bitset.hpp>

typedef boost::dynamic_bitset<> ClusterSet;

class FaserSCT_ID;
namespace TrackerDD { class SCT_DetectorManager; }

class CircleFitTrackSeedTool : public extends<AthAlgTool, ITrackSeedTool> {
public:
  CircleFitTrackSeedTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~CircleFitTrackSeedTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode run(std::vector<int> maskedLayers = {}, bool backward=false) override;
  
  
  const std::shared_ptr<std::vector<Acts::BoundTrackParameters>> initialTrackParameters() const override;
  const std::shared_ptr<const Acts::Surface> initialSurface() const override;
  const std::shared_ptr<std::vector<IndexSourceLink>> sourceLinks() const override;
  const std::shared_ptr<IdentifierLink> idLinks() const override;
  const std::shared_ptr<std::vector<Measurement>> measurements() const override;
  const std::shared_ptr<std::vector<const Tracker::FaserSCT_Cluster*>> clusters() const override;
  const std::shared_ptr<std::vector<std::array<std::vector<const Tracker::FaserSCT_Cluster*>, 3>>> seedClusters() const override;
  const std::shared_ptr<std::vector<const Tracker::FaserSCT_SpacePoint*>> spacePoints() const override;
  double targetZPosition() const override;


  struct Segment {
  public:
    Segment(const Trk::Track* track, const FaserSCT_ID *idHelper, const std::vector<int> &maskedLayers = {});
    int station = -1;
    std::vector<const Tracker::FaserSCT_Cluster*> clusters;
    std::vector<const Tracker::FaserSCT_SpacePoint*> spacePoints;
    std::vector<const Trk::MeasurementBase*> Measurements;//*************************
    ClusterSet clusterSet;
    Acts::Vector3 position;
    std::vector<Acts::Vector3> fakePositions;
    Acts::Vector3 momentum;
  };

  struct Seed {
  public:	  
    Seed(const std::vector<Segment> &segments, bool backward=false);

    ClusterSet clusterSet;
    std::vector<const Tracker::FaserSCT_Cluster*> clusters;
    std::vector<Segment> segs;//get segments of Seed************************
    std::vector<const Tracker::FaserSCT_SpacePoint*> spacePoints;
    std::vector<const Trk::MeasurementBase*> Measurements;//*************************
    std::vector<Acts::Vector3> positions;
    std::vector<Acts::Vector3> fakePositions;

    double c0, c1, cx, cy, r, chi2, momentum, charge, minZ;
    Acts::Vector3 direction;
    size_t size, stations, constraints;
    Acts::BoundTrackParameters get_params(double origin, Acts::BoundSquareMatrix cov, bool backward=false) const;

  private:
    void getChi2();
    double getX(double z);
    double getY(double z);
    void fit();
    void fakeFit(double B=0.55);
    void linearFit(const std::vector<Acts::Vector2> &points);
    double m_dx, m_dy;
    double m_MeV2GeV = 1e-3;
    double m_sigma_x = 0.8;
    double m_sigma_y = 0.016;
  };
  const std::shared_ptr<std::vector<CircleFitTrackSeedTool::Seed>> seeds() const;



private:
 

  void go(const std::array<std::vector<Segment>, 4> &v, std::vector<Segment> &combination,
          std::vector<Seed> &seeds, int offset, int k, bool backward=false);

  static std::map<Identifier, Index> s_indexMap;
  static std::map<Identifier, const Tracker::FaserSCT_SpacePoint*> s_spacePointMap;

  std::shared_ptr<std::vector<Acts::BoundTrackParameters>> m_initialTrackParameters;
  std::shared_ptr<const Acts::Surface> m_initialSurface;
  std::shared_ptr<std::vector<IndexSourceLink>> m_sourceLinks {};
  std::shared_ptr<IdentifierLink> m_idLinks {};
  std::shared_ptr<std::vector<Measurement>> m_measurements {};
  std::shared_ptr<std::vector<const Tracker::FaserSCT_Cluster*>> m_clusters {};
  std::shared_ptr<std::vector<std::array<std::vector<const Tracker::FaserSCT_Cluster*>, 3>>> m_seedClusters {};
  std::shared_ptr<std::vector<const Tracker::FaserSCT_SpacePoint*>> m_spacePoints {};
 
  std::shared_ptr<std::vector<Seed>> m_seeds {};

  double m_targetZPosition {0};

  const FaserSCT_ID* m_idHelper {nullptr};
  const TrackerDD::SCT_DetectorManager* m_detManager {nullptr};

  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool { this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
  SG::ReadHandleKey<TrackCollection> m_trackCollection { this, "TrackCollection", "SegmentFit", "Input track collection name" };
  SG::ReadHandleKey<Tracker::FaserSCT_ClusterContainer> m_clusterContainerKey { this, "ClusterContainer", "SCT_ClusterContainer"};
  SG::ReadHandleKey<FaserSCT_SpacePointContainer> m_spacePointContainerKey { this, "SpacePoints", "SCT_SpacePointContainer"};
  // SG::ReadHandleKey<McEventCollection> m_mcEventCollectionKey { this, "McEventCollection", "TruthEvent"};
  // SG::ReadHandleKey<TrackerSimDataCollection> m_simDataCollectionKey { this, "TrackerSimDataCollection", "SCT_SDO_Map"};

  Gaudi::Property<double> m_std_cluster {this, "std_cluster", 0.04};
  // covariance of the initial parameters
  Gaudi::Property<double> m_covLoc0 {this, "covLoc0", 1};
  Gaudi::Property<double> m_covLoc1 {this, "covLoc1", 1};
  Gaudi::Property<double> m_covPhi {this, "covPhi", 1};
  Gaudi::Property<double> m_covTheta {this, "covTheta", 1};
  Gaudi::Property<double> m_covQOverP {this, "covQOverP", 1};
  Gaudi::Property<double> m_covTime {this, "covTime", 1};
  Gaudi::Property<bool> m_removeIFT{this, "removeIFT", false};
};


inline const std::shared_ptr<std::vector<CircleFitTrackSeedTool::Seed>> 
CircleFitTrackSeedTool::seeds() const {
  return m_seeds;
}

inline const std::shared_ptr<std::vector<Acts::BoundTrackParameters>>
CircleFitTrackSeedTool::initialTrackParameters() const {
  return m_initialTrackParameters;
}

inline const std::shared_ptr<const Acts::Surface>
CircleFitTrackSeedTool::initialSurface() const {
  return m_initialSurface;
}

inline const std::shared_ptr<std::vector<IndexSourceLink>>
CircleFitTrackSeedTool::sourceLinks() const {
  return m_sourceLinks;
}

inline const std::shared_ptr<IdentifierLink>
CircleFitTrackSeedTool::idLinks() const {
  return m_idLinks;
}

inline const std::shared_ptr<std::vector<Measurement>>
CircleFitTrackSeedTool::measurements() const {
  return m_measurements;
}

inline const std::shared_ptr<std::vector<const Tracker::FaserSCT_Cluster*>>
CircleFitTrackSeedTool::clusters() const {
  return m_clusters;
}

inline const std::shared_ptr<std::vector<std::array<std::vector<const Tracker::FaserSCT_Cluster*>, 3>>>
CircleFitTrackSeedTool::seedClusters() const {
  return m_seedClusters;
}

inline const std::shared_ptr<std::vector<const Tracker::FaserSCT_SpacePoint*>>
CircleFitTrackSeedTool::spacePoints() const {
  return m_spacePoints;
}

inline double CircleFitTrackSeedTool::targetZPosition() const {
  return m_targetZPosition;
}

#endif  // FASERACTSKALMANFILTER_CIRCLEFITTRACKSEEDTOOL_H
