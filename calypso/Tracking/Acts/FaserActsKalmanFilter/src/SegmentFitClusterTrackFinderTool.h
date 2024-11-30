#ifndef FASERACTSKALMANFILTER_SEGMENTFITCLUSTERTRACKFINDERTOOL_H
#define FASERACTSKALMANFILTER_SEGMENTFITCLUSTERTRACKFINDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/ReadHandleKey.h"
#include <string>
#include <vector>

#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "FaserActsKalmanFilter/ITrackFinderTool.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrkTrack/TrackCollection.h"

class FaserSCT_ID;
namespace TrackerDD {
class SCT_DetectorManager;
}


class SegmentFitClusterTrackFinderTool : public extends<AthAlgTool, ITrackFinderTool> {
public:
  SegmentFitClusterTrackFinderTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SegmentFitClusterTrackFinderTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode run() override;

  virtual const std::shared_ptr<std::vector<Acts::BoundTrackParameters>> initialTrackParameters() const override;
  virtual const std::shared_ptr<const Acts::Surface> initialSurface() const override;
  virtual const std::shared_ptr<std::vector<std::vector<IndexSourceLink>>> sourceLinks() const override;
  virtual const std::shared_ptr<std::vector<IdentifierLink>> idLinks() const override;
  virtual const std::shared_ptr<std::vector<std::vector<Measurement>>> measurements() const override;
  virtual const std::shared_ptr<std::vector<std::vector<Tracker::FaserSCT_SpacePoint*>>> spacePoints() const override;
  virtual const std::shared_ptr<std::vector<std::vector<const Tracker::FaserSCT_Cluster*>>> clusters() const override;

private:
  std::shared_ptr<std::vector<Acts::BoundTrackParameters>> m_initialTrackParameters;
  std::shared_ptr<const Acts::Surface> m_initialSurface;
  std::shared_ptr<std::vector<std::vector<IndexSourceLink>>> m_sourceLinks {};
  std::shared_ptr<std::vector<IdentifierLink>> m_idLinks {};
  std::shared_ptr<std::vector<std::vector<Measurement>>> m_measurements {};
  std::shared_ptr<std::vector<std::vector<Tracker::FaserSCT_SpacePoint*>>> m_spacePoints {};
  std::shared_ptr<std::vector<std::vector<const Tracker::FaserSCT_Cluster*>>> m_clusters {};

  const FaserSCT_ID* m_idHelper {nullptr};
  const TrackerDD::SCT_DetectorManager* m_detManager {nullptr};
  static std::pair<double, double> momentum(const std::map<int, Amg::Vector3D>& pos, double B=0.57);

  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {
      this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
  SG::ReadHandleKey<TrackCollection> m_trackCollection {
      this, "TrackCollection", "SegmentFit", "Input track collection name" };

  // covariance of the initial parameters
  Gaudi::Property<double> m_sigmaCluster {this, "sigmaCluster", 0.04};
  Gaudi::Property<double> m_covLoc0 {this, "covLoc0", 1};
  Gaudi::Property<double> m_covLoc1 {this, "covLoc1", 1};
  Gaudi::Property<double> m_covPhi {this, "covPhi", 1};
  Gaudi::Property<double> m_covTheta {this, "covTheta", 1};
  Gaudi::Property<double> m_covQOverP {this, "covQOverP", 1};
  Gaudi::Property<double> m_covTime {this, "covTime", 1};
};


inline const std::shared_ptr<std::vector<Acts::BoundTrackParameters>>
SegmentFitClusterTrackFinderTool::initialTrackParameters() const {
  return m_initialTrackParameters;
}

inline const std::shared_ptr<const Acts::Surface>
SegmentFitClusterTrackFinderTool::initialSurface() const {
  return m_initialSurface;
}

inline const std::shared_ptr<std::vector<std::vector<IndexSourceLink>>>
SegmentFitClusterTrackFinderTool::sourceLinks() const {
  return m_sourceLinks;
}

inline const std::shared_ptr<std::vector<IdentifierLink>>
SegmentFitClusterTrackFinderTool::idLinks() const {
  return m_idLinks;
}

inline const std::shared_ptr<std::vector<std::vector<Measurement>>>
SegmentFitClusterTrackFinderTool::measurements() const {
  return m_measurements;
}

inline const std::shared_ptr<std::vector<std::vector<Tracker::FaserSCT_SpacePoint*>>>
SegmentFitClusterTrackFinderTool::spacePoints() const {
  return m_spacePoints;
}

inline const std::shared_ptr<std::vector<std::vector<const Tracker::FaserSCT_Cluster*>>>
SegmentFitClusterTrackFinderTool::clusters() const {
  return m_clusters;
}

#endif // FASERACTSKALMANFILTER_SEGMENTFITCLUSTERTRACKFINDERTOOL_H
