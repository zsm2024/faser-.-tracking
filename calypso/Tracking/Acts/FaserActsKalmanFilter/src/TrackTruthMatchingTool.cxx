#include "TrackTruthMatchingTool.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"

TrackTruthMatchingTool::TrackTruthMatchingTool(const std::string &type,
                                               const std::string &name,
                                               const IInterface *parent)
    : base_class(type, name, parent) {}

StatusCode TrackTruthMatchingTool::initialize() {
  ATH_CHECK(m_simDataCollectionKey.initialize());
  ATH_CHECK(m_truthParticleContainerKey.initialize());
  return StatusCode::SUCCESS;
}

std::pair<const xAOD::TruthParticle *, int>
TrackTruthMatchingTool::getTruthParticle(const Trk::Track *track) const {
  const xAOD::TruthParticle *truthParticle = nullptr;
  const EventContext &ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<xAOD::TruthParticleContainer> truthParticleContainer{
      m_truthParticleContainerKey, ctx};
  if (!truthParticleContainer.isValid()) {
    ATH_MSG_WARNING("xAOD::TruthParticleContainer is not valid.");
    return {truthParticle, -1};
  }
  SG::ReadHandle<TrackerSimDataCollection> simDataCollection{
      m_simDataCollectionKey, ctx};
  if (!simDataCollection.isValid()) {
    ATH_MSG_WARNING("TrackerSimDataCollection is not valid.");
    return {truthParticle, -1};
  }
  std::vector<ParticleHitCount> particleHitCounts{};
  identifyContributingParticles(*track, *simDataCollection, particleHitCounts);
  if (particleHitCounts.empty()) {
    ATH_MSG_WARNING("Cannot find any truth particle matched to the track.");
    return {truthParticle, -1};
  }
  int barcode = particleHitCounts.front().barcode;
  int hitCount = particleHitCounts.front().hitCount;
  auto it = std::find_if(truthParticleContainer->begin(),
                         truthParticleContainer->end(),
                         [barcode](const xAOD::TruthParticle_v1 *particle) {
                           return particle->barcode() == barcode;
                         });
  if (it == truthParticleContainer->end()) {
    ATH_MSG_WARNING("Cannot find particle with barcode "
                    << barcode << " in truth particle container.");
    return {truthParticle, -1};
  }
  truthParticle = *it;
  return {truthParticle, hitCount};
}

StatusCode TrackTruthMatchingTool::finalize() { return StatusCode::SUCCESS; }

void TrackTruthMatchingTool::increaseHitCount(
    std::vector<ParticleHitCount> &particleHitCounts, int barcode) {
  auto it = std::find_if(
      particleHitCounts.begin(), particleHitCounts.end(),
      [=](const ParticleHitCount &phc) { return (phc.barcode == barcode); });
  // either increase count if we saw the particle before or add it
  if (it != particleHitCounts.end()) {
    it->hitCount += 1u;
  } else {
    particleHitCounts.push_back({barcode, 1u});
  }
}

void TrackTruthMatchingTool::sortHitCount(
    std::vector<ParticleHitCount> &particleHitCounts) {
  std::sort(particleHitCounts.begin(), particleHitCounts.end(),
            [](const ParticleHitCount &lhs, const ParticleHitCount &rhs) {
              return (lhs.hitCount > rhs.hitCount);
            });
}

void TrackTruthMatchingTool::identifyContributingParticles(
    const Trk::Track &track, const TrackerSimDataCollection &simDataCollection,
    std::vector<ParticleHitCount> &particleHitCounts) {
  for (const Trk::MeasurementBase *meas : *track.measurementsOnTrack()) {
    const auto *clusterOnTrack =
        dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack *>(meas);
    if (!clusterOnTrack)
      continue;
    std::vector<int> barcodes{};
    const Tracker::FaserSCT_Cluster *cluster = clusterOnTrack->prepRawData();
    for (Identifier id : cluster->rdoList()) {
      if (simDataCollection.count(id) == 0)
        continue;
      const auto &deposits = simDataCollection.at(id).getdeposits();
      for (const TrackerSimData::Deposit &deposit : deposits) {
        int barcode = deposit.first.barcode();
        // count each barcode only once for a wafer
        if (std::find(barcodes.begin(), barcodes.end(), barcode) ==
            barcodes.end()) {
          barcodes.push_back(barcode);
          increaseHitCount(particleHitCounts, barcode);
        }
      }
    }
  }
  sortHitCount(particleHitCounts);
}
