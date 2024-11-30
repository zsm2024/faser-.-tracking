#ifndef FASERACTSKALMANFILTER_TRACKSELECTION_H
#define FASERACTSKALMANFILTER_TRACKSELECTION_H

#include "FaserActsTrack.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include "Acts/TrackFinding/CombinatorialKalmanFilter.hpp"
#include "Acts/EventData/TrackContainer.hpp"
#include "Acts/EventData/TrackProxy.hpp"
#include "Acts/EventData/VectorTrackContainer.hpp"
#include "Acts/EventData/VectorMultiTrajectory.hpp"


//using TrackFitterResult =
//Acts::Result<Acts::CombinatorialKalmanFilterResult<IndexSourceLink>>;


//@todo: make this constTrackContainer
struct TrackQuality {
  FaserActsTrackContainer::TrackProxy track; 
  size_t nMeasurements;
  double chi2;
};

void selectTracks(FaserActsTrackContainer& tracks, std::vector<TrackQuality>& trackQuality);

#endif  // FASERACTSKALMANFILTER_TRACKSELECTION_H
