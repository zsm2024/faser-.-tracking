#ifndef FASERACTSKALMANFILTER_ITRACKFINDERTOOL_H
#define FASERACTSKALMANFILTER_ITRACKFINDERTOOL_H

// #include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "FaserActsKalmanFilter/IndexSourceLink.h"
#include "FaserActsKalmanFilter/IdentifierLink.h"
#include "FaserActsKalmanFilter/Measurement.h"
#include "Acts/EventData/TrackParameters.hpp"

namespace Tracker
{
  class FaserSCT_Cluster;
  class FaserSCT_SpacePoint;
}

class ITrackFinderTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ITrackFinderTool, 1, 0);

  // TODO use Acts::BoundTrackParameters instead?
  virtual StatusCode run() = 0;
  virtual const std::shared_ptr<std::vector<Acts::BoundTrackParameters>> initialTrackParameters() const = 0;
  virtual const std::shared_ptr<const Acts::Surface> initialSurface() const = 0;
  virtual const std::shared_ptr<std::vector<std::vector<IndexSourceLink>>> sourceLinks() const = 0;
  virtual const std::shared_ptr<std::vector<IdentifierLink>> idLinks() const = 0;
  virtual const std::shared_ptr<std::vector<std::vector<Measurement>>> measurements() const = 0;
  virtual const std::shared_ptr<std::vector<std::vector<Tracker::FaserSCT_SpacePoint*>>> spacePoints() const = 0;
  virtual const std::shared_ptr<std::vector<std::vector<const Tracker::FaserSCT_Cluster*>>> clusters() const = 0;
};

#endif  // FASERACTSKALMANFILTER_ITRACKFINDERTOOL_H
