#include"RootSeedWriteTool.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "Identifier/Identifier.h"
#include "TrackClassification.h"
//#include "GeoPrimitives/GeoPrimitives.h"
//#include "Acts/EventData/MultiTrajectory.hpp"
//#include "Acts/EventData/MultiTrajectoryHelpers.hpp"
#include "Acts/Utilities/Helpers.hpp"
//#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "xAODTruth/TruthParticle.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"

#include <TFile.h>
#include <TTree.h>
#include<string>
RootSeedWriterTool::RootSeedWriterTool(
    const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent) {}

/// NaN values for TTree variables
//constexpr float NaNfloat = std::numeric_limits<float>::quiet_NaN();
constexpr float NaNfloat = std::numeric_limits<float>::quiet_NaN();
constexpr float NaNdouble = std::numeric_limits<double>::quiet_NaN();
constexpr int NaNint = std::numeric_limits<int>::quiet_NaN();
using Acts::VectorHelpers::eta;
using Acts::VectorHelpers::perp;
using Acts::VectorHelpers::phi;
using Acts::VectorHelpers::theta;


StatusCode RootSeedWriterTool::initialize() {
//  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
  ATH_CHECK(m_mcEventCollectionKey.initialize());
  ATH_CHECK(m_simDataCollectionKey.initialize());
  ATH_CHECK(m_truthParticleContainer.initialize());

  if (!m_noDiagnostics) {
    std::string filePath = m_filePath;
    std::string treeName = m_treeName;
    m_outputFile = TFile::Open(filePath.c_str(), "RECREATE");
    if (m_outputFile == nullptr) {
      ATH_MSG_WARNING("Unable to open output file at " << m_filePath);
      return StatusCode::RECOVERABLE;
    }
    m_outputFile->cd();
    m_outputTree = new TTree(treeName.c_str(), treeName.c_str());
    if (m_outputTree == nullptr) {
      ATH_MSG_ERROR("Unable to create TTree");
      return StatusCode::FAILURE;
    }
    
    
//    m_outputTree->Branch("truth_P", &m_truth_P);
//    m_outputTree->Branch("truth_px", &m_truth_px);
//    m_outputTree->Branch("truth_py", &m_truth_py);
//    m_outputTree->Branch("truth_pz", &m_truth_pz);
//    m_outputTree->Branch("truth_pT", &m_truth_pT);
//    m_outputTree->Branch("truth_pdg", &m_truth_pdg);
//    m_outputTree->Branch("truth_barcode", &m_truth_barcode);
//    m_outputTree->Branch("truth_theta", &m_truth_theta);
//    m_outputTree->Branch("truth_phi", &m_truth_phi);
//    m_outputTree->Branch("truth_eta", &m_truth_eta);

    m_outputTree->Branch("event_number", &m_eventNr);
    m_outputTree->Branch("nSeeds", &m_nseeds);
    m_outputTree->Branch("nClusters", &m_nClusters);
    m_outputTree->Branch("nMeasurements", &m_nMeasurements);
    m_outputTree->Branch("nSegments", &m_nSegments);
    m_outputTree->Branch("Positionx", &m_positionX);
    m_outputTree->Branch("Positiony", &m_positionY);
    m_outputTree->Branch("Positionz", &m_positionZ);
    m_outputTree->Branch("Spacepoint_x", &m_spacePointX);
    m_outputTree->Branch("Spacepoint_y", &m_spacePointY);
    m_outputTree->Branch("Spacepoint_z", &m_spacePointZ);
    m_outputTree->Branch("Fakeposition_x", &m_fakePositionX);
    m_outputTree->Branch("FakePosition_y", &m_fakePositionY);
    m_outputTree->Branch("FakePosition_z", &m_fakePositionZ);

    m_outputTree->Branch("nMajorityHits", &m_nMajorityHits);
    m_outputTree->Branch("majorityParticleId", &m_majorityParticleId);
    m_outputTree->Branch("t_truthHitRatio", &m_t_truthHitRatio);
    m_outputTree->Branch("t_charge", &m_t_charge);
    m_outputTree->Branch("t_time", &m_t_time);
    m_outputTree->Branch("t_vx", &m_t_vx);
    m_outputTree->Branch("t_vy", &m_t_vy);
    m_outputTree->Branch("t_vz", &m_t_vz);
    m_outputTree->Branch("t_px", &m_t_px);
    m_outputTree->Branch("t_py", &m_t_py);
    m_outputTree->Branch("t_pz", &m_t_pz);
    m_outputTree->Branch("t_theta", &m_t_theta);
    m_outputTree->Branch("t_phi", &m_t_phi);
    m_outputTree->Branch("t_eta", &m_t_eta);
    m_outputTree->Branch("t_p", &m_t_p);
    m_outputTree->Branch("t_pT", &m_t_pT);


  }
  return StatusCode::SUCCESS;
}
StatusCode RootSeedWriterTool::finalize() {
  if (!m_noDiagnostics) {
    m_outputFile->cd();
    m_outputTree->Write();
    m_outputFile->Close();
  }
  return StatusCode::SUCCESS;
}


StatusCode RootSeedWriterTool::write(const Acts::GeometryContext& geoContext, const std::vector<CircleFitTrackSeedTool::Seed> &seeds, bool isMC) const {
  EventContext ctx = Gaudi::Hive::currentContext();

  std::shared_ptr<TrackerSimDataCollection> simData {nullptr};
  std::map<int, const HepMC::GenParticle*> particles {};
  if (isMC) {

  //  //get the truthInfo of all truth particles 
  //  SG::ReadHandle<xAOD::TruthParticleContainer> truthParticleContainer { m_truthParticleContainer, ctx };
  //  if (truthParticleContainer.isValid() && truthParticleContainer->size() > 0) {
  //    for (auto particle : *truthParticleContainer) {
  //      
  //      m_truth_pdg.push_back(particle->pdgId());
  //      m_truth_barcode.push_back(particle->barcode());
  //      m_truth_P.push_back(particle->p4().P());
  //      m_truth_px.push_back(particle->p4().X());
  //      m_truth_py.push_back(particle->p4().Y());
  //      m_truth_pz.push_back(particle->p4().Z());
  //      m_truth_pT.push_back(particle->p4().Pt());
  //      m_truth_theta.push_back(particle->p4().Theta());
  //      m_truth_phi.push_back(particle->p4().Phi());
  //      m_truth_eta.push_back(particle->p4().Eta());
  //    }
  //  }

    SG::ReadHandle<TrackerSimDataCollection> simDataHandle {m_simDataCollectionKey, ctx};
    ATH_CHECK(simDataHandle.isValid());
    simData = std::make_shared<TrackerSimDataCollection>(*simDataHandle);
    
    SG::ReadHandle<McEventCollection> mcEvents {m_mcEventCollectionKey, ctx};
    ATH_CHECK(mcEvents.isValid());
    if (mcEvents->size() != 1) {
      ATH_MSG_ERROR("There should be exactly one event in the McEventCollection.");
      return StatusCode::FAILURE;
    }
    for (const auto& particle : mcEvents->front()->particles()) {
      particles[HepMC::barcode(particle)] = &(*particle);
    }
  }
  
  // For each particle within a track, how many hits did it contribute
  std::vector<ParticleHitCount> particleHitCounts;

  // Loop over the seeds from CircleFitTrackSeedTool
  m_eventNr = ctx.eventID().event_number();
  m_nseeds += seeds.size();
  for(auto seed : seeds){
    m_nClusters.push_back(seed.clusters.size());
    m_nMeasurements.push_back(seed.Measurements.size());
    m_nSegments.push_back(seed.stations);//seed has 'stations'variable,it means the size of segments;
      
    for(auto po : seed.positions){
      m_positionX.push_back(po.x());
      m_positionY.push_back(po.y());
      m_positionZ.push_back(po.z());
    }
  
    for(auto fake : seed.fakePositions){
      m_fakePositionX.push_back(fake.x());
      m_fakePositionY.push_back(fake.y());
      m_fakePositionZ.push_back(fake.z());
    }
    for(auto sp : seed.spacePoints){
      auto pos = sp->globalPosition();
      m_spacePointX.push_back(pos.x());
      m_spacePointY.push_back(pos.y());
      m_spacePointZ.push_back(pos.z());
    }


    //initilize the truth info;
    uint64_t majorityParticleId = NaNint;
    double nMajorityHits = NaNdouble;
    double t_truthHitRatio = NaNdouble;
    float t_charge = NaNint;
    float t_time = NaNfloat;
    float t_vx = NaNfloat;
    float t_vy = NaNfloat;
    float t_vz = NaNfloat;
    float t_px = NaNfloat;
    float t_py = NaNfloat;
    float t_pz = NaNfloat;
    float t_theta = NaNfloat;
    float t_phi = NaNfloat;
    float t_eta = NaNfloat;
    float t_p = NaNfloat;
    float t_pT = NaNfloat;
    //float t_d0 = NaNfloat;
    //float t_z0 = NaNfloat;

    if (isMC) {
      // Get the majority truth particle to this seed
      ATH_MSG_VERBOSE("get majority truth particle");
      identifyContributingParticles(*simData, seed.clusters, particleHitCounts);
      for (const auto& particle : particleHitCounts) {
        ATH_MSG_VERBOSE(particle.particleId << ": " << particle.hitCount << " hits");
      }

      bool foundMajorityParticle = false;
      // Get the truth particle info
      if (not particleHitCounts.empty()) {
        // Get the barcode of the majority truth particle
        majorityParticleId = particleHitCounts.front().particleId;
        nMajorityHits = particleHitCounts.front().hitCount;

	//Get the purity of every seed
        t_truthHitRatio = nMajorityHits / seed.Measurements.size();
        
	// Find the truth particle via the barcode
        auto ip = particles.find(majorityParticleId);
        if (ip != particles.end()) {
          foundMajorityParticle = true;

          const HepMC::GenParticle* particle = ip->second;
          ATH_MSG_DEBUG("Find the truth particle with barcode = " << majorityParticleId);
	
       	  // extrapolate parameters from vertex to reference surface at origin.
          std::optional<const Acts::BoundTrackParameters> truthParameters
              = m_summaryTool->extrapolateToReferenceSurface(ctx, particle);
          if (!truthParameters) {
            continue;
          }
          // Get the truth particle info at vertex
          // const HepMC::GenVertex* vertex = particle->production_vertex();
          Acts::Vector3 v;
          t_p = truthParameters->momentum().norm();
          t_charge = truthParameters->charge();
          t_time = truthParameters->time();
          t_vx = truthParameters->position(geoContext).x();
          t_vy = truthParameters->position(geoContext).y();
          t_vz = truthParameters->position(geoContext).z();
          t_px = truthParameters->momentum().x();
          t_py = truthParameters->momentum().y();
          t_pz = truthParameters->momentum().z();
          t_theta = theta(truthParameters->momentum().normalized());
          t_phi = phi(truthParameters->momentum().normalized());
          t_eta = eta(truthParameters->momentum().normalized());
          t_pT = t_p * perp(truthParameters->momentum().normalized());
        }
	else {
	  ATH_MSG_WARNING("Truth particle with barcode = " << majorityParticleId << " not found in the input collection!");
	}
      }
      if (not foundMajorityParticle) {
        ATH_MSG_WARNING("Truth particle for seed is not found!");
      }
    }
    m_majorityParticleId.push_back(majorityParticleId);
    m_nMajorityHits.push_back(nMajorityHits);
    m_t_truthHitRatio.push_back(t_truthHitRatio);
    m_t_charge.push_back(t_charge);
    m_t_time.push_back(t_time);
    m_t_vx.push_back(t_vx);
    m_t_vy.push_back(t_vy);
    m_t_vz.push_back(t_vz);
    m_t_px.push_back(t_px);
    m_t_py.push_back(t_py);
    m_t_pz.push_back(t_pz);
    m_t_theta.push_back(t_theta);
    m_t_phi.push_back(t_phi);
    m_t_eta.push_back(t_eta);
    m_t_p.push_back(t_p);
    m_t_pT.push_back(t_pT);
  } 
    
  m_outputTree->Fill();
  
//  m_truth_P.clear();
//  m_truth_px.clear();
//  m_truth_py.clear();
//  m_truth_pz.clear();
//  m_truth_pT.clear();
//  m_truth_pdg.clear();
//  m_truth_barcode.clear();
//  m_truth_theta.clear();
//  m_truth_phi.clear(); 
//  m_truth_eta.clear();
  m_nseeds=0;
  m_nClusters.clear();
  m_nMeasurements.clear();
  m_nSegments.clear();

  m_positionX.clear();
  m_positionY.clear();
  m_positionZ.clear();
  
  m_spacePointX.clear();
  m_spacePointY.clear();
  m_spacePointZ.clear();
  
  m_fakePositionX.clear();
  m_fakePositionY.clear();
  m_fakePositionZ.clear();

  m_nMajorityHits.clear();
  m_majorityParticleId.clear();
  m_t_truthHitRatio.clear();
  m_t_charge.clear();
  m_t_time.clear();
  m_t_vx.clear();
  m_t_vy.clear();
  m_t_vz.clear();
  m_t_px.clear();
  m_t_py.clear();
  m_t_pz.clear();
  m_t_theta.clear();
  m_t_phi.clear();
  m_t_p.clear();
  m_t_pT.clear();
  m_t_eta.clear();

  return StatusCode::SUCCESS;  
}
