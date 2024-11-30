#ifndef FASERACTSKALMANFILTER_SEGMENTFITTRACKFINDERTOOL_H
#define FASERACTSKALMANFILTER_SEGMENTFITTRACKFINDERTOOL_H

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
#include "TrkTrack/TrackCollection.h"

class FaserSCT_ID;
namespace TrackerDD {
class SCT_DetectorManager;
}


class SegmentFitTrackFinderTool : public extends<AthAlgTool, ITrackFinderTool> {
public:
  SegmentFitTrackFinderTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SegmentFitTrackFinderTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode run() override;

  virtual const std::shared_ptr<const Acts::BoundTrackParameters> initialTrackParameters() const override;
  virtual const std::shared_ptr<const Acts::Surface> initialSurface() const override;
  virtual const std::shared_ptr<std::vector<IndexSourceLink>> sourceLinks() const override;
  virtual const std::shared_ptr<IdentifierLink> idLinks() const override;
  virtual const std::shared_ptr<std::vector<Measurement>> measurements() const override;
  virtual const std::shared_ptr<std::vector<Tracker::FaserSCT_SpacePoint*>> spacePoints() const override;

private:
  std::shared_ptr<const Acts::BoundTrackParameters> m_initialTrackParameters;
  std::shared_ptr<const Acts::Surface> m_initialSurface;
  std::shared_ptr<std::vector<IndexSourceLink>> m_sourceLinks {};
  std::shared_ptr<IdentifierLink> m_idLinks {};
  std::shared_ptr<std::vector<Measurement>> m_measurements {};
  std::shared_ptr<std::vector<Tracker::FaserSCT_SpacePoint*>> m_spacePoints {};

  const FaserSCT_ID* m_idHelper {nullptr};
  const TrackerDD::SCT_DetectorManager* m_detManager {nullptr};
  static double momentum(const std::map<int, Amg::Vector3D>& pos, double B=0.57) ;

  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {
      this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
  SG::ReadHandleKey<TrackCollection> m_trackCollection {
      this, "TrackCollection", "SegmentFit", "Input track collection name" };
  SG::ReadHandleKey<FaserSCT_SpacePointContainer> m_spacePointContainerKey {
      this, "SpacePoints", "SCT_SpacePointContainer", "Space point container"};

  // covariance of the initial parameters
  Gaudi::Property<double> m_covLoc0 {this, "covLoc0", 1};
  Gaudi::Property<double> m_covLoc1 {this, "covLoc1", 1};
  Gaudi::Property<double> m_covPhi {this, "covPhi", 1};
  Gaudi::Property<double> m_covTheta {this, "covTheta", 1};
  Gaudi::Property<double> m_covQOverP {this, "covQOverP", 1};
  Gaudi::Property<double> m_covTime {this, "covTime", 1};
};


inline const std::shared_ptr<const Acts::BoundTrackParameters>
    SegmentFitTrackFinderTool::initialTrackParameters() const {
  return m_initialTrackParameters;
}

inline const std::shared_ptr<const Acts::Surface>
    SegmentFitTrackFinderTool::initialSurface() const {
return m_initialSurface;
}

inline const std::shared_ptr<std::vector<IndexSourceLink>>
   SegmentFitTrackFinderTool::sourceLinks() const {
  return m_sourceLinks;
}

inline const std::shared_ptr<IdentifierLink>
SegmentFitTrackFinderTool::idLinks() const {
  return m_idLinks;
}

inline const std::shared_ptr<std::vector<Measurement>>
    SegmentFitTrackFinderTool::measurements() const {
  return m_measurements;
}

inline const std::shared_ptr<std::vector<Tracker::FaserSCT_SpacePoint*>>
    SegmentFitTrackFinderTool::spacePoints() const {
  return m_spacePoints;
}
#endif // FASERACTSKALMANFILTER_SEGMENTFITTRACKFINDERTOOL_H
