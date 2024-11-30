#include "SelectRDOAlg.h"
#include "TrkTrack/Track.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "Identifier/Identifier.h"
#include <cmath>

SelectRDOAlg::SelectRDOAlg(const std::string &name, 
                           ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
{}


StatusCode SelectRDOAlg::initialize() 
{
  ATH_CHECK(m_trackCollection.initialize());
  ATH_CHECK(m_segmentCollection.initialize());
  ATH_CHECK(m_clusterContainer.initialize());
  ATH_CHECK(m_triggerContainer.initialize());
  ATH_CHECK(m_outputTrackCollection.initialize());

  ATH_CHECK(detStore()->retrieve(m_sctHelper, "FaserSCT_ID"));

  return StatusCode::SUCCESS;
}


StatusCode SelectRDOAlg::execute(const EventContext &ctx) const 
{

  setFilterPassed(false, ctx);
  SG::ReadHandle<TrackCollection> trackCollection {m_trackCollection, ctx};
  if (!trackCollection.isValid() || trackCollection->size() == 0) return StatusCode::SUCCESS;

  SG::ReadHandle<TrackCollection> segmentCollection {m_segmentCollection, ctx};
  if (!segmentCollection.isValid()) return StatusCode::SUCCESS;

  SG::ReadHandle<Tracker::FaserSCT_ClusterContainer> clusterContainer { m_clusterContainer, ctx };
  if (!clusterContainer.isValid()) return StatusCode::SUCCESS;

  SG::ReadHandle<xAOD::WaveformHitContainer> triggerContainer { m_triggerContainer, ctx };
  if (!triggerContainer.isValid()) return StatusCode::SUCCESS;

  // segment cuts

  std::vector<int> segmentCount {0, 0, 0, 0};
  int nSegments {0};

  for (const Trk::Track* segment : *segmentCollection)
  {
    if (segment == nullptr) continue;
    nSegments++;
    for (auto measurement : *(segment->measurementsOnTrack()))
    {
        const Tracker::FaserSCT_ClusterOnTrack* cluster = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*>(measurement);
        if (cluster != nullptr)
        {
            Identifier id = cluster->identify();
            int station = m_sctHelper->station(id);
            if (station >= 0 && station <= 3)
            {
              segmentCount[station]++;
              break;
            }
        }
    }
  }
  if ((nSegments > m_maxSegmentsTotal) || (segmentCount[1] > m_maxSegmentsStation) || (segmentCount[2] > m_maxSegmentsStation) || (segmentCount[3] > m_maxSegmentsStation))
    return StatusCode::SUCCESS;

  // track cuts

  int nTracks {0};
  int nLongTracks {0};
  double chi2PerDoF {0};
  double charge {0};
  double maxRadius {0};
  double momentum {0};
  const Trk::Track* theTrack {nullptr};

  for (const Trk::Track* track : *trackCollection)
  {
    if (track == nullptr) continue;
    nTracks++;
    std::set<int> stationMap;
    std::set<std::pair<int, int>> layerMap;

    // Check for hit in the three downstream stations
    for (auto measurement : *(track->measurementsOnTrack()))
    {
        const Tracker::FaserSCT_ClusterOnTrack* cluster = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*>(measurement);
        if (cluster != nullptr)
        {
            Identifier id = cluster->identify();
            int station = m_sctHelper->station(id);
            int layer = m_sctHelper->layer(id);
            stationMap.emplace(station);
            layerMap.emplace(station, layer);
        }
    }
    if (stationMap.count(1) == 0 || stationMap.count(2) == 0 || stationMap.count(3) == 0) continue;

    int nLayers = std::count_if(layerMap.begin(), layerMap.end(), [](std::pair<int,int> p){return p.first != 0;});
    if (nLayers < m_minLayers) continue;
    nLongTracks++;
    const Trk::TrackParameters* upstreamParameters {nullptr};
    for (auto params : *(track->trackParameters()))
    {
        if (params->position().z() < 0) continue;  // Ignore IFT hits
        double radius = sqrt(pow(params->position().x(), 2) + pow(params->position().y(), 2));
        if (radius > maxRadius) maxRadius = radius;
        if (upstreamParameters == nullptr || params->position().z() < upstreamParameters->position().z()) upstreamParameters = params;
    }

    momentum = upstreamParameters->momentum().mag()/1000;
    chi2PerDoF = track->fitQuality()->chiSquared() / track->fitQuality()->numberDoF();
    charge = upstreamParameters->charge();
    theTrack = track;
  }

  if ((nTracks != 1) || (nLongTracks != 1) || (momentum < m_minMomentum) || (momentum > m_maxMomentum) || (chi2PerDoF > m_maxChi2PerDoF) || ((charge>0) != m_acceptPositive) || (maxRadius > m_maxRadius))
    return StatusCode::SUCCESS;

  // cluster cuts

  // std::vector<int> clusterCount {0, 0, 0, 0};
  // for (auto collection : *clusterContainer)
  // {
  //   Identifier id = collection->identify();
  //   int station = m_sctHelper->station(id);
  //   if (station >= 0 && station <= 3) clusterCount[station] += collection->size();
  // }
  // if ((clusterCount[1] > m_maxClustersStation) || (clusterCount[2] > m_maxClustersStation) || (clusterCount[3] > m_maxClustersStation))
  //   return StatusCode::SUCCESS;

  // waveform charge cuts

  std::vector<double> timingCharge {0, 0, 0, 0};
  const int baseChannel = 8;

  for (auto hit : *triggerContainer) {
    if ((hit->hit_status()&2)==0) { // dont store secondary hits as they can overwrite the primary hit
      int ch=hit->channel();
      timingCharge[ch - baseChannel] += hit->integral()/50;
    }
  }
  if ((timingCharge[0] + timingCharge[1] > m_maxTimingCharge) || (timingCharge[2] + timingCharge[3] > m_maxTimingCharge))
    return StatusCode::SUCCESS;

  // passes all cuts

  // copy track without associated clusters
  
  SG::WriteHandle<TrackCollection> outputTrackCollection {m_outputTrackCollection, ctx};
  std::unique_ptr<TrackCollection> outputTracks = std::make_unique<TrackCollection>();

  Trk::TrackInfo i { theTrack->info() };
  auto q = std::make_unique<Trk::FitQuality>(*(theTrack->fitQuality()) );
  // DataVector<const Trk::TrackStateOnSurface>* s = new DataVector<const Trk::TrackStateOnSurface> {};  
  auto s = std::make_unique<Trk::TrackStates>();

  for (auto oldState : (*theTrack->trackStateOnSurfaces()))
  {
    const Trk::CurvilinearParameters* oldParam = dynamic_cast<const Trk::CurvilinearParameters*>(oldState->trackParameters());
    std::unique_ptr<Trk::TrackParameters> newParam { new Trk::CurvilinearParameters { *oldParam } };
    Trk::TrackStateOnSurface* newState = new Trk::TrackStateOnSurface { nullptr, std::move(newParam) };
    s->push_back(newState);
  }
  // std::unique_ptr<DataVector<const Trk::TrackStateOnSurface>> sink {s};
  outputTracks->push_back(new Trk::Track {i, std::move(s) , std::move(q) } );

  ATH_CHECK(outputTrackCollection.record(std::move(outputTracks)));

  // set filter passed

  setFilterPassed(true, ctx);
  return StatusCode::SUCCESS;
}


StatusCode SelectRDOAlg::finalize() 
{
  return StatusCode::SUCCESS;
}

