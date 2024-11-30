#include "TrackSelection.h"

// #include "TrackerPrepRawData/FaserSCT_Cluster.h"

namespace {

inline void sortTracks(std::vector<TrackQuality>& tracks) {
  std::sort(tracks.begin(), tracks.end(),
            [](const TrackQuality& lhs, const TrackQuality& rhs) {
    if (lhs.nMeasurements != rhs.nMeasurements) {
      return lhs.nMeasurements > rhs.nMeasurements;
    }
    return lhs.chi2 < rhs.chi2;
  });
}

}  // namespace


void selectTracks(FaserActsTrackContainer& tracks, std::vector<TrackQuality>& trackQuality) {
  for (const auto& track : tracks) {
          
    
    //auto& ckfResult = result.value();
    //
    //auto traj = FaserActsRecMultiTrajectory(ckfResult.fittedStates, ckfResult.lastMeasurementIndices, ckfResult.fittedParameters);
    //const auto& mj = traj.multiTrajectory();
    //const auto& trackTips = traj.tips();
    //auto it = std::max_element(trackTips.begin(), trackTips.end(),
    //                           [&](const size_t& lhs, const size_t& rhs) {
    //                             auto trajState_lhs = Acts::MultiTrajectoryHelpers::trajectoryState(mj, lhs);
    //                             auto trajState_rhs = Acts::MultiTrajectoryHelpers::trajectoryState(mj, rhs);
    //                             if (trajState_lhs.nMeasurements != trajState_rhs.nMeasurements) {
    //                               return trajState_lhs.nMeasurements > trajState_rhs.nMeasurements;
    //                             }
    //                             return trajState_lhs.chi2Sum < trajState_rhs.chi2Sum;
    //                           });
    //auto trajState = Acts::MultiTrajectoryHelpers::trajectoryState(mj, *it);
    trackQuality.push_back({track, track.nMeasurements(), track.chi2()});
  }
  sortTracks(trackQuality);
}
