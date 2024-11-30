#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackClassification.h"
#include "FaserActsTrack.h"
#include "FaserActsKalmanFilter/IndexSourceLink.h"

namespace {

/// Increase the hit count for the given particle id by one.
inline void increaseHitCount(std::vector<ParticleHitCount> &particleHitCounts, int particleId) {
  // linear search since there is no ordering
  auto it = std::find_if(particleHitCounts.begin(), particleHitCounts.end(),
                         [=](const ParticleHitCount &phc) {
                           return (phc.particleId == particleId);
                         });
  // either increase count if we saw the particle before or add it
  if (it != particleHitCounts.end()) {
    it->hitCount += 1u;
  } else {
    particleHitCounts.push_back({particleId, 1u});
  }
}

/// Sort hit counts by decreasing values, i.e. majority particle comes first.
inline void sortHitCount(std::vector<ParticleHitCount> &particleHitCounts) {
  std::sort(particleHitCounts.begin(), particleHitCounts.end(),
            [](const ParticleHitCount &lhs, const ParticleHitCount &rhs) {
              return (lhs.hitCount > rhs.hitCount);
            });
}

}  // namespace


/// Identify all particles that contribute to a trajectory.
void identifyContributingParticles(
    const TrackerSimDataCollection& simDataCollection,
    const FaserActsTrackContainer::ConstTrackProxy& track,
    std::vector<ParticleHitCount>& particleHitCounts) {
  particleHitCounts.clear();

  for (const auto& state : track.trackStatesReversed()) {   
    // no truth info with non-measurement state
    if (not state.typeFlags().test(Acts::TrackStateFlag::MeasurementFlag)) {
      continue;
    }
  
    std::vector<int> barcodes {};
    // register all particles that generated this hit
    if(not state.hasUncalibratedSourceLink()){
      throw std::runtime_error("The measurement state does not have a source link?");
    }
    IndexSourceLink sl = state.getUncalibratedSourceLink().template get<IndexSourceLink>();
    if(sl.hit()==nullptr){
      throw std::runtime_error("The source hit is empty");
    }
    for (const Identifier &id : sl.hit()->rdoList()) {
      if (simDataCollection.count(id) == 0) {
        continue;
      }
      const auto &deposits = simDataCollection.find(id)->second.getdeposits();
      for (const TrackerSimData::Deposit &deposit : deposits) {
        int barcode = deposit.first.barcode();
        if (std::find(barcodes.begin(), barcodes.end(), barcode) == barcodes.end()) {
          barcodes.push_back(barcode);
          increaseHitCount(particleHitCounts, barcode);
        }
      }
    }
  }
  sortHitCount(particleHitCounts);
}

/* Identify all particles that contribute to a trajectory.
 * If a cluster consists of multiple RDOs we check for each from which particle(s) it has been created.
 * And if multiple particles created a RDO we increase the hit count for each of them.
 */
void identifyContributingParticles(
    const TrackerSimDataCollection& simDataCollection,
    const std::vector<const Tracker::FaserSCT_Cluster*> clusters,
    std::vector<ParticleHitCount>& particleHitCounts) {
  particleHitCounts.clear();
  for (const Tracker::FaserSCT_Cluster *cluster : clusters) {
    std::vector<int> barcodes {};
    for (const Identifier &id : cluster->rdoList()) {
      if (simDataCollection.count(id) == 0) continue;
      const auto &deposits = simDataCollection.find(id)->second.getdeposits();
      for (const TrackerSimData::Deposit &deposit : deposits) {
        int barcode = deposit.first.barcode();
        if (std::find(barcodes.begin(), barcodes.end(), barcode) == barcodes.end()) {
          barcodes.push_back(barcode);
          increaseHitCount(particleHitCounts, barcode);
        }
      }
    }
  }
  sortHitCount(particleHitCounts);
}

