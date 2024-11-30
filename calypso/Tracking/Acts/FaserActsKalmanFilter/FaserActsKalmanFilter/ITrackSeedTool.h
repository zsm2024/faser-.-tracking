#ifndef FASERACTSKALMANFILTER_ITRACKSEEDTOOL_H
#define FASERACTSKALMANFILTER_ITRACKSEEDTOOL_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "FaserActsKalmanFilter/IndexSourceLink.h"
#include "FaserActsKalmanFilter/IdentifierLink.h"
#include "FaserActsKalmanFilter/Measurement.h"
#include "Acts/EventData/TrackParameters.hpp"
// #include "TrackerPrepRawData/FaserSCT_Cluster.h"
// #include "TrackerSpacePoint/FaserSCT_SpacePoint.h"

namespace Tracker
{
  class FaserSCT_Cluster;
  class FaserSCT_SpacePoint;
}

class ITrackSeedTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ITrackSeedTool, 1, 0);

  virtual StatusCode run(std::vector<int> maskedLayers = {}, bool backward = false) = 0;
  virtual const std::shared_ptr<std::vector<Acts::BoundTrackParameters>> initialTrackParameters() const = 0;
  virtual const std::shared_ptr<const Acts::Surface> initialSurface() const = 0;
  virtual const std::shared_ptr<std::vector<IndexSourceLink>> sourceLinks() const = 0;
  virtual const std::shared_ptr<IdentifierLink> idLinks() const = 0;
  virtual const std::shared_ptr<std::vector<Measurement>> measurements() const = 0;
  virtual const std::shared_ptr<std::vector<const Tracker::FaserSCT_Cluster*>> clusters() const = 0;
  virtual const std::shared_ptr<std::vector<std::array<std::vector<const Tracker::FaserSCT_Cluster*>, 3>>> seedClusters() const = 0;
  virtual const std::shared_ptr<std::vector<const Tracker::FaserSCT_SpacePoint*>> spacePoints() const = 0;
  virtual double targetZPosition() const = 0;
};

#endif  // FASERACTSKALMANFILTER_ITRACKSEEDTOOL_H
