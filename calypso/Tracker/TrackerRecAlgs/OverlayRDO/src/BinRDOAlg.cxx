#include "BinRDOAlg.h"
#include "TrkTrack/Track.h"

BinRDOAlg::BinRDOAlg(const std::string &name, 
                           ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
{}


StatusCode BinRDOAlg::initialize() 
{
  ATH_CHECK(m_trackCollection.initialize());

  return StatusCode::SUCCESS;
}

StatusCode BinRDOAlg::execute(const EventContext &ctx) const 
{

  setFilterPassed(false, ctx);
  SG::ReadHandle<TrackCollection> trackCollection {m_trackCollection, ctx};
  if (!trackCollection.isValid() || trackCollection->size() == 0) return StatusCode::SUCCESS;

  // track cuts

  double xUpstream {0.0};
  double yUpstream {0.0};

  for (const Trk::Track* track : *trackCollection)
  {
    if (track == nullptr) continue;
    const Trk::TrackParameters* upstreamParameters {nullptr};
    for (auto params : *(track->trackParameters()))
    {
        if (params->position().z() < 0) continue;  // Ignore IFT hits
        if (upstreamParameters == nullptr || params->position().z() < upstreamParameters->position().z()) upstreamParameters = params;
    }
    xUpstream = upstreamParameters->position().x();
    yUpstream = upstreamParameters->position().y();
    break;
  }


  // set filter passed

  setFilterPassed(((xUpstream >= m_xMin) && (xUpstream <= m_xMax) && (yUpstream >= m_yMin) && (yUpstream <= m_yMax)), ctx);
  return StatusCode::SUCCESS;
}


StatusCode BinRDOAlg::finalize() 
{
  return StatusCode::SUCCESS;
}

