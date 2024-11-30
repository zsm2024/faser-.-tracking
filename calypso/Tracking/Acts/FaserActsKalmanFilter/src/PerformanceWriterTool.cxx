#include "PerformanceWriterTool.h"
#include "TrackClassification.h"
#include "Acts/EventData/MultiTrajectory.hpp"
#include "Acts/EventData/MultiTrajectoryHelpers.hpp"
#include "TFile.h"


PerformanceWriterTool::PerformanceWriterTool(
    const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent) {}


StatusCode PerformanceWriterTool::initialize() {
    ATH_CHECK(m_extrapolationTool.retrieve());
    ATH_CHECK(m_mcEventCollectionKey.initialize());
    ATH_CHECK(m_simDataCollectionKey.initialize());

    if (!m_noDiagnostics) {
      std::string filePath = m_filePath;
      m_outputFile = TFile::Open(filePath.c_str(), "RECREATE");
      if (m_outputFile == nullptr) {
        ATH_MSG_WARNING("Unable to open output file at " << m_filePath);
        return StatusCode::RECOVERABLE;
      }
    }

    // initialize the residual and efficiency plots tool
    m_resPlotTool.book(m_resPlotCache);
    m_effPlotTool.book(m_effPlotCache);
    m_summaryPlotTool.book(m_summaryPlotCache);
  return StatusCode::SUCCESS;
}


StatusCode PerformanceWriterTool::finalize() {
  if (!m_noDiagnostics) {
    // fill residual and pull details into additional hists
    m_resPlotTool.refinement(m_resPlotCache);
    if (m_outputFile) {
      m_outputFile->cd();
      m_resPlotTool.write(m_resPlotCache);
      m_effPlotTool.write(m_effPlotCache);
      m_summaryPlotTool.write(m_summaryPlotCache);
      ATH_MSG_VERBOSE("Wrote performance plots to '" << m_outputFile->GetPath() << "'");
    }

    m_resPlotTool.clear(m_resPlotCache);
    m_effPlotTool.clear(m_effPlotCache);
    m_summaryPlotTool.clear(m_summaryPlotCache);
    if (m_outputFile) {
      m_outputFile->Close();
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode PerformanceWriterTool::write(const Acts::GeometryContext& geoContext, const TrajectoriesContainer& trajectories) {
  const EventContext& ctx = Gaudi::Hive::currentContext();

  SG::ReadHandle<McEventCollection> mcEvents {m_mcEventCollectionKey, ctx};
  ATH_CHECK(mcEvents.isValid());
  if (mcEvents->size() != 1) {
    ATH_MSG_ERROR("There should be exactly one event in the McEventCollection.");
    return StatusCode::FAILURE;
  }

  std::map<int, const HepMC::GenParticle*> particles {};
  for (const HepMC::GenParticle* particle : mcEvents->front()->particle_range()) {
    particles[particle->barcode()] = particle;
  }

  SG::ReadHandle<TrackerSimDataCollection> simData {m_simDataCollectionKey, ctx};
  ATH_CHECK(simData.isValid());

  // Truth particles with corresponding reconstructed tracks
  std::vector<int> reconParticleIds;
  reconParticleIds.reserve(particles.size());
  // For each particle within a track, how many hits did it contribute
  std::vector<ParticleHitCount> particleHitCounts;


  // Loop over all trajectories
  for (size_t itraj = 0; itraj < trajectories.size(); ++itraj) {
    const auto& traj = trajectories[itraj];

    if (traj.empty()) {
      ATH_MSG_WARNING("Empty trajectories object " << itraj);
      continue;
    }

    // The trajectory entry indices and the multiTrajectory
    const auto& trackTips = traj.tips();
    const auto& mj = traj.multiTrajectory();

    // Check the size of the trajectory entry indices. For track fitting, there
    // should be at most one trajectory
    if (trackTips.size() > 1) {
      ATH_MSG_ERROR("Track fitting should not result in multiple trajectories.");
      return StatusCode::FAILURE;
    }
    // Get the entry index for the single trajectory
    auto trackTip = trackTips.front();

    // Select reco track with fitted parameters
    if (not traj.hasTrackParameters(trackTip)) {
      ATH_MSG_WARNING("No fitted track parameters.");
      continue;
    }
    const auto& fittedParameters = traj.trackParameters(trackTip);

    // Get the majority truth particle for this trajectory
    identifyContributingParticles(*simData, traj, trackTip, particleHitCounts);
    if (particleHitCounts.empty()) {
      ATH_MSG_WARNING("No truth particle associated with this trajectory.");
      continue;
    }
    // Find the truth particle for the majority barcode
    const auto ip = particles.find(particleHitCounts.front().particleId);
    if (ip == particles.end()) {
      ATH_MSG_WARNING("Majority particle not found in the particles collection.");
      continue;
    }

    // Record this majority particle ID of this trajectory
    reconParticleIds.push_back(ip->first);
    const HepMC::GenParticle* truthParticle = ip->second;
    std::unique_ptr<const Acts::BoundTrackParameters> truthParameters
        = extrapolateToReferenceSurface(ctx, truthParticle);
    // Fill the residual plots
    if (truthParameters) {
      m_resPlotTool.fill(m_resPlotCache, geoContext, std::move(truthParameters), fittedParameters);
    } else {
      ATH_MSG_WARNING("Can not extrapolate truth parameters to reference surface.");
    }
    // Collect the trajectory summary info
    auto trajState = Acts::MultiTrajectoryHelpers::trajectoryState(mj, trackTip);
    // Fill the trajectory summary info
    m_summaryPlotTool.fill(m_summaryPlotCache, fittedParameters, trajState.nStates, trajState.nMeasurements,
                           trajState.nOutliers, trajState.nHoles, trajState.nSharedHits);
  }

  // Fill the efficiency, defined as the ratio between number of tracks with fitted parameter and total truth tracks
  // (assumes one truth partilce has one truth track)
  for (const auto& particle : particles) {
    bool isReconstructed = false;
    // Find if the particle has been reconstructed
    auto it = std::find(reconParticleIds.begin(), reconParticleIds.end(), particle.first);
    if (it != reconParticleIds.end()) {
      isReconstructed = true;
    }
    m_effPlotTool.fill(m_effPlotCache, particle.second, isReconstructed);
  }

  return StatusCode::SUCCESS;
}

std::unique_ptr<const Acts::BoundTrackParameters> PerformanceWriterTool::extrapolateToReferenceSurface(
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
  Acts::BoundTrackParameters startParameters(std::move(startSurface), params, charge);
  std::unique_ptr<const Acts::BoundTrackParameters> targetParameters =
      m_extrapolationTool->propagate(ctx, startParameters, *targetSurface);
  return targetParameters;
}