#ifndef FASERACTSKALMANFILTER_AMBIGUITYSOLVER_H
#define FASERACTSKALMANFILTER_AMBIGUITYSOLVER_H

#include "Acts/TrackFinding/CombinatorialKalmanFilter.hpp"
#include "FaserActsRecMultiTrajectory.h"

using CombinatorialKalmanFilterResult = Acts::CombinatorialKalmanFilterResult<IndexSourceLink>;
using TrackFitterResult = Acts::Result<CombinatorialKalmanFilterResult>;
using TrackFinderResult = std::vector<TrackFitterResult>;


size_t numberMeasurements(const CombinatorialKalmanFilterResult& ckfResult) {
  auto traj = FaserActsRecMultiTrajectory(ckfResult.fittedStates, ckfResult.lastMeasurementIndices, ckfResult.fittedParameters);
  const auto& mj = traj.multiTrajectory();
  const auto& trackTips = traj.tips();
  size_t maxMeasurements = 0;
  for (const auto& trackTip : trackTips) {
    auto trajState = Acts::MultiTrajectoryHelpers::trajectoryState(mj, trackTip);
    size_t nMeasurements = trajState.nMeasurements;
    if (nMeasurements > maxMeasurements) {
      maxMeasurements = nMeasurements;
    }
    std::cout << "# measurements: " << trajState.nMeasurements << std::endl;
  }
  return maxMeasurements;
}

int countSharedHits(const CombinatorialKalmanFilterResult& result1, const CombinatorialKalmanFilterResult& result2) {
  int count = 0;
  std::vector<size_t> hitIndices {};

  for (auto measIndex : result1.lastMeasurementIndices) {
    result1.fittedStates.visitBackwards(measIndex, [&](const auto& state) {
      if (not state.typeFlags().test(Acts::TrackStateFlag::MeasurementFlag))
        return;
      size_t hitIndex = state.uncalibrated().index();
      hitIndices.emplace_back(hitIndex);
    });
  }

  for (auto measIndex : result2.lastMeasurementIndices) {
    result2.fittedStates.visitBackwards(measIndex, [&](const auto& state) {
      if (not state.typeFlags().test(Acts::TrackStateFlag::MeasurementFlag))
        return;
      size_t hitIndex = state.uncalibrated().index();
      if (std::find(hitIndices.begin(), hitIndices.end(), hitIndex) != hitIndices.end()) {
        count += 1;
      }
    });
  }
  return count;
}


std::pair<int, int> solveAmbiguity(TrackFinderResult& results, size_t minMeasurements = 13) {
  std::map<std::pair<size_t, size_t>, size_t> trackPairs {};
  for (size_t i = 0; i < results.size(); ++i) {
    // if (not results.at(i).ok()) continue;
    // if (numberMeasurements(results.at(i).value()) < minMeasurements) continue;
    for (size_t j = i+1; j < results.size(); ++j) {
      // if (not results.at(j).ok()) continue;
      // if (numberMeasurements(results.at(j).value()) < minMeasurements) continue;
      int n = countSharedHits(results.at(i).value(), results.at(j).value());
      trackPairs[std::make_pair(i, j)] = n;
    }
  }

  std::pair<size_t, size_t> bestTrackPair;
  size_t minSharedHits = std::numeric_limits<size_t>::max();
  for (const auto& trackPair : trackPairs) {
    if (trackPair.second < minSharedHits) {
      minSharedHits = trackPair.second;
      bestTrackPair = trackPair.first;
    }
  }

  return bestTrackPair;
}

#endif //FASERACTSKALMANFILTER_AMBIGUITYSOLVER_H
