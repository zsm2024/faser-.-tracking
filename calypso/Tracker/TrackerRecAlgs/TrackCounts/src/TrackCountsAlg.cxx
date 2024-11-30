#include "TrackCountsAlg.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include <TTree.h>
#include <vector>
#include <numeric>

namespace Tracker
{
  using TrackStatesOnSurface = DataVector<const Trk::TrackStateOnSurface>;

  TrackCountsAlg::TrackCountsAlg(const std::string &name, ISvcLocator *pSvcLocator)
      : AthReentrantAlgorithm(name, pSvcLocator), AthHistogramming(name),
        m_idHelper(nullptr), m_histSvc("THistSvc/THistSvc", name)
{
}

  StatusCode TrackCountsAlg::initialize() {
    ATH_MSG_INFO(name() << "::" << __FUNCTION__);
    ATH_CHECK(m_trackCollection.initialize());
    ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));

    m_tree = new TTree("trackCounts", "Track Counts");
    m_tree->Branch("time", &m_time, "time/I");
    m_tree->Branch("station", &m_station, "station/F");
    ATH_CHECK(histSvc()->regTree("/HIST1/trackCounts", m_tree));
    return StatusCode::SUCCESS;
  }

  StatusCode TrackCountsAlg::execute(const EventContext &ctx) const {
    ATH_MSG_INFO(name() << "::" << __FUNCTION__);

    SG::ReadHandle<TrackCollection> trackCollection {m_trackCollection, ctx};
    if (!trackCollection.isValid()){
      ATH_MSG_ERROR("Could not read " << trackCollection.name());
      return StatusCode::FAILURE;
    }

    TrackCollection::const_iterator collItr = trackCollection->begin();
    TrackCollection::const_iterator collItrEnd = trackCollection->end();
    for (; collItr != collItrEnd; ++collItr)
    {
      m_time = ctx.eventID().time_stamp();
      std::vector<int> stations;

      const Trk::Track* track = *collItr;
      TrackStatesOnSurface::const_iterator stateItr = track->trackStateOnSurfaces()->begin();
      TrackStatesOnSurface::const_iterator stateItrEnd = track->trackStateOnSurfaces()->end();
      for (; stateItr !=stateItrEnd; ++stateItr)
      {
        const Trk::TrackStateOnSurface* trackState = *stateItr;
        const auto* clusterOnTrack = dynamic_cast<const FaserSCT_ClusterOnTrack*>(trackState->measurementOnTrack());
        if (clusterOnTrack)
        {
          const FaserSCT_Cluster* cluster = clusterOnTrack->prepRawData();
          stations.push_back(m_idHelper->station(cluster->identify()));
        }
      }
      m_station = accumulate(stations.begin(), stations.end(), 0.0) / stations.size();
      m_tree->Fill();
    }

    return StatusCode::SUCCESS;
  }

  StatusCode TrackCountsAlg::finalize() {
    ATH_MSG_INFO(name() << "::" << __FUNCTION__);
    return StatusCode::SUCCESS;
  }

} // Tracker
