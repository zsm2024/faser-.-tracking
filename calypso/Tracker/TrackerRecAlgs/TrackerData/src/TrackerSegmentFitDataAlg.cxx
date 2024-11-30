#include "TrackerSegmentFitDataAlg.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include <TTree.h>


namespace Tracker {

TrackerSegmentFitDataAlg::TrackerSegmentFitDataAlg(const std::string &name, ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator), AthHistogramming(name),
      m_histSvc("THistSvc/THistSvc", name), m_detMgr(nullptr), m_idHelper(nullptr) {}


StatusCode TrackerSegmentFitDataAlg::initialize() {
  ATH_CHECK(detStore()->retrieve(m_detMgr, "SCT"));
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
  ATH_CHECK(m_trackCollection.initialize());

  m_tracks = new TTree("tracks", "tracks");
  m_tracks->Branch("run_number", &m_run_number, "run_number/I");
  m_tracks->Branch("event_number", &m_event_number, "event_number/I");
  m_tracks->Branch("chi2", &m_chi2, "chi2/I");
  m_tracks->Branch("dof", &m_nDOF, "dof/I");
  m_tracks->Branch("number_clusters", &m_nClusters, "clusters/I");
  m_tracks->Branch("x", &m_track_x, "x/D");
  m_tracks->Branch("y", &m_track_y, "y/D");
  m_tracks->Branch("z", &m_track_z, "z/D");
  m_tracks->Branch("px", &m_track_px, "px/D");
  m_tracks->Branch("py", &m_track_py, "py/D");
  m_tracks->Branch("pz", &m_track_pz, "pz/D");
  ATH_CHECK(histSvc()->regTree("/HIST2/tracks", m_tracks));

  m_hits = new TTree("hits", "hits");
  m_hits->Branch("run_number", &m_run_number, "run_number/I");
  m_hits->Branch("event_number", &m_event_number, "event_number/I");
  m_hits->Branch("station", &m_station, "station/I");
  m_hits->Branch("plane", &m_plane, "plane/I");
  m_hits->Branch("module", &m_module, "module/I");
  m_hits->Branch("row", &m_row, "row/I");
  m_hits->Branch("sensor", &m_sensor, "sensor/I");
  m_hits->Branch("x", &m_x, "x/D");
  m_hits->Branch("y", &m_y, "y/D");
  m_hits->Branch("z", &m_z, "z/D");
  ATH_CHECK(histSvc()->regTree("/HIST2/hits", m_hits));

  return StatusCode::SUCCESS;
}

StatusCode TrackerSegmentFitDataAlg::execute(const EventContext &ctx) const {

  m_run_number = ctx.eventID().run_number();
  m_event_number = ctx.eventID().event_number();

  // Fill tree with clusters from tracker segment fit
  SG::ReadHandle<TrackCollection> trackCollection {m_trackCollection, ctx};
  ATH_CHECK(trackCollection.isValid());

  for (const Trk::Track* track : *trackCollection) {
    m_chi2 = track->fitQuality()->chiSquared();
    m_nDOF = track->fitQuality()->numberDoF();
    m_nClusters = track->trackParameters()->size() - 1;
    auto fitParameters = track->trackParameters()->front();
    const Amg::Vector3D fitPosition = fitParameters->position();
    const Amg::Vector3D fitMomentum = fitParameters->momentum();
    m_track_x = fitPosition.x();
    m_track_y = fitPosition.y();
    m_track_z = fitPosition.z();
    m_track_px = fitMomentum.x();
    m_track_py = fitMomentum.y();
    m_track_pz = fitMomentum.z();
    m_tracks->Fill();

    for (const Trk::TrackStateOnSurface* trackState : *track->trackStateOnSurfaces()) {
      const auto* clusterOnTrack =
          dynamic_cast<const FaserSCT_ClusterOnTrack*>(trackState->measurementOnTrack());
      if (!clusterOnTrack)
        continue;
      const FaserSCT_Cluster* cluster = clusterOnTrack->prepRawData();
      Identifier id = cluster->identify();
      m_station = m_idHelper->station(id);
      m_plane = m_idHelper->layer(id);
      m_module = m_idHelper->eta_module(id);
      m_row = m_idHelper->phi_module(id);
      m_sensor = m_idHelper->side(id);
      m_strip = m_idHelper->strip(id);
      Amg::Vector3D position = cluster->globalPosition();
      m_x = position.x();
      m_y = position.y();
      m_z = position.z();
      m_hits->Fill();
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TrackerSegmentFitDataAlg::finalize() {
  return StatusCode::SUCCESS;
}

}  // namespace Tracker
