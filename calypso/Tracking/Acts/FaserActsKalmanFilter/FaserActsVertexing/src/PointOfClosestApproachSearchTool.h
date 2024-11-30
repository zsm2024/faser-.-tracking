#ifndef FASERACTSVERTEXING_POINTOFCLOSESTAPPROACHSEARCHTOOL_H
#define FASERACTSVERTEXING_POINTOFCLOSESTAPPROACHSEARCHTOOL_H

#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "Acts/EventData/TrackParameters.hpp"
#include "AthenaBaseComps/AthAlgTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsExtrapolationTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "FaserActsVertexing/IVertexingTool.h"
#include "TrkTrack/Track.h"

namespace FaserTracking {

class PointOfClosestApproachSearchTool : public extends<AthAlgTool, IVertexingTool> {
public:
  PointOfClosestApproachSearchTool(const std::string &type, const std::string &name,
                                   const IInterface *parent);
  virtual ~PointOfClosestApproachSearchTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  std::optional<POCA> getVertex(const std::vector<const Trk::Track *> &tracks) const override;

  std::unique_ptr<const Acts::BoundTrackParameters>
  extrapolateTrack(const Trk::Track *track, double targetPosition) const override;

private:
  struct VertexCandidate {
    VertexCandidate(double z, double chi2, const Eigen::Vector2d &center) : chi2(chi2) {
      position = Eigen::Vector3d(center.x(), center.y(), z);
    }
    double chi2;
    Eigen::Vector3d position;
  };

  struct TrackPosition {
    TrackPosition(const Acts::Vector3 &position, const Acts::BoundSymMatrix &covariance) {
      pos << position.x(), position.y();
      cov << covariance(1, 1), 0, 0, covariance(0, 0);
      inv = cov.inverse();
    }
    Eigen::Vector2d pos;
    Eigen::Matrix2d cov;
    Eigen::Matrix2d inv;
  };

  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool{
      this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
  ToolHandle<IFaserActsExtrapolationTool> m_extrapolationTool{
    this, "ExtrapolationTool", "FaserActsExtrapolationTool"};
  DoubleProperty m_stepSize{this, "StepSize", 50};
  DoubleProperty m_zMin{this, "ZMin", -2000};
  DoubleProperty m_zMax{this, "ZMax", 500};
  StringProperty m_debugLevel{this, "DebugLevel", "INFO"};

  void clear() const;

  std::optional<TrackPosition>
  extrapolateTrackParameters(const Acts::BoundTrackParameters &startParameters, double z) const;

  Eigen::Vector2d getCenter(const std::vector<TrackPosition> &trackPositions) const;

  Acts::BoundTrackParameters
  getParametersFromTrack(const Trk::TrackParameters *trackParameters) const;

  double chi2(const std::vector<TrackPosition> &trackPositions,
              const Eigen::Vector2d &trackCenter) const;

  double chi2Y(const std::vector<TrackPosition> &trackPositions,
               const Eigen::Vector2d &trackCenter) const;

  std::optional<VertexCandidate>
  getVertexCandidate(std::vector<Acts::BoundTrackParameters> &trackParameters, double z) const;

  mutable EventContext m_ctx;
  mutable Acts::GeometryContext m_gctx;

  mutable std::vector<VertexCandidate> m_allVertexCandidates;
  mutable std::vector<VertexCandidate> m_goodVertexCandidates;
};

} // namespace FaserTracking

#endif /* FASERACTSVERTEXING_POINTOFCLOSESTAPPROACHSEARCHTOOL_H */
