#include "TruthBasedInitialParameterTool.h"

#include "StoreGate/ReadHandle.h"
#include "HepMC3/GenVertex.h"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Definitions/Units.hpp"
#include <random>
#include <cmath>


using namespace Acts::UnitLiterals;

TruthBasedInitialParameterTool::TruthBasedInitialParameterTool(
    const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent) {}


StatusCode TruthBasedInitialParameterTool::initialize() {
  ATH_MSG_INFO("TruthBasedInitialParameterTool::initialize");
  ATH_CHECK(m_simDataCollectionKey.initialize());
  ATH_CHECK(m_simWriterTool.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode TruthBasedInitialParameterTool::finalize() {
  ATH_MSG_INFO("TruthBasedInitialParameterTool::finalize");
  return StatusCode::SUCCESS;
}


Acts::BoundTrackParameters TruthBasedInitialParameterTool::getInitialParameters(std::vector<Identifier> ids) const {
  SG::ReadHandle<TrackerSimDataCollection> simDataCollection(m_simDataCollectionKey);

  // get simulated vertex and momentum of the HepMcParticle with the highest energy deposit
  float highestDep = 0;
  HepMC3::FourVector vertex;
  HepMC3::FourVector momentum;

  for (Identifier id : ids) {
    if( simDataCollection->count(id) == 0)
      continue;

    const auto& deposits = simDataCollection->at(id).getdeposits();
    for( const auto& depositPair : deposits) {
      if( std::abs(depositPair.first->pdg_id()) == 13 && depositPair.second > highestDep) {
        highestDep = depositPair.second;
        vertex = depositPair.first->production_vertex()->position();
        momentum = depositPair.first->momentum();
      }
    }
  }

  Acts::Vector3 truthVertex = {vertex.x(), vertex.y(), vertex.z()}; // in mm
  Acts::Vector3 truthMomentum = {momentum.x() / 1000, momentum.y() / 1000, momentum.z() / 1000}; // in GeV
  m_simWriterTool->writeout(truthVertex, truthMomentum);

  // smearing parameters
  double sigmaU = 200_um;
  double sigmaV = 200_um;
  double sigmaPhi = 1_degree;
  double sigmaTheta = 1_degree;
  double sigmaP = 0.1 * truthVertex.norm();
  // var(q/p) = (d(1/p)/dp)² * var(p) = (-1/p²)² * var(p)
  double sigmaQOverP = sigmaP / (truthMomentum.norm() * truthMomentum.norm());
  double sigmaT0 = 1_ns;

  // create covariance matrix
  Acts::BoundSquareMatrix cov = Acts::BoundSquareMatrix::Zero();
  cov(Acts::eBoundLoc0, Acts::eBoundLoc1) = sigmaU * sigmaU;
  cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = sigmaV * sigmaV;
  cov(Acts::eBoundPhi, Acts::eBoundPhi) = sigmaPhi * sigmaPhi;
  cov(Acts::eBoundTheta, Acts::eBoundTheta) = sigmaTheta * sigmaTheta;
  cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = sigmaQOverP * sigmaQOverP;
  cov(Acts::eBoundTime, Acts::eBoundTime) = sigmaT0 * sigmaT0;
   
  // smear truth parameters
  std::random_device rd;
  std::default_random_engine rng {rd()};
  std::normal_distribution<> norm; // mu: 0 sigma: 1
  double charge = 1;
  double time =0;
  auto theta = Acts::VectorHelpers::theta(truthMomentum.normalized());
  auto phi = Acts::VectorHelpers::phi(truthMomentum.normalized());

  auto perigee = Acts::Surface::makeShared<const Acts::PerigeeSurface>(truthVertex);
  //@todo make the particle hypothesis configurable
  Acts::ParticleHypothesis particleHypothesis = Acts::ParticleHypothesis::muon();
  
  Acts::BoundVector params = Acts::BoundVector::Zero();
  // smear the position/time
  params[Acts::eBoundLoc0] = sigmaU * norm(rng);
  params[Acts::eBoundLoc1] = sigmaV * norm(rng);
  params[Acts::eBoundTime] = time + sigmaT0 * norm(rng);
  // smear direction angles phi,theta ensuring correct bounds
  const auto [newPhi, newTheta] = Acts::detail::normalizePhiTheta(
      phi + sigmaPhi * norm(rng), theta + sigmaTheta * norm(rng));
  params[Acts::eBoundPhi] = newPhi;
  params[Acts::eBoundTheta] = newTheta;
  // compute smeared absolute momentum vector
  const double newP = std::max(0.0, truthMomentum.norm() + sigmaP * norm(rng));
  params[Acts::eBoundQOverP] = particleHypothesis.qOverP(newP, charge);; 

  
  ATH_MSG_DEBUG("Smeared starting parameters : " << perigee
                            ->localToGlobal(
                                Acts::GeometryContext(),
                                Acts::Vector2{params[Acts::eBoundLoc0],
                                              params[Acts::eBoundLoc1]},
                                truthMomentum)
                            .transpose()
                     << ", " << params[Acts::eBoundTime] << ", "
                     << params[Acts::eBoundPhi] << ", "
                     << params[Acts::eBoundTheta] << ", "
                     << params[Acts::eBoundQOverP]);
 
  return Acts::BoundTrackParameters(perigee, params, cov, particleHypothesis); 
}
