#ifndef FASERACTSKALMANFILTER_TRACKCLASSIFICATION_H
#define FASERACTSKALMANFILTER_TRACKCLASSIFICATION_H

// Ensure Eigen plugin comes first
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "FaserActsTrack.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"

struct ParticleHitCount {
  int particleId;
  size_t hitCount;
};

/// Identify all particles that contribute to a trajectory.
void identifyContributingParticles(
    const TrackerSimDataCollection& simDataCollection,
    const FaserActsTrackContainer::ConstTrackProxy& track, 
    std::vector<ParticleHitCount>& particleHitCounts);

void identifyContributingParticles(
    const TrackerSimDataCollection& simDataCollection,
    const std::vector<const Tracker::FaserSCT_Cluster*> clusters,
    std::vector<ParticleHitCount>& particleHitCounts);

#endif  // FASERACTSKALMANFILTER_TRACKCLASSIFICATION_H
