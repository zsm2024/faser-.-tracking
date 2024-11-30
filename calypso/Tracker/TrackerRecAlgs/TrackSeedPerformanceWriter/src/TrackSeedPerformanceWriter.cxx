#include "TrackSeedPerformanceWriter.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerIdentifier/FaserSCT_ID.h"


namespace Tracker {

  TrackSeedPerformanceWriter::TrackSeedPerformanceWriter(const std::string &name, ISvcLocator *pSvcLocator)
      : AthReentrantAlgorithm(name, pSvcLocator), AthHistogramming(name),
        m_histSvc("THistSvc/THistSvc", name), m_idHelper(nullptr){}


  StatusCode TrackSeedPerformanceWriter::initialize() {
    ATH_CHECK(m_trackCollectionKey.initialize());
    ATH_CHECK(m_simDataCollectionKey.initialize());
    ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));

    m_tree = new TTree("tree", "tree");
    m_tree->Branch("run", &m_run, "run/I");
    m_tree->Branch("event", &m_event, "event/I");
    m_tree->Branch("station", &m_station, "station/I");
    m_tree->Branch("chi2", &m_chi2, "chi2/D");
    m_tree->Branch("dof", &m_dof, "dof/I");
    m_tree->Branch("nHits", &m_nHits, "nHits/I");
    m_tree->Branch("x", &m_x, "x/D");
    m_tree->Branch("y", &m_y, "y/D");
    m_tree->Branch("z", &m_z, "z/D");
    m_tree->Branch("px", &m_px, "px/D");
    m_tree->Branch("py", &m_py, "py/D");
    m_tree->Branch("pz", &m_pz, "pz/D");
    // m_tree->Branch("barcode", &m_barcode, "barcode/I");
    m_tree->Branch("barcodes", &m_barcodes);
    m_tree->Branch("nMajorityHits", &m_nMajorityHits, "nMajorityHits/I");
    m_tree->Branch("nMajorityParticle", &m_majorityParticle, "nMajorityParticle/I");
    // m_tree->Branch("run", &m_run);
    // m_tree->Branch("event", &m_event);
    // m_tree->Branch("station", &m_station);
    // m_tree->Branch("barcodes", &m_barcodes);
    ATH_CHECK(histSvc()->regTree("/HIST1/TrackSeedPerformance", m_tree));

    return StatusCode::SUCCESS;
  }


  StatusCode TrackSeedPerformanceWriter::execute(const EventContext &ctx) const {
    m_run = ctx.eventID().run_number();
    m_event = ctx.eventID().event_number();

    SG::ReadHandle<TrackCollection> trackCollection{m_trackCollectionKey, ctx};
    ATH_CHECK(trackCollection.isValid());

    SG::ReadHandle<TrackerSimDataCollection> simDataCollection {m_simDataCollectionKey, ctx};
    ATH_CHECK(simDataCollection.isValid());

    for (const Trk::Track *track: *trackCollection) {
      m_chi2 = track->fitQuality()->chiSquared();
      m_dof = track->fitQuality()->numberDoF();
      const Amg::Vector3D trackPosition = track->trackParameters()->front()->position();
      const Amg::Vector3D trackMomentum = track->trackParameters()->front()->momentum();
      m_x = trackPosition.x();
      m_y = trackPosition.y();
      m_z = trackPosition.z();
      m_px = trackMomentum.x();
      m_py = trackMomentum.y();
      m_pz = trackMomentum.z();
      m_nHits = track->measurementsOnTrack()->size();
      m_barcodes = {};
      m_hitCounts = {};
      for (const auto meas: *track->measurementsOnTrack()) {
        const auto *clusterOnTrack = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack *>(meas);
        if (clusterOnTrack) {
          const Tracker::FaserSCT_Cluster *cluster = clusterOnTrack->prepRawData();
          Identifier id = cluster->identify();
          m_barcode = matchHit(id, simDataCollection.get());
          // fill hit map
          if (m_hitCounts.count(m_barcode) > 0) {
            m_hitCounts[m_barcode] += 1;
          } else {
            m_hitCounts[m_barcode] = 1;
          }
          m_barcodes.push_back(m_barcode);
          m_station = m_idHelper->station(id);
        }
      }

      // find majority particle
      m_nMajorityHits = 0;
      m_majorityParticle = 0;
      for (const auto& hit : m_hitCounts) {
        if (hit.second > m_nMajorityHits) {
          m_majorityParticle = hit.first;
          m_nMajorityHits = hit.second;
        }
      }
      m_tree->Fill();
    }

    return StatusCode::SUCCESS;
  }


  StatusCode TrackSeedPerformanceWriter::finalize() {
    return StatusCode::SUCCESS;
  }


  int TrackSeedPerformanceWriter::matchHit(
      Identifier id, const TrackerSimDataCollection *simDataCollection) const {
    int barcode = 0;
    if (simDataCollection->count(id) != 0) {
      const auto& deposits = simDataCollection->find(id)->second.getdeposits();
      float highestDep = 0;
      for (const TrackerSimData::Deposit &deposit : deposits) {
        if (deposit.second > highestDep) {
          highestDep = deposit.second;
#ifdef HEPMC3
          barcode = deposit.first->id();
#else
          barcode = deposit.first->barcode();
#endif          
        }
      }
    }
    return barcode;
  }

}  // namespace Tracker
