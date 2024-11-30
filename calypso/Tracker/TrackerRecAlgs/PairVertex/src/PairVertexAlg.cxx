#include "PairVertexAlg.h"

#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "Identifier/Identifier.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "ScintIdentifier/VetoNuID.h"
#include "ScintIdentifier/VetoID.h"
#include "ScintIdentifier/TriggerID.h"
#include "ScintIdentifier/PreshowerID.h"
#include "FaserCaloIdentifier/EcalID.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include <cmath>
#include <numeric>

#include <TTree.h>
#include <TBranch.h>

#include <typeinfo>

constexpr float NaN = std::numeric_limits<double>::quiet_NaN();

namespace Tracker {


PairVertexAlg::PairVertexAlg(const std::string &name, ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator), AthHistogramming(name), m_idHelper(nullptr), m_histSvc("THistSvc/THistSvc", name) {}

void PairVertexAlg::addBranch(const std::string &name,
				float* var) {
  m_tree->Branch(name.c_str(),var,(name+"/F").c_str());
}
void PairVertexAlg::addBranch(const std::string &name,
				unsigned int* var) {
  m_tree->Branch(name.c_str(),var,(name+"/I").c_str());
}
void PairVertexAlg::addWaveBranches(const std::string &name,
				      int nchannels,
				      int first) {
  for(int ch=0;ch<nchannels;ch++) {
    std::string base=name+std::to_string(ch)+"_";
    addBranch(base+"time",&m_wave_localtime[first]);
    addBranch(base+"peak",&m_wave_peak[first]);
    addBranch(base+"width",&m_wave_width[first]);
    addBranch(base+"integral",&m_wave_integral[first]);
    addBranch(base+"raw_peak",&m_wave_raw_peak[first]);
    addBranch(base+"raw_charge",&m_wave_raw_integral[first]);
    addBranch(base+"baseline",&m_wave_baseline_mean[first]);
    addBranch(base+"baseline_rms",&m_wave_baseline_rms[first]);
    addBranch(base+"status",&m_wave_status[first]);
    first++;
  }
}

void PairVertexAlg::FillWaveBranches(const xAOD::WaveformHitContainer &wave) const {
  for (auto hit : wave) {
    if (waveformHitOK(hit) && (hit->hit_status()&2)==0) { // dont store secoondary hits as they can overwrite the primary hit
      int ch=hit->channel();
      //m_wave_localtime[ch]=hit->localtime()+m_clock_phase;
      m_wave_peak[ch]=hit->peak();
      m_wave_width[ch]=hit->width();
      m_wave_integral[ch]=hit->integral();

      m_wave_raw_peak[ch]=hit->raw_peak();
      m_wave_raw_integral[ch]=hit->raw_integral();
      m_wave_baseline_mean[ch]=hit->baseline_mean();
      m_wave_baseline_rms[ch]=hit->baseline_rms();
      m_wave_status[ch]=hit->hit_status();  
    }
  }
}

bool PairVertexAlg::waveformHitOK(const xAOD::WaveformHit* hit) const
{
    if (hit->status_bit(xAOD::WaveformStatus::THRESHOLD_FAILED) || hit->status_bit(xAOD::WaveformStatus::SECONDARY)) return false;
    return true;
}


StatusCode PairVertexAlg::initialize() 
{
  ATH_CHECK(m_mcEventCollectionKey.initialize());
  ATH_CHECK(m_trackCollectionKey.initialize());
  ATH_CHECK(m_vetoNuContainer.initialize());
  ATH_CHECK(m_vetoContainer.initialize());
  ATH_CHECK(m_triggerContainer.initialize());
  ATH_CHECK(m_preshowerContainer.initialize());
  ATH_CHECK(m_ecalContainer.initialize());

  ATH_CHECK(m_fiducialParticleTool.retrieve());
  ATH_CHECK(m_trackTruthMatchingTool.retrieve());
  ATH_CHECK(m_truthParticleContainer.initialize());

  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_detMgr, "SCT"));
  ATH_CHECK(detStore()->retrieve(m_vetoNuHelper,    "VetoNuID"));
  ATH_CHECK(detStore()->retrieve(m_vetoHelper,      "VetoID"));
  ATH_CHECK(detStore()->retrieve(m_triggerHelper,   "TriggerID"));
  ATH_CHECK(detStore()->retrieve(m_preshowerHelper, "PreshowerID"));
  ATH_CHECK(detStore()->retrieve(m_ecalHelper,      "EcalID"));

  m_tree = new TTree("events", "Events");

  m_tree->Branch("TrackCount", &m_trackCount, "trackCount/I");

  m_tree->Branch("tracksX", &m_tracksX);
  m_tree->Branch("tracksY", &m_tracksY);
  m_tree->Branch("tracksZ", &m_tracksZ);
  m_tree->Branch("tracksPx", &m_tracksPx);
  m_tree->Branch("tracksPy", &m_tracksPy);
  m_tree->Branch("tracksPz", &m_tracksPz);
  m_tree->Branch("tracksPmag", &m_tracksPmag);

  m_tree->Branch("tracksCharge", &m_tracksCharge);
  m_tree->Branch("tracksSigmaQovP", &m_sigmaQoverP);
  m_tree->Branch("tracksChi2", &m_tracksChi2);
  m_tree->Branch("tracksDoF", &m_tracksDoF);
  m_tree->Branch("trackPositiveIndicies", &m_trackPositiveIndicies);
  m_tree->Branch("trackNegativeIndicies", &m_trackNegativeIndicies);

  m_tree->Branch("tracksTruthBarcode", &m_tracksTruthBarcode);
  m_tree->Branch("tracksTruthPdg", &m_tracksTruthPdg);
  m_tree->Branch("tracksTruthCharge", &m_tracksTruthCharge);
  m_tree->Branch("tracksTruthPmag", &m_tracksTruthPmag);
  m_tree->Branch("tracksIsFiducial", &m_tracksIsFiducial);

  m_tree->Branch("trackD2i", &m_trackD2i); //Index of track with daughter particle Barcode 2
  m_tree->Branch("trackD3i", &m_trackD3i); //Index of track with daughter particle Barcode 3

  m_tree->Branch("tracksNLayersHit", & m_tracksnLayersHit);
  m_tree->Branch("tracksLayersIFT", & m_tracksLayersIFT);
  m_tree->Branch("tracksIFTHit", & m_tracksIFThit);

  m_tree->Branch("tracksLayerMap", &m_layerMap);


  //WAVEFORM
  addWaveBranches("VetoSt1",2,6);
  addWaveBranches("VetoSt2",1,14);
  addWaveBranches("VetoNu",2,4);
  addWaveBranches("Timing",4,8);
  addWaveBranches("Preshower",2,12);
  addWaveBranches("Calo",4,0);

  //TRUTH
  m_tree->Branch("tTrkBarcode", &m_truthBarcode); 
  m_tree->Branch("tTrkPdg", &m_truthPdg); 

  m_tree->Branch("vertexPos", &m_vertexPos); // [x,y,z]
  m_tree->Branch("isFiducial", &m_isFiducial);
  
  m_tree->Branch("truthM_P", &m_truthM_P);
  m_tree->Branch("truthM_px", &m_truthM_px);
  m_tree->Branch("truthM_py", &m_truthM_py);
  m_tree->Branch("truthM_pz", &m_truthM_pz);
  m_tree->Branch("truthM_x", &m_truthM_x);
  m_tree->Branch("truthM_y", &m_truthM_y);
  m_tree->Branch("truthM_z", &m_truthM_z);

  //Electron
  m_tree->Branch("truthd0_P", &m_truthd0_P);
  m_tree->Branch("truthd0_px", &m_truthd0_px);
  m_tree->Branch("truthd0_py", &m_truthd0_py);
  m_tree->Branch("truthd0_pz", &m_truthd0_pz);
  m_tree->Branch("truthd0_x", &m_truthd0_x);
  m_tree->Branch("truthd0_y", &m_truthd0_y);
  m_tree->Branch("truthd0_z", &m_truthd0_z);
  m_tree->Branch("truthd0_IsFiducial", &m_truthd0_IsFiducial);
  m_tree->Branch("truthd0_charge", &m_truthd0_charge);

  //Positron
  m_tree->Branch("truthd1_P", &m_truthd1_P);
  m_tree->Branch("truthd1_px", &m_truthd1_px);
  m_tree->Branch("truthd1_py", &m_truthd1_py);
  m_tree->Branch("truthd1_pz", &m_truthd1_pz);
  m_tree->Branch("truthd1_x", &m_truthd1_x);
  m_tree->Branch("truthd1_y", &m_truthd1_y);
  m_tree->Branch("truthd1_z", &m_truthd1_z);
  m_tree->Branch("truthd1_IsFiducial", &m_truthd1_IsFiducial);
  m_tree->Branch("truthd1_charge", &m_truthd1_charge);




  ATH_CHECK(histSvc()->regTree("/HIST/events", m_tree));
  return StatusCode::SUCCESS;
}

StatusCode PairVertexAlg::execute(const EventContext &ctx) const 
{
    clearTree();
    m_numberOfEvents++;
    //WAVEFORM & CALO
    SG::ReadHandle<xAOD::WaveformHitContainer> vetoNuContainer { m_vetoNuContainer, ctx };
    ATH_CHECK(vetoNuContainer.isValid());
    SG::ReadHandle<xAOD::WaveformHitContainer> vetoContainer { m_vetoContainer, ctx };
    ATH_CHECK(vetoContainer.isValid());
    SG::ReadHandle<xAOD::WaveformHitContainer> triggerContainer { m_triggerContainer, ctx };
    ATH_CHECK(triggerContainer.isValid());
    SG::ReadHandle<xAOD::WaveformHitContainer> preshowerContainer { m_preshowerContainer, ctx };
    ATH_CHECK(preshowerContainer.isValid());
    SG::ReadHandle<xAOD::WaveformHitContainer> ecalContainer { m_ecalContainer, ctx };
    ATH_CHECK(ecalContainer.isValid());

    FillWaveBranches(*vetoContainer);
    FillWaveBranches(*triggerContainer);
    FillWaveBranches(*preshowerContainer);
    FillWaveBranches(*ecalContainer);
    FillWaveBranches(*vetoNuContainer);    

    //FILL WITH TRUTHPARTICLECONTAINER
    SG::ReadHandle<xAOD::TruthParticleContainer> truthParticleContainer { m_truthParticleContainer, ctx };
    if (truthParticleContainer.isValid() && truthParticleContainer->size() > 0)
    {
      for (auto particle : *truthParticleContainer)
      {
        if ( particle->barcode() <= 3) 
        {

          if ( particle->barcode() == 1) // mother particle (A')
            {

            m_truthM_P.push_back(particle->p4().P());
            m_truthM_px.push_back(particle->p4().X());
            m_truthM_py.push_back(particle->p4().Y());
            m_truthM_pz.push_back(particle->p4().Z());
            if (m_fiducialParticleTool->isFiducial(particle->barcode())) {ATH_MSG_ERROR("DP has hits! Something is wrong");}
            if ( particle->hasProdVtx()) {
              m_truthM_x.push_back(particle->prodVtx()->x());
              m_truthM_y.push_back(particle->prodVtx()->y());
              m_truthM_z.push_back(particle->prodVtx()->z());
            } else {
              m_truthM_x.push_back(NaN);
              m_truthM_y.push_back(NaN);
              m_truthM_z.push_back(NaN);
            }}

          if ( particle->charge() < 0) // negative daughter particle
            {
              m_truthd0_P.push_back(particle->p4().P());
              m_truthd0_px.push_back(particle->p4().X());
              m_truthd0_py.push_back(particle->p4().Y());
              m_truthd0_pz.push_back(particle->p4().Z());
              m_truthd0_charge.push_back(particle->charge());
              m_truthd0_IsFiducial.push_back(m_fiducialParticleTool->isFiducial(particle->barcode()));

            if ( particle->hasProdVtx()) {
              m_truthd0_x.push_back(particle->prodVtx()->x());
              m_truthd0_y.push_back(particle->prodVtx()->y());
              m_truthd0_z.push_back(particle->prodVtx()->z());
            } else {
              m_truthd0_x.push_back(NaN);
              m_truthd0_y.push_back(NaN);
              m_truthd0_z.push_back(NaN);
            }}

          if ( particle->charge() > 0) // positive daughter particle
            {
              m_truthd1_P.push_back(particle->p4().P());
              m_truthd1_px.push_back(particle->p4().X());
              m_truthd1_py.push_back(particle->p4().Y());
              m_truthd1_pz.push_back(particle->p4().Z());
              m_truthd1_charge.push_back(particle->charge());
              m_truthd1_IsFiducial.push_back(m_fiducialParticleTool->isFiducial(particle->barcode()));

            if ( particle->hasProdVtx()) {
              m_truthd1_x.push_back(particle->prodVtx()->x());
              m_truthd1_y.push_back(particle->prodVtx()->y());
              m_truthd1_z.push_back(particle->prodVtx()->z());
            } else {
              m_truthd1_x.push_back(NaN);
              m_truthd1_y.push_back(NaN);
              m_truthd1_z.push_back(NaN);
              }}
                }}}

    //FILL WITH TRACKCOLLECTION
    SG::ReadHandle<TrackCollection> tracks { m_trackCollectionKey, ctx };
    ATH_CHECK(tracks.isValid());
    for (auto trk : *tracks)
    {
        const Trk::TrackParameters* upstream {nullptr};
        if (trk == nullptr)
        {
            ATH_MSG_WARNING("Null pointer from TrackContainer.");
            m_invalidTrackContainerEntries++;
            continue;
        }
        auto parameters = trk->trackParameters();

        //Error Catching
        if (parameters == nullptr || parameters->empty()) 
        {
            m_numberOfNoParameterTracks++;
            ATH_MSG_WARNING("Track without parameters found.");
            return StatusCode::SUCCESS;
        }
        for (auto state : *parameters)
        {
            if (state == nullptr)
            {
                m_numberOfNullParameters++;
                ATH_MSG_WARNING("Null track parameters returned.");
                continue;
            }
            if (!state->hasSurface())
            {
                m_numberOfParametersWithoutSurface++;
                ATH_MSG_WARNING("Track state without surface found.");
                continue;
            }
            if (!upstream || upstream->associatedSurface().center().z() > state->associatedSurface().center().z())
                upstream = state;
        }
        if (!upstream)
        {
            m_numberOfNoUpstreamTracks++;
            ATH_MSG_WARNING("Unable to find track parameters on any surface");
            continue;
        }//End error catching; assign state to upstream if everything else is ok.

        
        //Truth info
        auto [truthParticle, hitCount] = m_trackTruthMatchingTool->getTruthParticle(trk);
        if (truthParticle != nullptr) {
          m_tracksTruthPdg.push_back(truthParticle->pdgId());
          m_tracksTruthBarcode.push_back(truthParticle->barcode());
          m_tracksIsFiducial.push_back(m_fiducialParticleTool->isFiducial(truthParticle->barcode()));
          m_tracksTruthCharge.push_back(truthParticle->charge());
          m_tracksTruthPmag.push_back(truthParticle->p4().P());
          if (truthParticle->barcode()==2) m_trackD2i=m_trackCount;
          if (truthParticle->barcode()==3) m_trackD3i=m_trackCount;
          }
        else {
          m_tracksTruthPdg.push_back(NaN);
          m_tracksTruthBarcode.push_back(NaN);
          m_tracksIsFiducial.push_back(NaN);
          m_tracksTruthCharge.push_back(NaN);
          m_tracksTruthPmag.push_back(NaN);
        }

        //LAYERMAPPING
        std::vector<std::vector<int>> layerMap={{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
        std::set<int> stationMap;

        for (auto measurement : *(trk->measurementsOnTrack())) {
            const Tracker::FaserSCT_ClusterOnTrack* cluster = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*>(measurement);
            if (cluster != nullptr) {
                Identifier id = cluster->identify();
                int station = m_idHelper->station(id);
                int layer = m_idHelper->layer(id);
                stationMap.emplace(station);
                layerMap[station][layer]+=1;
            }}
        int layersHit=0;
        int iftHit=0;
        for (int st=0;st<4;st++) {
            for (int lr=0;lr<3;lr++){
                if (layerMap[st][lr]>0) layersHit++; 
                if (st==0 && layerMap[st][lr]>0) iftHit++;
            }}
        if (stationMap.count(1) == 0 || stationMap.count(2) == 0 || stationMap.count(3) == 0) continue;

        m_tracksnLayersHit.push_back(layersHit);
        m_tracksIFThit.push_back(iftHit);

        //TRACKPARAMETERS
        const Trk::TrackParameters* upstreamParameters = trk->trackParameters()->front();
        const Trk::TrackParameters* downstreamParameters = trk->trackParameters()->back();

        auto covariance = *upstreamParameters->covariance();
        double sigmaQoverP = sqrt(covariance(4,4));
        m_sigmaQoverP.push_back(sigmaQoverP);

        auto momentum = upstreamParameters->momentum();
        double charge = upstreamParameters->charge();
        auto position = upstreamParameters->position();
        
        m_tracksCharge.push_back(charge);
        if (charge>0){ m_trackPositiveIndicies.push_back(m_trackCount);}
        if (charge>0) {m_trackNegativeIndicies.push_back(m_trackCount);}
        m_tracksChi2.push_back(trk->fitQuality()->chiSquared());
        m_tracksDoF.push_back(trk->fitQuality()->numberDoF());

        m_tracksX.push_back(position.x());
        m_tracksY.push_back(position.y());
        m_tracksZ.push_back(position.z());

        m_tracksPx.push_back(momentum.x());
        m_tracksPy.push_back(momentum.y());
        m_tracksPz.push_back(momentum.z());
        m_tracksPmag.push_back(momentum.mag());

        ++m_trackCount;
        
    }


    // MC TRUTH
    SG::ReadHandle<McEventCollection> mcEvents {m_mcEventCollectionKey, ctx};
    ATH_CHECK(mcEvents.isValid());
    if (mcEvents->size() != 1) 
    {
        ATH_MSG_ERROR("There should be exactly one event in the McEventCollection.");
        return StatusCode::FAILURE;
    }
    const HepMC::GenEvent* theEvent = mcEvents->at(0);
    auto pVertices    = theEvent->vertices_begin();
    auto pVerticesEnd = theEvent->vertices_end();

    for (; pVertices != pVerticesEnd; ++pVertices)
    {
        const HepMC::GenVertex* v = *pVertices;
        m_vertexPos[0]=v->position().x();
        m_vertexPos[1]=v->position().y();
        m_vertexPos[2]=v->position().z();
        break;
    }

    m_tree->Fill();

    return StatusCode::SUCCESS;
}

StatusCode PairVertexAlg::finalize() 
{
    ATH_MSG_INFO("Found " << m_numberOfOppositeSignPairs << " opposite sign pairs in " << m_numberOfEvents << " total events.");
    ATH_MSG_INFO(m_numberOfPositiveTracks << " events had a positive track, and " << m_numberOfNegativeTracks << " had a negative track.");
    ATH_MSG_INFO(m_numberOfNoUpstreamTracks << " tracks could not locate an upstream position.");
    ATH_MSG_INFO(m_numberOfNoParameterTracks << " tracks in track pairs had no track parameters.");
    ATH_MSG_INFO(m_numberOfNullParameters << " track parameters were null and " << m_numberOfParametersWithoutSurface << " had no associated surface.");
    ATH_MSG_INFO(m_invalidTrackContainerEntries << " invalid TrackContainer entries found.");
    return StatusCode::SUCCESS;
}

void PairVertexAlg::clearTree() const
{
    m_trackCount=0;
    m_tracksPmag.clear();
    m_sigmaQoverP.clear();
        
    m_tracksX.clear();
    m_tracksY.clear();
    m_tracksZ.clear();
    m_tracksPx.clear();
    m_tracksPy.clear();
    m_tracksPz.clear();
    m_tracksPmag.clear();

    m_tracksCharge.clear();
    m_sigmaQoverP.clear();
    m_tracksChi2.clear();
    m_tracksDoF.clear();

    m_trackPositiveIndicies.clear();
    m_trackNegativeIndicies.clear();
    m_trackD2i=-1;
    m_trackD3i=-1;

    m_tracksTruthBarcode.clear();
    m_tracksTruthPdg.clear();
    m_tracksTruthCharge.clear();
    m_tracksTruthPmag.clear();
    m_tracksIsFiducial.clear();

    m_tracksnLayersHit.clear();
    m_tracksLayersIFT.clear();
    m_tracksIFThit.clear();



    for(int ii=0;ii<15;ii++) {
      m_wave_localtime[ii]=0;
      m_wave_peak[ii]=0;
      m_wave_width[ii]=0;
      m_wave_integral[ii]=0;

      m_wave_raw_peak[ii]=0;
      m_wave_raw_integral[ii]=0;
      m_wave_baseline_mean[ii]=0;
      m_wave_baseline_rms[ii]=0;
      m_wave_status[ii]=0;
    }
    m_vertexPos= {-1,-1,-1};
    m_isFiducial.clear();
    m_truthPdg.clear();
    m_truthBarcode.clear();

    m_truthd0_P.clear();
    m_truthd0_px.clear();
    m_truthd0_py.clear();
    m_truthd0_pz.clear();
    m_truthd0_IsFiducial.clear();
    m_truthd0_charge.clear();

    m_truthd1_P.clear();
    m_truthd1_px.clear();
    m_truthd1_py.clear();
    m_truthd1_pz.clear();
    m_truthd1_IsFiducial.clear();
    m_truthd1_charge.clear();

}

} // end Tracker



