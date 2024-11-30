#ifndef FASERACTSVERTEXING_IVERTEXINGTOOL_H
#define FASERACTSVERTEXING_IVERTEXINGTOOL_H

#include "Acts/EventData/TrackParameters.hpp"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "TrkTrack/Track.h"
#include <vector>

namespace FaserTracking {

// Point of closest approach between n tracks. Contains the three-dimensional
// reconstructed position, the chi2 value of the vertex fit and the tracks that
// originate from this vertex.
struct POCA {
  POCA(Eigen::Vector3d vertex, double chi2, std::vector<const Trk::Track *> tracks)
      : position(vertex), chi2(chi2), tracks(tracks) {}
  Eigen::Vector3d position;
  double chi2;
  std::vector<const Trk::Track *> tracks;
};

class IVertexingTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IVertexingTool, 1, 0);

  // Run vertex fit and return the fitted POCA.
  virtual std::optional<POCA> getVertex(const std::vector<const Trk::Track *> &tracks) const = 0;

  // Get bound track parameters at given z position.
  virtual std::unique_ptr<const Acts::BoundTrackParameters>
  extrapolateTrack(const Trk::Track *track, double targetPosition) const = 0;
};

} // namespace FaserTracking

#endif // FASERACTSVERTEXING_IVERTEXINGTOOL_H
