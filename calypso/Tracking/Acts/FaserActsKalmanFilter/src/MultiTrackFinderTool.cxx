#include "MultiTrackFinderTool.h"

#include "Acts/Definitions/TrackParametrization.hpp"
#include "Acts/EventData/Measurement.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "FaserActsKalmanFilter/IndexSourceLink.h"
#include "Identifier/Identifier.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
//#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include <algorithm>
#include <vector>


MultiTrackFinderTool::MultiTrackFinderTool(const std::string& type, const std::string& name, const IInterface* parent)
    : base_class(type, name, parent) {}


StatusCode MultiTrackFinderTool::initialize() {
      ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
      ATH_CHECK(detStore()->retrieve(m_detManager, "SCT"));
      ATH_CHECK(m_trackingGeometryTool.retrieve());
      ATH_CHECK(m_trackCollection.initialize());
  return StatusCode::SUCCESS;
}


StatusCode MultiTrackFinderTool::run() {
  SG::ReadHandle<TrackCollection> trackCollection {m_trackCollection};
      ATH_CHECK(trackCollection.isValid());

  using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;
  std::shared_ptr<IdentifierMap> identifierMap = m_trackingGeometryTool->getIdentifierMap();

  std::map<int, std::vector<Tracklet>> tracklets;
  for (const Trk::Track* track : *trackCollection) {
    std::vector<Identifier> ids;
    std::vector<Acts::GeometryIdentifier> geoIds;
    std::vector<double> positions;
    std::vector<const Tracker::FaserSCT_Cluster*> clusters {};
    auto fitParameters = track->trackParameters()->front();
    const Amg::Vector3D fitPosition = fitParameters->position();
    const Amg::Vector3D fitMomentum = fitParameters->momentum();
    ATH_MSG_DEBUG(fitPosition.x() << ", " << fitPosition.y() << fitPosition.z());
    for (const Trk::TrackStateOnSurface* state : *(track->trackStateOnSurfaces())) {
      auto clusterOnTrack = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*> ( state->measurementOnTrack());
      if (clusterOnTrack) {
        const Tracker::FaserSCT_Cluster* cluster = clusterOnTrack->prepRawData();
        clusters.push_back(cluster);
        Identifier id = cluster->detectorElement()->identify();
        ids.push_back(id);
        Acts::GeometryIdentifier geoId = identifierMap->at(id);
        geoIds.push_back(geoId);
        const auto& par = cluster->localPosition();
        positions.push_back(par.x());
      }
    }
    auto tracklet = Tracklet(ids, geoIds, positions, fitPosition, clusters);
    int station = m_idHelper->station(ids.front());
    tracklets[station].push_back(tracklet);
  }

  // create all combinations of tracklets
  std::vector<ProtoTrack> protoTracks {};
  for (const Tracklet& t1 : tracklets[1]) {
    for (const Tracklet& t2 : tracklets[2]) {
      for (const Tracklet& t3 : tracklets[3]) {
        protoTracks.push_back(ProtoTrack(t1, t2, t3));
      }
    }
  }

  // sort proto tracks by their chi2 value and select best two tracks
  std::sort(protoTracks.begin(), protoTracks.end(), sort_chi2());

  std::vector<std::vector<IndexSourceLink>> sourceLinkVector;
  std::vector<std::vector<Measurement>> measurementVector;
  std::vector<std::map<Index, Identifier>> idLinkVector;
  std::vector<Acts::BoundTrackParameters> paramVector;
  std::vector<std::vector<const Tracker::FaserSCT_Cluster*>> clusterVector;
  int n_protoTracks = std::min((int)protoTracks.size(), 2);
  for (int i = 0; i < n_protoTracks; ++i) {
    // FIXME check if protoTrack exists
    ProtoTrack track = protoTracks[i];
    auto [measurements, sourceLinks, idLinks, clusters] = track.run();
    auto params = track.initialTrackParameters(m_covLoc0, m_covLoc1, m_covPhi, m_covTheta, m_covQOverP, m_covTime);
    sourceLinkVector.push_back(sourceLinks);
    measurementVector.push_back(measurements);
    idLinkVector.push_back(idLinks);
    paramVector.push_back(params);
    clusterVector.push_back(clusters);
  }

  m_sourceLinks = std::make_shared<std::vector<std::vector<IndexSourceLink>>>(sourceLinkVector);
  m_measurements = std::make_shared<std::vector<std::vector<Measurement>>>(measurementVector);
  m_idLinks = std::make_shared<std::vector<std::map<Index, Identifier>>>(idLinkVector);
  m_initialTrackParameters = std::make_shared<std::vector<Acts::BoundTrackParameters>>(paramVector);
  m_clusters = std::make_shared<std::vector<std::vector<const Tracker::FaserSCT_Cluster*>>>(clusterVector);

  // create initial surface
  m_initialSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3 {0, 0, 0}, Acts::Vector3{0, 0, 1});

  return StatusCode::SUCCESS;
}


StatusCode MultiTrackFinderTool::finalize() {
  return StatusCode::SUCCESS;
}



