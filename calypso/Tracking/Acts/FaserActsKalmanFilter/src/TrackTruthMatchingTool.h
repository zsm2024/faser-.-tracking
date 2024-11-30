#ifndef FASERACTSKALMANFILTER_TRACKTRUTHMATCHINGTOOL_H
#define FASERACTSKALMANFILTER_TRACKTRUTHMATCHINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "FaserActsKalmanFilter/ITrackTruthMatchingTool.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include "TrkTrack/Track.h"
#include "xAODTruth/TruthParticleContainer.h"

class TrackTruthMatchingTool
    : public extends<AthAlgTool, ITrackTruthMatchingTool> {
public:
  TrackTruthMatchingTool(const std::string &type, const std::string &name,
                         const IInterface *parent);
  virtual ~TrackTruthMatchingTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  std::pair<const xAOD::TruthParticle*, int>
  getTruthParticle(const Trk::Track *track) const;

private:
  struct ParticleHitCount {
    int barcode;
    size_t hitCount;
  };
  static void increaseHitCount(std::vector<ParticleHitCount> &particleHitCounts,
                               int particleId);
  static void sortHitCount(std::vector<ParticleHitCount> &particleHitCounts);
  static void identifyContributingParticles(
      const Trk::Track &track, const TrackerSimDataCollection &simDataCollection,
      std::vector<ParticleHitCount> &particleHitCounts);

  SG::ReadHandleKey<TrackerSimDataCollection> m_simDataCollectionKey{
      this, "TrackerSimDataCollection", "SCT_SDO_Map"};
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleContainerKey{
      this, "ParticleContainer", "TruthParticles"};
};

#endif /* FASERACTSKALMANFILTER_TRACKTRUTHMATCHINGTOOL_H */
