// Amg

// Ensure Eigen plugin comes first
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"


#include "RootTrajectorySummaryWriterTool.h"

#include "Acts/EventData/MultiTrajectory.hpp"
#include "Acts/EventData/MultiTrajectoryHelpers.hpp"
#include "Acts/EventData/detail/TransformationBoundToFree.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "FaserActsRecMultiTrajectory.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackClassification.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include <TFile.h>
#include <TTree.h>

/// NaN values for TTree variables
constexpr float NaNfloat = std::numeric_limits<float>::quiet_NaN();
constexpr float NaNint = std::numeric_limits<int>::quiet_NaN();


using Acts::VectorHelpers::eta;
using Acts::VectorHelpers::perp;
using Acts::VectorHelpers::phi;
using Acts::VectorHelpers::theta;

RootTrajectorySummaryWriterTool::RootTrajectorySummaryWriterTool(
    const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent) {}


StatusCode RootTrajectorySummaryWriterTool::initialize() {
  ATH_CHECK(m_simDataCollectionKey.initialize());
  ATH_CHECK(m_mcEventCollectionKey.initialize());
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));

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

    m_outputTree = new TTree("tree", "tree");

    m_outputTree->Branch("event_nr", &m_eventNr);
    m_outputTree->Branch("track_nr", &m_trackNr); 

    m_outputTree->Branch("nStates", &m_nStates);
    m_outputTree->Branch("nMeasurements", &m_nMeasurements);
    m_outputTree->Branch("nOutliers", &m_nOutliers);
    m_outputTree->Branch("nHoles", &m_nHoles);
    m_outputTree->Branch("nSharedHits", &m_nSharedHits);
    m_outputTree->Branch("chi2Sum", &m_chi2Sum);
    m_outputTree->Branch("NDF", &m_NDF);
    m_outputTree->Branch("rMax_hit", &m_rMax_hit);
    m_outputTree->Branch("measurementChi2", &m_measurementChi2);
    m_outputTree->Branch("outlierChi2", &m_outlierChi2);
    m_outputTree->Branch("measurementVolume", &m_measurementVolume);
    m_outputTree->Branch("measurementLayer", &m_measurementLayer);
    m_outputTree->Branch("measurementIndex", &m_measurementIndex);
    m_outputTree->Branch("outlierVolume", &m_outlierVolume);
    m_outputTree->Branch("outlierLayer", &m_outlierLayer);
    m_outputTree->Branch("outlierIndex", &m_outlierIndex);

    m_outputTree->Branch("nMajorityHits", &m_nMajorityHits);
    m_outputTree->Branch("majorityParticleId", &m_majorityParticleId);
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
    m_outputTree->Branch("t_d0", &m_t_d0);
    m_outputTree->Branch("t_z0", &m_t_z0);


    m_outputTree->Branch("hasFittedParams", &m_hasFittedParams);
    m_outputTree->Branch("surface_x", &m_surface_x);
    m_outputTree->Branch("surface_y", &m_surface_y);
    m_outputTree->Branch("surface_z", &m_surface_z);
    m_outputTree->Branch("eLOC0_fit", &m_eLOC0_fit);
    m_outputTree->Branch("eLOC1_fit", &m_eLOC1_fit);
    m_outputTree->Branch("ePHI_fit", &m_ePHI_fit);
    m_outputTree->Branch("eTHETA_fit", &m_eTHETA_fit);
    m_outputTree->Branch("eQOP_fit", &m_eQOP_fit);
    m_outputTree->Branch("eT_fit", &m_eT_fit);
    m_outputTree->Branch("err_eLOC0_fit", &m_err_eLOC0_fit);
    m_outputTree->Branch("err_eLOC1_fit", &m_err_eLOC1_fit);
    m_outputTree->Branch("err_ePHI_fit", &m_err_ePHI_fit);
    m_outputTree->Branch("err_eTHETA_fit", &m_err_eTHETA_fit);
    m_outputTree->Branch("err_eQOP_fit", &m_err_eQOP_fit);
    m_outputTree->Branch("err_eT_fit", &m_err_eT_fit);
    m_outputTree->Branch("res_eLOC0_fit", &m_res_eLOC0_fit);
    m_outputTree->Branch("res_eLOC1_fit", &m_res_eLOC1_fit);
    m_outputTree->Branch("res_ePHI_fit", &m_res_ePHI_fit);
    m_outputTree->Branch("res_eTHETA_fit", &m_res_eTHETA_fit);
    m_outputTree->Branch("res_eQOP_fit", &m_res_eQOP_fit);
    m_outputTree->Branch("res_eT_fit", &m_res_eT_fit);
    m_outputTree->Branch("pull_eLOC0_fit", &m_pull_eLOC0_fit);
    m_outputTree->Branch("pull_eLOC1_fit", &m_pull_eLOC1_fit);
    m_outputTree->Branch("pull_ePHI_fit", &m_pull_ePHI_fit);
    m_outputTree->Branch("pull_eTHETA_fit", &m_pull_eTHETA_fit);
    m_outputTree->Branch("pull_eQOP_fit", &m_pull_eQOP_fit);
    m_outputTree->Branch("pull_eT_fit", &m_pull_eT_fit);
  }

  return StatusCode::SUCCESS;
}


StatusCode RootTrajectorySummaryWriterTool::finalize() {
  if (!m_noDiagnostics) {
    m_outputFile->cd();
    m_outputTree->Write();
    m_outputFile->Close();
  }
  return StatusCode::SUCCESS;
}


StatusCode RootTrajectorySummaryWriterTool::write(
    const Acts::GeometryContext& geoContext, const FaserActsTrackContainer& tracks, bool isMC) const {
  EventContext ctx = Gaudi::Hive::currentContext();

  std::shared_ptr<TrackerSimDataCollection> simData {nullptr};
  std::map<int, const HepMC::GenParticle*> particles {};

  if (isMC) {
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

  // Get the event number
  m_eventNr = ctx.eventID().event_number();

  Acts::Vector3 truthUnitDir(1,1,1);
  // Loop over the trajectories
  for (const auto& track : tracks) {
    m_trackNr.push_back(track.index());

    // Collect the trajectory summary info
    m_nStates.push_back(track.nTrackStates());
    m_nMeasurements.push_back(track.nMeasurements());
    m_nOutliers.push_back(track.nOutliers());
    m_nHoles.push_back(track.nHoles());
    m_nSharedHits.push_back(track.nSharedHits());
    m_chi2Sum.push_back(track.chi2());
    m_NDF.push_back(track.nDoF());

    {
      std::vector<double> measurementChi2;
      std::vector<double> measurementVolume;
      std::vector<double> measurementLayer;
      std::vector<int> measurementIndex;
      std::vector<double> outlierChi2;
      std::vector<double> outlierVolume;
      std::vector<double> outlierLayer;
      std::vector<int> outlierIndex;
      double rMax_hit = 0; 
      for (const auto& state : track.trackStatesReversed()) {
        const auto& geoID = state.referenceSurface().geometryId();
        const auto& volume = geoID.volume();
        const auto& layer = geoID.layer();
        if (state.typeFlags().test(Acts::TrackStateFlag::MeasurementFlag)) {
          measurementChi2.push_back(state.chi2());
          measurementVolume.push_back(volume);
          measurementLayer.push_back(layer);
          // expand the local measurements into the full bound space
          Acts::BoundVector meas = state.effectiveProjector().transpose() *
                               state.effectiveCalibrated();
          // extract local and global position
          Acts::Vector2 local(meas[Acts::eBoundLoc0], meas[Acts::eBoundLoc1]);
          const auto& surface = state.referenceSurface(); 
          // The direction does not matter for plane surface 
          Acts::Vector3 global =
              surface.localToGlobal(geoContext, local, truthUnitDir);
          auto r_hit = std::hypot(global[Acts::ePos0],global[Acts::ePos1]);
          if(r_hit>rMax_hit){
            rMax_hit = r_hit;
          }
          IndexSourceLink sl = state.getUncalibratedSourceLink().template get<IndexSourceLink>();
          measurementIndex.push_back(sl.index());
        }
        if (state.typeFlags().test(Acts::TrackStateFlag::OutlierFlag)) {
          outlierChi2.push_back(state.chi2());
          outlierVolume.push_back(volume);
          outlierLayer.push_back(layer);
          IndexSourceLink sl = state.getUncalibratedSourceLink().template get<IndexSourceLink>();
          outlierIndex.push_back(sl.index());
        }
      }


      // IDs are stored as double (as the vector of vector of int is not known
      // to ROOT)
      m_measurementChi2.push_back(std::move(measurementChi2));
      m_measurementVolume.push_back(std::move(measurementVolume));
      m_measurementLayer.push_back(std::move(measurementLayer));
      m_measurementIndex.push_back(std::move(measurementIndex));
      m_outlierChi2.push_back(std::move(outlierChi2));
      m_outlierVolume.push_back(std::move(outlierVolume));
      m_outlierLayer.push_back(std::move(outlierLayer));
      m_outlierIndex.push_back(std::move(outlierIndex));
      m_rMax_hit.push_back(rMax_hit); 
    }

    // Initialize the truth particle info
    uint64_t majorityParticleId = NaNint;
    unsigned int nMajorityHits = NaNint;
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
    float t_d0 = NaNfloat;
    float t_z0 = NaNfloat;


    // Get the perigee surface
    const Acts::Surface* pSurface =
      track.hasReferenceSurface() ? &track.referenceSurface() : nullptr;


    if (isMC) {
      // Get the majority truth particle to this track
      ATH_MSG_VERBOSE("get majority truth particle");
      identifyContributingParticles(*simData, track, particleHitCounts);
      for (const auto& particle : particleHitCounts) {
        ATH_MSG_VERBOSE(particle.particleId << ": " << particle.hitCount << " hits");
      }

      bool foundMajorityParticle = false;
      // Get the truth particle info
      if (not particleHitCounts.empty()) {
        // Get the barcode of the majority truth particle
        majorityParticleId = particleHitCounts.front().particleId;
        nMajorityHits = particleHitCounts.front().hitCount;

        // Find the truth particle via the barcode
        auto ip = particles.find(majorityParticleId);
        if (ip != particles.end()) {
          foundMajorityParticle = true;

          const HepMC::GenParticle* particle = ip->second;
          ATH_MSG_DEBUG("Find the truth particle with barcode = " << majorityParticleId);

          // extrapolate parameters from vertex to reference surface at origin.
          std::optional<const Acts::BoundTrackParameters> truthParameters
              = extrapolateToReferenceSurface(ctx, particle);
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

          auto unitDirection = Acts::Vector3(t_px, t_py, t_pz).normalized();
          auto position = Acts::Vector3 (t_vx, t_vy, t_vz);
   
          if (pSurface != nullptr) {
            auto intersection =
                pSurface
                    ->intersect(geoContext, position,
                                unitDirection, Acts::BoundaryCheck(false))
                    .closest();
            auto position = intersection.position();

            // get the truth perigee parameter
            auto lpResult = pSurface->globalToLocal(geoContext, position,
                                                    unitDirection);
            if (lpResult.ok()) {
              t_d0 = lpResult.value()[Acts::BoundIndices::eBoundLoc0];
              t_z0 = lpResult.value()[Acts::BoundIndices::eBoundLoc1];
            } else {
              ATH_MSG_ERROR("Global to local transformation did not succeed.");
            }
          }

        } else {
          ATH_MSG_WARNING("Truth particle with barcode = " << majorityParticleId << " not found in the input collection!");
        }
      }
      if (not foundMajorityParticle) {
        ATH_MSG_WARNING("Truth particle for track " << track.index() << " not found!");
      }
    }

    // Push the corresponding truth particle info for the track.
    // Always push back even if majority particle not found
    m_majorityParticleId.push_back(majorityParticleId);
    m_nMajorityHits.push_back(nMajorityHits);
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
    m_t_d0.push_back(t_d0);
    m_t_z0.push_back(t_z0);

    // Initialize the fitted track parameters info
    std::array<float, Acts::eBoundSize> param = {NaNfloat, NaNfloat, NaNfloat, NaNfloat, NaNfloat, NaNfloat};
    std::array<float, Acts::eBoundSize> res = {NaNfloat, NaNfloat, NaNfloat, NaNfloat, NaNfloat, NaNfloat};
    std::array<float, Acts::eBoundSize> error = {NaNfloat, NaNfloat, NaNfloat, NaNfloat, NaNfloat, NaNfloat};
    std::array<float, Acts::eBoundSize> pull = {NaNfloat, NaNfloat, NaNfloat, NaNfloat, NaNfloat, NaNfloat};
    float surfaceX = NaNfloat; 
    float surfaceY = NaNfloat; 
    float surfaceZ = NaNfloat; 
    bool hasFittedParams = track.hasReferenceSurface();
    if (hasFittedParams) {
      const auto& parameter = track.parameters(); 
      for (unsigned int i = 0; i < Acts::eBoundSize; ++i) {
        param[i] = parameter[i];
      }

      res = {param[Acts::eBoundLoc0] - t_d0,
             param[Acts::eBoundLoc1] - t_z0,
             param[Acts::eBoundPhi] - t_phi,
             param[Acts::eBoundTheta] - t_theta,
             param[Acts::eBoundQOverP] - t_charge / t_p,
             param[Acts::eBoundTime] - t_time};

      const auto& covariance = track.covariance(); 
      for (unsigned int i = 0; i < Acts::eBoundSize; ++i) {
        error[i] = std::sqrt(covariance(i, i));
        pull[i] = res[i] / error[i];
      }
      auto surface = &(track.referenceSurface()); 
      auto center = surface->center(geoContext);
      surfaceX = center.x(); 
      surfaceY = center.y(); 
      surfaceZ = center.z(); 
    }

    // Push the fitted track parameters.
    // Always push back even if no fitted track parameters
    m_surface_x.push_back(surfaceX); 
    m_surface_y.push_back(surfaceY); 
    m_surface_z.push_back(surfaceZ); 
    m_eLOC0_fit.push_back(param[Acts::eBoundLoc0]);
    m_eLOC1_fit.push_back(param[Acts::eBoundLoc1]);
    m_ePHI_fit.push_back(param[Acts::eBoundPhi]);
    m_eTHETA_fit.push_back(param[Acts::eBoundTheta]);
    m_eQOP_fit.push_back(param[Acts::eBoundQOverP]);
    m_eT_fit.push_back(param[Acts::eBoundTime]);

    m_res_eLOC0_fit.push_back(res[Acts::eBoundLoc0]);
    m_res_eLOC1_fit.push_back(res[Acts::eBoundLoc1]);
    m_res_ePHI_fit.push_back(res[Acts::eBoundPhi]);
    m_res_eTHETA_fit.push_back(res[Acts::eBoundTheta]);
    m_res_eQOP_fit.push_back(res[Acts::eBoundQOverP]);
    m_res_eT_fit.push_back(res[Acts::eBoundTime]);

    m_err_eLOC0_fit.push_back(error[Acts::eBoundLoc0]);
    m_err_eLOC1_fit.push_back(error[Acts::eBoundLoc1]);
    m_err_ePHI_fit.push_back(error[Acts::eBoundPhi]);
    m_err_eTHETA_fit.push_back(error[Acts::eBoundTheta]);
    m_err_eQOP_fit.push_back(error[Acts::eBoundQOverP]);
    m_err_eT_fit.push_back(error[Acts::eBoundTime]);

    m_pull_eLOC0_fit.push_back(pull[Acts::eBoundLoc0]);
    m_pull_eLOC1_fit.push_back(pull[Acts::eBoundLoc1]);
    m_pull_ePHI_fit.push_back(pull[Acts::eBoundPhi]);
    m_pull_eTHETA_fit.push_back(pull[Acts::eBoundTheta]);
    m_pull_eQOP_fit.push_back(pull[Acts::eBoundQOverP]);
    m_pull_eT_fit.push_back(pull[Acts::eBoundTime]);

    m_hasFittedParams.push_back(hasFittedParams);
  }    // all tracks 


  // fill the variables
  m_outputTree->Fill();

  m_trackNr.clear();
  m_nStates.clear();
  m_nMeasurements.clear();
  m_nOutliers.clear();
  m_nHoles.clear();
  m_nSharedHits.clear();
  m_chi2Sum.clear();
  m_NDF.clear();
  m_measurementChi2.clear();
  m_outlierChi2.clear();
  m_measurementVolume.clear();
  m_measurementLayer.clear();
  m_measurementIndex.clear();
  m_outlierVolume.clear();
  m_outlierLayer.clear();
  m_outlierIndex.clear();
  m_rMax_hit.clear();

  m_nMajorityHits.clear();
  m_majorityParticleId.clear();
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
  m_t_d0.clear();
  m_t_z0.clear();

  m_hasFittedParams.clear();
  m_surface_x.clear(); 
  m_surface_y.clear(); 
  m_surface_z.clear(); 
  m_eLOC0_fit.clear();
  m_eLOC1_fit.clear();
  m_ePHI_fit.clear();
  m_eTHETA_fit.clear();
  m_eQOP_fit.clear();
  m_eT_fit.clear();
  m_err_eLOC0_fit.clear();
  m_err_eLOC1_fit.clear();
  m_err_ePHI_fit.clear();
  m_err_eTHETA_fit.clear();
  m_err_eQOP_fit.clear();
  m_err_eT_fit.clear();
  m_res_eLOC0_fit.clear();
  m_res_eLOC1_fit.clear();
  m_res_ePHI_fit.clear();
  m_res_eTHETA_fit.clear();
  m_res_eQOP_fit.clear();
  m_res_eT_fit.clear();
  m_pull_eLOC0_fit.clear();
  m_pull_eLOC1_fit.clear();
  m_pull_ePHI_fit.clear();
  m_pull_eTHETA_fit.clear();
  m_pull_eQOP_fit.clear();
  m_pull_eT_fit.clear();

  return StatusCode::SUCCESS;
}


std::optional<const Acts::BoundTrackParameters> RootTrajectorySummaryWriterTool::extrapolateToReferenceSurface(
    const EventContext& ctx, const HepMC::GenParticle* particle) const {
  const HepMC::FourVector &vertex = particle->production_vertex()->position();
  const HepMC::FourVector &momentum = particle->momentum();

  // The coordinate system of the Acts::PlaneSurface is defined as
  // T = Z = normal, U = X x T = -Y, V = T x U = x
  auto startSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3(0, 0, vertex.z()), Acts::Vector3(0, 0, 1));
  auto targetSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3(0, 0, 0), Acts::Vector3(0, 0, -1));
  Acts::BoundVector params = Acts::BoundVector::Zero();
  params[Acts::eBoundLoc0] = -vertex.y();
  params[Acts::eBoundLoc1] = vertex.x();
  params[Acts::eBoundPhi] = momentum.phi();
  params[Acts::eBoundTheta] = momentum.theta();
  // FIXME get charge of HepMC::GenParticle, the following does not work, e.g. for pions
  double charge = particle->pdg_id() > 0 ? -1 : 1;
  double MeV2GeV = 1e-3;
  params[Acts::eBoundQOverP] = charge / (momentum.rho() * MeV2GeV);
  params[Acts::eBoundTime] = vertex.t();
  Acts::BoundTrackParameters startParameters(std::move(startSurface), params,  std::nullopt, Acts::ParticleHypothesis::pion());
  std::optional<const Acts::BoundTrackParameters> targetParameters =
      m_extrapolationTool->propagate(ctx, startParameters, *targetSurface);
  return targetParameters;
}
