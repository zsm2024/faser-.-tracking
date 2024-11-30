#include "NeutrinoSearchAlg.h"
#include "TrkTrack/Track.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "ScintIdentifier/VetoNuID.h"
#include "ScintIdentifier/VetoID.h"
#include "ScintIdentifier/TriggerID.h"
#include "ScintIdentifier/PreshowerID.h"
#include "FaserCaloIdentifier/EcalID.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "Identifier/Identifier.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "xAODTruth/TruthParticle.h"
#include <cmath>



NeutrinoSearchAlg::NeutrinoSearchAlg(const std::string &name, 
                                    ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator), 
      AthHistogramming(name),
      m_histSvc("THistSvc/THistSvc", name) {}


StatusCode NeutrinoSearchAlg::initialize() 
{
  ATH_CHECK(m_truthEventContainer.initialize());
  ATH_CHECK(m_truthParticleContainer.initialize());
  ATH_CHECK( m_vetoNuHitKey.initialize() );

  ATH_CHECK(m_trackCollection.initialize());
  ATH_CHECK(m_vetoNuContainer.initialize());
  ATH_CHECK(m_vetoContainer.initialize());
  ATH_CHECK(m_triggerContainer.initialize());
  ATH_CHECK(m_preshowerContainer.initialize());
  ATH_CHECK(m_ecalContainer.initialize());
  ATH_CHECK(m_clusterContainer.initialize());
  ATH_CHECK(m_simDataCollection.initialize());

  ATH_CHECK(detStore()->retrieve(m_sctHelper,       "FaserSCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_vetoNuHelper,    "VetoNuID"));
  ATH_CHECK(detStore()->retrieve(m_vetoHelper,      "VetoID"));
  ATH_CHECK(detStore()->retrieve(m_triggerHelper,   "TriggerID"));
  ATH_CHECK(detStore()->retrieve(m_preshowerHelper, "PreshowerID"));
  ATH_CHECK(detStore()->retrieve(m_ecalHelper,      "EcalID"));

  ATH_CHECK(detStore()->retrieve(m_detMgr, "SCT"));
  ATH_CHECK(m_extrapolationTool.retrieve());
  ATH_CHECK(m_trackingGeometryTool.retrieve());

  if (m_useFlukaWeights)
  {
    m_baseEventCrossSection = (m_flukaCrossSection * kfemtoBarnsPerMilliBarn)/m_flukaCollisions;
  }
  else if (m_useGenieWeights)
  {
    m_baseEventCrossSection = 1.0/m_genieLuminosity;
  }
  else
  {
    m_baseEventCrossSection = 1.0;
  }

  m_tree = new TTree("tree", "tree");
  m_tree->Branch("run_number", &m_run_number, "run_number/I");
  m_tree->Branch("event_number", &m_event_number, "event_number/I");

//   m_tree->Branch("VetoNuTruthX", &m_vetoNuHitsMeanX, "vetoNuTruthX/D");
//   m_tree->Branch("VetoNuTruthY", &m_vetoNuHitsMeanY, "vetoNuTruthY/D");

  m_tree->Branch("VetoNuPmt0", &m_vetoNu0, "vetoNu0/D");
  m_tree->Branch("VetoNuPmt1", &m_vetoNu1, "vetoNu1/D");

  m_tree->Branch("VetoPmt00",  &m_veto00,  "veto00/D");
  m_tree->Branch("VetoPmt01",  &m_veto01,  "veto01/D");
  m_tree->Branch("VetoUpstream", &m_vetoUpstream, "vetoUpstream/D");
  m_tree->Branch("VetoPmt10",  &m_veto10,  "veto10/D");
  m_tree->Branch("VetoPmt11",  &m_veto11,  "veto11/D");
  m_tree->Branch("VetoDownstream", &m_vetoDownstream, "vetoDownstream/D");

  m_tree->Branch("TriggerPmt00", &m_trigger00, "trigger00/D");
  m_tree->Branch("TriggerPmt01", &m_trigger01, "trigger01/D");
  m_tree->Branch("TriggerPmt10", &m_trigger10, "trigger10/D");
  m_tree->Branch("TriggerPmt11", &m_trigger11, "trigger11/D");
  m_tree->Branch("TriggerTotal", &m_triggerTotal, "triggerTotal/D");

  m_tree->Branch("PreshowerPmt0", &m_preshower0, "preshower0/D");
  m_tree->Branch("PreshowerPmt1", &m_preshower1, "preshower1/D");

  m_tree->Branch("EcalPmt00", &m_ecal00, "ecal00/D");
  m_tree->Branch("EcalPmt01", &m_ecal01, "ecal01/D");
  m_tree->Branch("EcalPmt10", &m_ecal10, "ecal10/D");
  m_tree->Branch("EcalPmt11", &m_ecal11, "ecal11/D");
  m_tree->Branch("EcalTotal", &m_ecalTotal, "ecalTotal/D");
  
  m_tree->Branch("Clust0", &m_station0Clusters, "clust0/I");
  m_tree->Branch("Clust1", &m_station1Clusters, "clust0/I");
  m_tree->Branch("Clust2", &m_station2Clusters, "clust0/I");
  m_tree->Branch("Clust3", &m_station3Clusters, "clust0/I");

  m_tree->Branch("x", &m_x, "x/D");
  m_tree->Branch("y", &m_y, "y/D");
  m_tree->Branch("z", &m_z, "z/D");
  m_tree->Branch("px", &m_px, "px/D");
  m_tree->Branch("py", &m_py, "py/D");
  m_tree->Branch("pz", &m_pz, "pz/D");
  m_tree->Branch("p", &m_p, "p/D");
  m_tree->Branch("charge", &m_charge, "charge/I");
  m_tree->Branch("chi2", &m_chi2, "chi2/D");
  m_tree->Branch("ndof", &m_ndof, "ndof/I");
  m_tree->Branch("longTracks", &m_longTracks, "longTracks/I");
  m_tree->Branch("pTruthLepton", &m_truthLeptonMomentum, "pTruthLepton/D");
  m_tree->Branch("xTruthLepton", &m_truthLeptonX, "xTruthLepton/D");
  m_tree->Branch("yTruthLepton", &m_truthLeptonY, "yTruthLepton/D");
  m_tree->Branch("truthBarcode", &m_truthBarcode, "truthBarcode/I");
  m_tree->Branch("truthPdg", &m_truthPdg, "truthPdg/I");
  m_tree->Branch("CrossSection", &m_crossSection, "crossSection/D");
  m_tree->Branch("xVetoNu", &m_xVetoNu, "xVetoNu/D");
  m_tree->Branch("yVetoNu", &m_yVetoNu, "yVetoNu/D");
  m_tree->Branch("sxVetoNu", &m_sxVetoNu, "sxVetoNu/D");
  m_tree->Branch("syVetoNu", &m_syVetoNu, "syVetoNu/D");
  m_tree->Branch("thetaxVetoNu", &m_thetaxVetoNu, "thetaxVetoNu/D");
  m_tree->Branch("thetayVetoNu", &m_thetayVetoNu, "thetayVetoNu/D");

  ATH_CHECK(histSvc()->regTree("/HIST2/tree", m_tree));

  if (m_enforceBlinding)
  {
    ATH_MSG_INFO("Blinding will be enforced for real data.");
  }
  else
  {
    ATH_MSG_INFO("Blinding will NOT be enforced for real data.");
  }

  return StatusCode::SUCCESS;
}


StatusCode NeutrinoSearchAlg::execute(const EventContext &ctx) const 
{

  clearTree();

  m_run_number = ctx.eventID().run_number();
  m_event_number = ctx.eventID().event_number();

  bool realData = true;

  // Work out effective cross section for MC
  SG::ReadHandle<xAOD::TruthEventContainer> truthEventContainer { m_truthEventContainer, ctx };
  if (truthEventContainer.isValid() && truthEventContainer->size() > 0)
  {
    realData = false;
    if (m_useFlukaWeights)
    {
        double flukaWeight = truthEventContainer->at(0)->weights()[0];
        ATH_MSG_ALWAYS("Found fluka weight = " << flukaWeight);
        m_crossSection = m_baseEventCrossSection * flukaWeight;

        // // Find crossing position of VetoNu so we can check extrapolation
        // SG::ReadHandle<ScintHitCollection> h_vetoNuHits {m_vetoNuHitKey, ctx};
        // if (h_vetoNuHits.isValid() && h_vetoNuHits->size()!=0){
        //     double totalEnergyLoss {0.0};
        //     for (const ScintHit& hit : *h_vetoNuHits)
        //     {
        //         m_vetoNuHitsMeanX += hit.energyLoss() * (hit.localStartPosition().x() + hit.localEndPosition().x()) / 2;
        //         m_vetoNuHitsMeanY += hit.energyLoss() * (hit.localStartPosition().y() + hit.localEndPosition().y()) / 2;
        //         totalEnergyLoss += hit.energyLoss();
        //     }
        //     if (totalEnergyLoss > 0)
        //     {
        //         m_vetoNuHitsMeanX /= totalEnergyLoss;
        //         m_vetoNuHitsMeanY /= totalEnergyLoss;                
        //     }
        // }
    }
    else if (m_useGenieWeights)
    {
        m_crossSection = m_baseEventCrossSection;
    }
    else
    {
        ATH_MSG_WARNING("Monte carlo event with no weighting scheme specified.  Setting crossSection (weight) to " << m_baseEventCrossSection << " fb.");
        m_crossSection = m_baseEventCrossSection;
    }
  }

  // Find the primary lepton (if any)
  SG::ReadHandle<xAOD::TruthParticleContainer> truthParticleContainer { m_truthParticleContainer, ctx };
  if (truthParticleContainer.isValid() && truthParticleContainer->size() > 0)
  {
    for (auto particle : *truthParticleContainer)
    {
        if ( particle->absPdgId() == 11 || particle->absPdgId() == 13 || particle->absPdgId() == 15 )
        {
            if (particle->status() == 1 && (particle->nParents() == 0 || particle->nParents() == 2) )
            {
                m_truthLeptonMomentum = particle->p4().P();
                double deltaZ = m_zExtrapolate - particle->prodVtx()->z();
                double thetaX = particle->px()/particle->pz();
                double thetaY = particle->py()/particle->pz();
                m_truthLeptonX = particle->prodVtx()->x() + thetaX * deltaZ;
                m_truthLeptonY = particle->prodVtx()->y() + thetaY * deltaZ;
            }
            break;
        }
    }
  }

  SG::ReadHandle<xAOD::WaveformHitContainer> vetoNuContainer { m_vetoNuContainer, ctx };
  ATH_CHECK(vetoNuContainer.isValid());

  // If real data, check for blinding before we do anything else

  bool blinded = realData;
  for (auto hit : *vetoNuContainer)
  {
    if (!waveformHitOK(hit)) continue;
    blinded = false;
    auto id = hit->identify();
    if (m_vetoNuHelper->plate(id) == 0)
    {
        m_vetoNu0 += hit->integral();
    }
    else if (m_vetoNuHelper->plate(id) == 1)
    {
        m_vetoNu1 += hit->integral();
    }
    else
    {
        ATH_MSG_FATAL("Invalid VetoNu plate number: " << m_vetoNuHelper->plate(id));
        return StatusCode::FAILURE;
    }
  }

  if (m_enforceBlinding && blinded) return StatusCode::SUCCESS;

  SG::ReadHandle<xAOD::WaveformHitContainer> vetoContainer { m_vetoContainer, ctx };
  ATH_CHECK(vetoContainer.isValid());

  SG::ReadHandle<xAOD::WaveformHitContainer> triggerContainer { m_triggerContainer, ctx };
  ATH_CHECK(triggerContainer.isValid());

  SG::ReadHandle<xAOD::WaveformHitContainer> preshowerContainer { m_preshowerContainer, ctx };
  ATH_CHECK(preshowerContainer.isValid());

  SG::ReadHandle<xAOD::WaveformHitContainer> ecalContainer { m_ecalContainer, ctx };
  ATH_CHECK(ecalContainer.isValid());

  for (auto hit : *vetoContainer)
  {
    if (!waveformHitOK(hit)) continue;
    auto id = hit->identify();
    auto station = m_vetoHelper->station(id);
    auto plate   = m_vetoHelper->plate(id);
    if (station == 0)
    {
        if (plate == 0)
        {
            m_veto00 += hit->integral();
            m_vetoUpstream += hit->integral();
        }
        else if (plate == 1)
        {
            m_veto01 += hit->integral();
            m_vetoUpstream += hit->integral();
        }
        else
        {
            ATH_MSG_FATAL("Invalid Veto plate number: " << plate);
        }
    }
    else if (station == 1)
    {
        if (plate == 0)
        {
            m_veto10 += hit->integral();
            m_vetoDownstream += hit->integral();
        }
        else if (plate == 1)
        {
            m_veto11 += hit->integral();
            m_vetoDownstream += hit->integral();
        }
        else
        {
            ATH_MSG_FATAL("Invalid Veto plate number: " << plate);
        }
    }
    else
    {
        ATH_MSG_FATAL("Invalid Veto station number: " << station);
        return StatusCode::FAILURE;
    }
  }

  for (auto hit : *triggerContainer)
  {
    if (!waveformHitOK(hit)) continue;
    auto id = hit->identify();
    auto plate = m_triggerHelper->plate(id);
    auto pmt   = m_triggerHelper->pmt(id);
    if (plate == 0)
    {
        if (pmt == 0)
        {
            m_trigger00 += hit->integral();
            m_triggerTotal += hit->integral();
        }
        else if (pmt == 1)
        {
            m_trigger01 += hit->integral();
            m_triggerTotal += hit->integral();
        }
        else
        {
            ATH_MSG_FATAL("Invalid Trigger pmt number: " << pmt);
        }
    }
    else if (plate == 1)
    {
        if (pmt == 0)
        {
            m_trigger10 += hit->integral();
            m_triggerTotal += hit->integral();
        }
        else if (pmt == 1)
        {
            m_trigger11 += hit->integral();
            m_triggerTotal += hit->integral();
        }
        else
        {
            ATH_MSG_FATAL("Invalid Trigger pmt number: " << pmt);
        }
    }
    else
    {
        ATH_MSG_FATAL("Invalid Trigger plate number: " << plate);
        return StatusCode::FAILURE;
    }
  }

  for (auto hit : *preshowerContainer)
  {
    if (!waveformHitOK(hit)) continue;
    auto id = hit->identify();
    if (m_preshowerHelper->plate(id) == 0)
    {
        m_preshower0 += hit->integral();
    }
    else if (m_preshowerHelper->plate(id) == 1)
    {
        m_preshower1 += hit->integral();
    }
    else
    {
        ATH_MSG_FATAL("Invalid Preshower plate number: " << m_preshowerHelper->plate(id));
        return StatusCode::FAILURE;
    }
  }

  for (auto hit : *ecalContainer)
  {
    if (!waveformHitOK(hit)) continue;
    auto id = hit->identify();
    auto row = m_ecalHelper->row(id);
    auto mod = m_ecalHelper->module(id);
    if (row == 0)
    {
        if (mod == 0)
        {
            m_ecal00 += hit->integral();
            m_ecalTotal += hit->integral();
        }
        else if (mod == 1)
        {
            m_ecal01 += hit->integral();
            m_ecalTotal += hit->integral();
        }
        else
        {
            ATH_MSG_FATAL("Invalid Ecal module number: " << mod);
        }
    }
    else if (row == 1)
    {
        if (mod == 0)
        {
            m_ecal10 += hit->integral();
            m_ecalTotal += hit->integral();
        }
        else if (mod == 1)
        {
            m_ecal11 += hit->integral();
            m_ecalTotal += hit->integral();
        }
        else
        {
            ATH_MSG_FATAL("Invalid Ecal module number: " << mod);
        }
    }
    else
    {
        ATH_MSG_FATAL("Invalid Ecal row number: " << row);
        return StatusCode::FAILURE;
    }
  }

  SG::ReadHandle<Tracker::FaserSCT_ClusterContainer> clusterContainer { m_clusterContainer, ctx };
  ATH_CHECK(clusterContainer.isValid());

  for (auto collection : *clusterContainer)
  {
    Identifier id = collection->identify();
    int station = m_sctHelper->station(id);
    int clusters = (int) collection->size();
    switch (station)
    {
        case 0:
            m_station0Clusters += clusters;
            break;
        case 1:
            m_station1Clusters += clusters;
            break;
        case 2:
            m_station2Clusters += clusters;
            break;
        case 3:
            m_station3Clusters += clusters;
            break;
        default:
            ATH_MSG_FATAL("Unknown tracker station number " << station);
            break;
    }
  }


  SG::ReadHandle<TrackCollection> trackCollection {m_trackCollection, ctx};
  ATH_CHECK(trackCollection.isValid());

  const Trk::TrackParameters* candidateParameters {nullptr};
  const Trk::Track* candidateTrack {nullptr};

  for (const Trk::Track* track : *trackCollection)
  {
    if (track == nullptr) continue;
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
    if (nLayers < m_minTrackerLayers) continue;
    m_longTracks++;
    const Trk::TrackParameters* upstreamParameters {nullptr};
    for (auto params : *(track->trackParameters()))
    {
        if (params->position().z() < 0) continue;  // Ignore IFT hits
        if (upstreamParameters == nullptr || params->position().z() < upstreamParameters->position().z()) upstreamParameters = params;
    }
    if (candidateParameters == nullptr || upstreamParameters->momentum().mag() > candidateParameters->momentum().mag())
    {
        candidateParameters = upstreamParameters;
        candidateTrack = track;
        m_chi2 = track->fitQuality()->chiSquared();
        m_ndof = track->fitQuality()->numberDoF();
    }
  }

  SG::ReadHandle<TrackerSimDataCollection> simDataCollection {m_simDataCollection, ctx};
//   ATH_MSG_INFO("SimData valid? " << simDataCollection.isValid());
  if (candidateTrack != nullptr && simDataCollection.isValid())
  {
    std::map<int, float> truthMap;
    for (auto measurement : *(candidateTrack->measurementsOnTrack()))
    {
        const Tracker::FaserSCT_ClusterOnTrack* cluster = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*>(measurement);
        if (cluster != nullptr)
        {
            // ATH_MSG_INFO("ClusterOnTrack is OK");
            // cluster->dump(msg());

// Hack to work around issue with cluster navigation

            // auto idRDO = cluster->identify();

            // if (simDataCollection->count(idRDO) > 0)
            // {
            //     // ATH_MSG_INFO("rdo entry found");
            //     const auto& simdata = simDataCollection->find(idRDO)->second;
            //     const auto& deposits = simdata.getdeposits();
            //     //loop through deposits and record contributions
            //     HepMcParticleLink primary{};
            //     for( const auto& depositPair : deposits)
            //     {
            //         // ATH_MSG_INFO("Deposit found");
            //         float eDep = depositPair.second;
            //         int barcode = depositPair.first->barcode();
            //         // if( depositPair.second > highestDep)
            //         // {
            //         //     highestDep = depositPair.second;
            //         //     barcode = depositPair.first->barcode();
            //         //     primary = depositPair.first;
            //         //     depositPair.first->print(std::cout);
            //         //     ATH_MSG_INFO("pdg id "<<depositPair.first->pdg_id());
            //         // }
            //         if (truthMap.count(barcode) > 0)
            //         {
            //             truthMap[barcode] += eDep;
            //         }
            //         else
            //         {
            //             truthMap[barcode] = eDep;
            //         }
            //     }
            // }
                    
                


                // const Tracker::FaserSCT_Cluster* origCluster = dynamic_cast<const Tracker::FaserSCT_Cluster*>(cluster->prepRawData());
                auto origCluster = cluster->prepRawData();
                if (origCluster != nullptr)
                {
                    // ATH_MSG_INFO("Orig Cluster is OK");
                    auto rdoList = origCluster->rdoList();
                    for (auto idRDO : rdoList)
                    {
                        // ATH_MSG_INFO("rdoList not empty");
                        if (simDataCollection->count(idRDO) > 0)
                        {
                            // ATH_MSG_INFO("rdo entry found");
                            const auto& simdata = simDataCollection->find(idRDO)->second;
                            const auto& deposits = simdata.getdeposits();
                            //loop through deposits and record contributions
                            HepMcParticleLink primary{};
                            for( const auto& depositPair : deposits)
                            {
                                // ATH_MSG_INFO("Deposit found");
                                float eDep = depositPair.second;
                                int barcode = depositPair.first->barcode();
                                // if( depositPair.second > highestDep)
                                // {
                                //     highestDep = depositPair.second;
                                //     barcode = depositPair.first->barcode();
                                //     primary = depositPair.first;
                                //     depositPair.first->print(std::cout);
                                //     ATH_MSG_INFO("pdg id "<<depositPair.first->pdg_id());
                                // }
                                if (truthMap.count(barcode) > 0)
                                {
                                    truthMap[barcode] += eDep;
                                }
                                else
                                {
                                    truthMap[barcode] = eDep;
                                }
                            }
                        }
                    }
                }
        }
    }
    std::vector<std::pair<int, float>> truth(truthMap.begin(), truthMap.end());
    std::sort(truth.begin(), truth.end(), [](auto v1, auto v2) { return v1.second > v2.second; });
    // if (truth.size()>0) ATH_MSG_ALWAYS("Selected track truth info:");
    for (auto v : truth)
    {
        auto truthParticle = (*(std::find_if(truthParticleContainer->cbegin(), truthParticleContainer->cend(), [v](const xAOD::TruthParticle* p){ return p->barcode() == v.first; })));
        if (m_truthPdg == 0) m_truthPdg = truthParticle->pdgId();
        if (m_truthBarcode == 0) m_truthBarcode = v.first;
        // ATH_MSG_ALWAYS("truth info: barcode = " << v.first << " ( " << truthParticle->p4().P()/1000 << " GeV/c, Id code = " << truthParticle->pdgId() << ") -> deposited energy: " << v.second/1000);
    }
  }

  if (candidateParameters != nullptr)
  {
    m_x = candidateParameters->position().x();
    m_y = candidateParameters->position().y();
    m_z = candidateParameters->position().z();
    m_px = candidateParameters->momentum().x();
    m_py = candidateParameters->momentum().y();
    m_pz = candidateParameters->momentum().z();
    m_p = sqrt(m_px * m_px + m_py * m_py + m_pz * m_pz);
    m_charge = (int) candidateParameters->charge();

    FaserActsGeometryContext faserGeometryContext = m_trackingGeometryTool->getNominalGeometryContext();
    auto gctx = faserGeometryContext.context();


    Amg::Vector3D position = candidateParameters->position();
    Amg::Vector3D momentum = candidateParameters->momentum();
    auto covariance = *candidateParameters->covariance();
    Acts::BoundVector params = Acts::BoundVector::Zero();
    params[Acts::eBoundLoc0] = -position.y();
    params[Acts::eBoundLoc1] = position.x();
    params[Acts::eBoundPhi] = momentum.phi();
    params[Acts::eBoundTheta] = momentum.theta();
    params[Acts::eBoundQOverP] = candidateParameters->charge() / momentum.mag();
    params[Acts::eBoundTime] = 0;

    using namespace Acts::UnitLiterals;
    std::optional<AmgSymMatrix(6)> cov = std::nullopt;
    Acts::BoundMatrix newCov = Acts::BoundMatrix::Zero();
    for (size_t i = 0; i < 5; i++)
        for (size_t j = 0; j < 5; j++)
            newCov(i, j) = covariance(i, j);
    // ATH_MSG_ALWAYS("Covariance: " << covariance(1,1) << " newCov: " << newCov(1,1));
    // Convert the covariance matrix from GeV
    for(int i=0; i < newCov.rows(); i++){
      newCov(i, 4) = newCov(i, 4)/1_MeV;
    }
    for(int i=0; i < newCov.cols(); i++){
      newCov(4, i) = newCov(4, i)/1_MeV;
    }
    cov =  std::optional<AmgSymMatrix(6)>(newCov);
    // ATH_MSG_ALWAYS("cov: " << cov.value()(1,1));
    auto startSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(Acts::Vector3(0, 0, position.z()), Acts::Vector3(0, 0, 1));
    Acts::BoundTrackParameters startParameters(std::move(startSurface), params, candidateParameters->charge(), cov);
    auto targetSurface_VetoNu = Acts::Surface::makeShared<Acts::PlaneSurface>(Acts::Vector3(0, 0, m_zExtrapolate), Acts::Vector3(0, 0, 1)); // -3112 mm is z position of VetoNu planes touching
    std::unique_ptr<const Acts::BoundTrackParameters> targetParameters_VetoNu =m_extrapolationTool->propagate(ctx, startParameters, *targetSurface_VetoNu, Acts::backward);
    if (targetParameters_VetoNu != nullptr) 
    {
        auto targetPosition_VetoNu = targetParameters_VetoNu->position(gctx);
        auto targetMomentum_VetoNu = targetParameters_VetoNu->momentum();
        // Acts::BoundSymMatrix targetCovariance_VetoNu { targetParameters_VetoNu->covariance() };
        m_xVetoNu = targetPosition_VetoNu.x();
        m_yVetoNu = targetPosition_VetoNu.y();
        if (targetParameters_VetoNu->covariance().has_value())
        {
            auto targetCovariance_VetoNu = targetParameters_VetoNu->covariance().value() ;
            m_sxVetoNu = sqrt(targetCovariance_VetoNu(Acts::eBoundLoc0, Acts::eBoundLoc0));
            m_syVetoNu = sqrt(targetCovariance_VetoNu(Acts::eBoundLoc1, Acts::eBoundLoc1));
        }
        m_thetaxVetoNu = atan(targetMomentum_VetoNu[0]/targetMomentum_VetoNu[2]);
        m_thetayVetoNu = atan(targetMomentum_VetoNu[1]/targetMomentum_VetoNu[2]);
        ATH_MSG_INFO("vetoNu good targetParameters xV,yV = (" << m_xVetoNu << ", " << m_yVetoNu << ")");
    } 
    else 
    {
        ATH_MSG_INFO("vetoNu null targetParameters with p = " << momentum.mag() << ", theta = " << momentum.theta() << ", x,y = (" << position.x() << ", " << position.y() << ")");
    }
  }

  // Here we apply the signal selection
  // Very simple/unrealistic to start
  if (m_vetoUpstream == 0 || m_vetoDownstream == 0 ||
        m_triggerTotal == 0 ||
        m_preshower0 == 0 || m_preshower1 == 0 ||
        // m_ecalTotal == 0 ||
        candidateParameters == nullptr)
    {
        // if (m_longTracks > 0)
        // {
        //     ATH_MSG_ALWAYS(m_vetoUpstream);
        //     ATH_MSG_ALWAYS(m_vetoDownstream);
        //     ATH_MSG_ALWAYS(m_triggerTotal);
        //     ATH_MSG_ALWAYS(m_preshower0);
        //     ATH_MSG_ALWAYS(m_preshower1);
        //     ATH_MSG_ALWAYS((candidateParameters == nullptr ? "Null" : "Not Null"));
        // }
        return StatusCode::SUCCESS;
    }

  m_tree->Fill();

  return StatusCode::SUCCESS;
}


StatusCode NeutrinoSearchAlg::finalize() 
{
  return StatusCode::SUCCESS;
}

bool NeutrinoSearchAlg::waveformHitOK(const xAOD::WaveformHit* hit) const
{
    if (hit->status_bit(xAOD::WaveformStatus::THRESHOLD_FAILED) || hit->status_bit(xAOD::WaveformStatus::SECONDARY)) return false;
    return true;
}

void
NeutrinoSearchAlg::clearTree() const
{
  m_run_number = 0;
  m_event_number = 0;
//   m_vetoNuHitsMeanX = 0;
//   m_vetoNuHitsMeanY = 0;
  m_vetoNu0 = 0;
  m_vetoNu1 = 0;
  m_veto00 = 0;
  m_veto01 = 0;
  m_veto10 = 0;
  m_veto11 = 0;
  m_vetoUpstream = 0;
  m_vetoDownstream = 0;
  m_trigger00 = 0;
  m_trigger01 = 0;
  m_trigger10 = 0;
  m_trigger11 = 0;
  m_triggerTotal = 0;
  m_preshower0 = 0;
  m_preshower1 = 0;
  m_ecal00 = 0;
  m_ecal01 = 0;
  m_ecal10 = 0;
  m_ecal11 = 0;
  m_ecalTotal = 0;
  m_station0Clusters = 0;
  m_station1Clusters = 0;
  m_station2Clusters = 0;
  m_station3Clusters = 0;
  m_crossSection = 0;
  m_chi2 = 0;
  m_ndof = 0;
  m_px = 0;
  m_py = 0;
  m_pz = 0;
  m_p = 0;
  m_charge = 0;
  m_x = 0;
  m_y = 0;
  m_z = 0;
  m_longTracks = 0;
  m_truthLeptonX = 0;
  m_truthLeptonY = 0;
  m_truthLeptonMomentum = 0;
  m_truthBarcode = 0;
  m_truthPdg = 0;
  m_xVetoNu = 0;
  m_yVetoNu = 0;
  m_sxVetoNu = 0;
  m_syVetoNu = 0;
  m_thetaxVetoNu = 0;
  m_thetayVetoNu = 0;
}
