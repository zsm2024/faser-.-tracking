#include "PointOfClosestApproachSearchTool.h"
#include "TrkParameters/TrackParameters.h"
#include <climits>

namespace FaserTracking {

PointOfClosestApproachSearchTool::PointOfClosestApproachSearchTool(const std::string &type,
                                                                   const std::string &name,
                                                                   const IInterface *parent)
    : base_class(type, name, parent) {}

StatusCode PointOfClosestApproachSearchTool::initialize() {
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  ATH_CHECK(m_extrapolationTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode PointOfClosestApproachSearchTool::finalize() { return StatusCode::SUCCESS; }

std::optional<POCA>
PointOfClosestApproachSearchTool::getVertex(const std::vector<const Trk::Track *> &tracks) const {

  if (tracks.size() < 2) {
    ATH_MSG_DEBUG("Found " << tracks.size() << " tracks, but require at least 2 tracks.");
    return {};
  } else if (tracks.size() > 3) {
    ATH_MSG_DEBUG("Found " << tracks.size() << " tracks, but require at most 3 tracks.");
    return {};
  }

  clear();

  m_ctx = Gaudi::Hive::currentContext();
  m_gctx = m_trackingGeometryTool->getNominalGeometryContext().context();

  // convert to Acts::BoundTrackParameters
  std::vector<Acts::BoundTrackParameters> trackParameters{};
  for (const Trk::Track *track : tracks) {
    trackParameters.push_back(getParametersFromTrack(track->trackParameters()->front()));
  }

  // extrapolate to nSteps z positions betweeen zMin and zMax and calculate center of tracks and
  // chi2 for each position.
  size_t nSteps = (m_zMax - m_zMin) / m_stepSize + 1;
  m_allVertexCandidates.reserve(nSteps);
  for (int z = m_zMin; z <= m_zMax; z += m_stepSize) {
    auto vertexCandidate = getVertexCandidate(trackParameters, z);
    if (vertexCandidate) {
      m_allVertexCandidates.push_back(*vertexCandidate);
    }
  }

  // find vertex candidates with minium (local) chi2 value
  for (size_t i = 1; i < nSteps - 1; ++i) {
    if ((m_allVertexCandidates[i - 1].chi2 > m_allVertexCandidates[i].chi2) and
        (m_allVertexCandidates[i + 1].chi2 > m_allVertexCandidates[i].chi2)) {
      m_goodVertexCandidates.push_back(m_allVertexCandidates[i]);
    }
  }

  // Check if there are one or two local minima and calculate mean in the case of two minima.
  if (m_goodVertexCandidates.size() == 0) {
    ATH_MSG_DEBUG("Cannot find minimum. Extrapolation failed. This is likely an error.");
    return {};
  } else if (m_goodVertexCandidates.size() == 1) {
    const VertexCandidate &vx = m_goodVertexCandidates.front();
    return POCA(vx.position, vx.chi2, tracks);
  } else if (m_goodVertexCandidates.size() == 2) {
    const VertexCandidate &vx0 = m_goodVertexCandidates.at(0);
    const VertexCandidate &vx1 = m_goodVertexCandidates.at(1);
    return POCA(0.5 * (vx0.position + vx1.position), 0.5 * (vx0.chi2 + vx1.chi2), tracks);
  } else {
    ATH_MSG_DEBUG("Cannot find global minimum. Expect a maximum of two local chi2 minima but found "
                  << m_goodVertexCandidates.size() << " minima.");
    return {};
  }
}

// Calculate center of tracks
Eigen::Vector2d PointOfClosestApproachSearchTool::getCenter(
    const std::vector<PointOfClosestApproachSearchTool::TrackPosition> &trackPositions) const {
  Eigen::Vector2d center{0, 0};
  Eigen::Vector2d sumInverse{0, 0};
  for (const TrackPosition &value : trackPositions) {
    center(0) += value.inv(0, 0) * value.pos(0);
    sumInverse(0) += value.inv(0, 0);
    center(1) += value.inv(1, 1) * value.pos(1);
    sumInverse(1) += value.inv(1, 1);
  }
  center(0) /= sumInverse(0);
  center(1) /= sumInverse(1);
  return center;
}

// Extrapolate track to given z positon.
std::unique_ptr<const Acts::BoundTrackParameters>
PointOfClosestApproachSearchTool::extrapolateTrack(const Trk::Track *track,
                                                   double targetPosition) const {
  Acts::BoundTrackParameters startParameters =
      getParametersFromTrack(track->trackParameters()->front());
  auto targetSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3(0, 0, targetPosition), Acts::Vector3(0, 0, 1));
  Acts::NavigationDirection navigationDirection =
      targetPosition > startParameters.referenceSurface().center(m_gctx).z() ? Acts::forward
                                                                             : Acts::backward;
  return m_extrapolationTool->propagate(m_ctx, startParameters, *targetSurface,
                                        navigationDirection);
}

std::optional<PointOfClosestApproachSearchTool::TrackPosition>
PointOfClosestApproachSearchTool::extrapolateTrackParameters(
    const Acts::BoundTrackParameters &startParameters, double z) const {
  auto targetSurface =
      Acts::Surface::makeShared<Acts::PlaneSurface>(Acts::Vector3(0, 0, z), Acts::Vector3(0, 0, 1));
  Acts::NavigationDirection navigationDirection =
      z > startParameters.referenceSurface().center(m_gctx).z() ? Acts::forward : Acts::backward;
  std::unique_ptr<const Acts::BoundTrackParameters> targetParameters =
      m_extrapolationTool->propagate(m_ctx, startParameters, *targetSurface, navigationDirection);
  if (targetParameters != nullptr && targetParameters->covariance().has_value()) {
    // extrapolate covariance matrix?
    // return TrackPosition{targetParameters->position(m_gctx),
    // startParameters.covariance().value().topLeftCorner(2, 2)};
    return TrackPosition(targetParameters->position(m_gctx),
                         targetParameters->covariance().value().topLeftCorner(2, 2));
  } else {
    ATH_MSG_DEBUG("Extrapolation failed.");
    return std::nullopt;
  }
}

std::optional<PointOfClosestApproachSearchTool::VertexCandidate>
PointOfClosestApproachSearchTool::getVertexCandidate(
    std::vector<Acts::BoundTrackParameters> &trackParameters, double z) const {
  // extrapolate tracks to given z position
  std::vector<TrackPosition> trackPositions{};
  for (const Acts::BoundTrackParameters &startParameters : trackParameters) {
    auto tp = extrapolateTrackParameters(startParameters, z);
    if (tp) {
      trackPositions.push_back(*tp);
    }
  }
  if (trackPositions.size() < 2) {
    ATH_MSG_DEBUG(
        "Extrapolation failed for too many tracks. Require atleast 2 extrapolated positions.");
    return std::nullopt;
  }

  // calculate center weighted by inverse covariance matrix
  Eigen::Vector2d trackCenter = getCenter(trackPositions);

  // either use weighted x and y position or only y position to calculate chi2 value
  double vertexChi2 = chi2Y(trackPositions, trackCenter);

  return PointOfClosestApproachSearchTool::VertexCandidate(z, vertexChi2, trackCenter);
}

double PointOfClosestApproachSearchTool::chi2(const std::vector<TrackPosition> &trackPositions,
                                              const Eigen::Vector2d &trackCenter) const {
  double chi2 = 0;
  for (const TrackPosition &value : trackPositions) {
    Eigen::Vector2d delta = trackCenter - value.pos;
    chi2 += delta.dot(value.inv.diagonal().cwiseProduct(delta));
  }
  return chi2;
}

double PointOfClosestApproachSearchTool::chi2Y(const std::vector<TrackPosition> &trackPositions,
                                               const Eigen::Vector2d &trackCenter) const {
  double chi2 = 0;
  for (const TrackPosition &value : trackPositions) {
    Eigen::Vector2d delta = trackCenter - value.pos;
    chi2 += delta.y() * value.inv(1, 1) * delta.y();
  }
  return chi2;
}

Acts::BoundTrackParameters PointOfClosestApproachSearchTool::getParametersFromTrack(
    const Trk::TrackParameters *trackParameters) const {
  using namespace Acts::UnitLiterals;

  Acts::GeometryContext geometryContext =
      m_trackingGeometryTool->getNominalGeometryContext().context();
  Acts::AngleAxis3 rotZ(M_PI / 2, Acts::Vector3(0., 0., 1.));
  Acts::Translation3 translation{0., 0., trackParameters->associatedSurface().center().z()};
  auto transform = Acts::Transform3(translation * rotZ);
  auto surface = Acts::Surface::makeShared<Acts::PlaneSurface>(transform);

  auto bound = Acts::detail::transformFreeToBoundParameters(
      trackParameters->position(), 0, trackParameters->momentum(),
      trackParameters->charge() / trackParameters->momentum().mag() / 1_MeV, *surface,
      geometryContext);
  if (!bound.ok()) {
    ATH_MSG_WARNING("FreeToBound parameter transformation failed");
  }
  // convert covariance matrix to GeV
  Acts::BoundSymMatrix cov = Acts::BoundSymMatrix::Identity();
  cov.topLeftCorner(5, 5) = *trackParameters->covariance();
  for (int i = 0; i < cov.rows(); i++) {
    cov(i, 4) = cov(i, 4) / 1_MeV;
  }
  for (int i = 0; i < cov.cols(); i++) {
    cov(4, i) = cov(4, i) / 1_MeV;
  }
  return Acts::BoundTrackParameters{surface, bound.value(), trackParameters->charge(), cov};
}

void PointOfClosestApproachSearchTool::clear() const {
  m_allVertexCandidates.clear();
  m_goodVertexCandidates.clear();
}

} // namespace FaserTracking
