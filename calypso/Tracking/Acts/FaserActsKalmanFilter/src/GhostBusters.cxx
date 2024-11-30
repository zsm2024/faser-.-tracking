#include "GhostBusters.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "Identifier/Identifier.h"


GhostBusters::GhostBusters(const std::string &name, ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator), AthHistogramming(name),
      m_histSvc("THistSvc/THistSvc", name) {}


StatusCode GhostBusters::initialize() {
  ATH_CHECK(m_trackCollection.initialize());
  // ATH_CHECK(m_simDataCollectionKey.initialize());
  ATH_CHECK(m_outputTrackCollection.initialize());
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));

  m_tree = new TTree("tree", "tree");
  m_tree->Branch("event_number", &m_event_number, "event_number/I");
  m_tree->Branch("station", &m_station, "station/I");
  m_tree->Branch("x", &m_x, "x/D");
  m_tree->Branch("y", &m_y, "y/D");
  m_tree->Branch("z", &m_z, "z/D");
  m_tree->Branch("chi2", &m_chi2, "chi2/D");
  // m_tree->Branch("majorityHits", &m_majorityHits, "majorityHits/I");
  m_tree->Branch("size", &m_size, "size/I");
  m_tree->Branch("isGhost", &m_isGhost, "isGhost/B");
  ATH_CHECK(histSvc()->regTree("/HIST2/tree", m_tree));

  return StatusCode::SUCCESS;
}


StatusCode GhostBusters::execute(const EventContext &ctx) const {
  m_event_number = ctx.eventID().event_number();

  SG::WriteHandle outputTrackCollection {m_outputTrackCollection, ctx};
  std::unique_ptr<TrackCollection> outputTracks = std::make_unique<TrackCollection>();

  // SG::ReadHandle<TrackerSimDataCollection> simData {m_simDataCollectionKey, ctx};
  // ATH_CHECK(simData.isValid());

  SG::ReadHandle<TrackCollection> trackCollection {m_trackCollection, ctx};
  ATH_CHECK(trackCollection.isValid());

  std::map<int, std::vector<Segment>> segments {};
  for (const Trk::Track* track : *trackCollection) {
    auto segment = Segment(track, m_idHelper);
    segments[segment.station()].push_back(segment);
  }

  for (const auto &station : segments) {
    double nGoodSegments = 0;
    std::vector<Segment> stationSegments = station.second;
    std::sort(stationSegments.begin(), stationSegments.end(), [](const Segment &lhs, const Segment &rhs) { return lhs.y() > rhs.y(); });
    double maxX = stationSegments.front().x();
    double maxY = stationSegments.front().y();
    double minX = stationSegments.back().x();
    double minY = stationSegments.back().y();
    double deltaY = maxY - minY;
    double meanX = 0.5 * (minX + maxX);
    double meanY = 0.5 * (minY + maxY);
    double deltaX = deltaY / (2 * std::tan(0.02));
    for (Segment &segment : stationSegments) {
      bool isGhost = (segment.y() > meanY - m_yTolerance * deltaY) && (segment.y() < meanY + m_yTolerance * deltaY) && (
          ((segment.x() > meanX - (1 + m_xTolerance) * deltaX) &&
           (segment.x() < meanX - (1 - m_xTolerance) * deltaX)) ||
          ((segment.x() > meanX + (1 - m_xTolerance) * deltaX) && (segment.x() < meanX + (1 + m_xTolerance) * deltaX)));
      if (isGhost) segment.setGhost();
      if (not isGhost && segment.size() >= 5) nGoodSegments++;
    }
    for (const Segment &segment : stationSegments) {
      m_x = segment.x();
      m_y = segment.y();
      m_z = segment.z();
      m_chi2 = segment.chi2();
      m_station = segment.station();
      m_size = segment.size();
      // m_majorityHits = segment.majorityHits();
      m_isGhost = segment.isGhost();
      if (nGoodSegments >= 2 && segment.size() == 4) m_isGhost = true;
      m_tree->Fill();
      if (segment.isGhost()) continue;
      if (nGoodSegments >= 2 && segment.size() == 4) continue;
      outputTracks->push_back(new Trk::Track(*segment.track()));
    }
  }

  ATH_CHECK(outputTrackCollection.record(std::move(outputTracks)));
  return StatusCode::SUCCESS;
}


StatusCode GhostBusters::finalize() {
  return StatusCode::SUCCESS;
}


GhostBusters::Segment::Segment(const Trk::Track *track, const FaserSCT_ID *idHelper) {
  m_track = track;
  m_position = track->trackParameters()->front()->position();
  m_chi2 = track->fitQuality()->chiSquared();
  std::vector<const Tracker::FaserSCT_Cluster*> clusters {};
  for (const Trk::TrackStateOnSurface* trackState : *(track->trackStateOnSurfaces())) {
    auto clusterOnTrack = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*> (trackState->measurementOnTrack());
    if (clusterOnTrack) {
      clusters.emplace_back(clusterOnTrack->prepRawData());
      m_station = idHelper->station(clusterOnTrack->identify());
    }
  }
  m_size = clusters.size();
  // identifyContributingParticles(simDataCollection, clusters, m_particleHitCounts);
}
