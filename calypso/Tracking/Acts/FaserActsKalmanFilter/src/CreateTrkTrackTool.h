#ifndef FASERACTSKALMANFILTER_CREATETRKTRACKTOOL_H
#define FASERACTSKALMANFILTER_CREATETRKTRACKTOOL_H

#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/EventData/TrackContainer.hpp"
#include "Acts/EventData/TrackProxy.hpp"
#include "Acts/EventData/VectorTrackContainer.hpp"
#include "Acts/EventData/VectorMultiTrajectory.hpp"

#include <optional>

class FaserSCT_ID;

class CreateTrkTrackTool: public AthAlgTool {
public:
  using TrackContainer =
      Acts::TrackContainer<Acts::VectorTrackContainer,
                           Acts::VectorMultiTrajectory, std::shared_ptr>;
 
  CreateTrkTrackTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~CreateTrkTrackTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  std::unique_ptr<Trk::Track> createTrack(const Acts::GeometryContext &gctx, const TrackContainer::TrackProxy &track, const std::optional<Acts::BoundTrackParameters>& fittedParams = std::nullopt, bool backwardPropagation=false) const;
  std::unique_ptr<Trk::TrackParameters> ConvertActsTrackParameterToATLAS(const Acts::BoundTrackParameters &actsParameter, const Acts::GeometryContext& gctx) const;
private:
  const FaserSCT_ID* m_idHelper {nullptr};
};

#endif //FASERACTSKALMANFILTER_CREATETRKTRACKTOOL_H
