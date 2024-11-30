/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @author Elmar Ritsch
 * @date October 2016
 * @brief A generic particle filter tool for HepMC::GenParticle types
 */


// class header include
#include "FaserGenParticleGenericFilter.h"

// HepMC includes
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/SimpleVector.h"

// STL includes
#include <limits>
#include <algorithm>

/** Constructor **/
ISF::FaserGenParticleGenericFilter::FaserGenParticleGenericFilter( const std::string& t,
                                                                   const std::string& n,
                                                                   const IInterface* p )
  : base_class(t,n,p),
    m_minTheta(0),
    m_maxTheta(M_PI),
    m_minEnergy(std::numeric_limits<decltype(m_minEnergy)>::lowest()),
    m_maxEnergy(std::numeric_limits<decltype(m_maxEnergy)>::max()),
    m_pdgs(),
    m_maxTransverseDistance(std::numeric_limits<decltype(m_maxTransverseDistance)>::max()),
    m_minLongitudinalPosition(std::numeric_limits<decltype(m_minLongitudinalPosition)>::max()),
    m_maxLongitudinalPosition(std::numeric_limits<decltype(m_maxLongitudinalPosition)>::max())
{
    // different cut parameters
    declareProperty("MinTheta",
                    m_minTheta,
                    "Minimum Particle Polar Angle");
    declareProperty("MaxTheta",
                    m_maxTheta,
                    "Maximum Particle Polar Angle");
    declareProperty("MinEnergy",
                    m_minEnergy,
                    "Minimum Particle Kinetic Energy");
    declareProperty("MaxEnergy",
                    m_maxEnergy,
                    "Maximum Particle Kinetic Energy");
    declareProperty("ParticlePDG",
                    m_pdgs,
                    "List of accepted particle PDG IDs (any accepted if empty)");
    declareProperty("MaxTransverseDistance",
                    m_maxTransverseDistance,
                    "Only particles with std::max(ProductionVertex[0],ProductionVertex[1])<MaxTransverseDistance may get filtered out");
    declareProperty("MinLongitudinalPosition",
                    m_minLongitudinalPosition,
                    "Minimum value of ProductionVertex[2] for which cut applies");
    declareProperty("MaxLongitudinalPosition",
                    m_maxLongitudinalPosition,
                    "Maximum value of ProductionVertex[2] for which cut applies");
}


/** Athena algtool's Hooks */
StatusCode  ISF::FaserGenParticleGenericFilter::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    ATH_MSG_VERBOSE("initialize() successful");
    return StatusCode::SUCCESS;
}


/** Athena algtool's Hooks */
StatusCode  ISF::FaserGenParticleGenericFilter::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_VERBOSE("finalize() successful");
    return StatusCode::SUCCESS;
}


/** Returns whether the given particle passes all cuts or not */
#ifdef HEPMC3
bool ISF::FaserGenParticleGenericFilter::pass(const HepMC::ConstGenParticlePtr& particle) const
{
  bool pass = true;
  const HepMC::ConstGenVertexPtr productionVertex = particle?particle->production_vertex():nullptr;
  const auto* position = productionVertex ? &productionVertex->position() : nullptr;

  if (!position || 
    (std::max(position->x(),position->y()) <= m_maxTransverseDistance && 
     position->z()>= m_minLongitudinalPosition && 
     position->z()<= m_maxLongitudinalPosition)) {
    pass = check_cuts_passed(particle);
  }

  const auto momentum = particle->momentum();
  ATH_MSG_VERBOSE( "GenParticle '" << particle << "' with "
                   << (position ? "pos: (" + std::to_string(position->x()) + "," + std::to_string(position->y()) + "," + std::to_string(position->z()) + ")" : "")
                   << ", mom: Ek = " << std::max(0.0,momentum.e() - momentum.m()) << ", theta = " << momentum.theta() 
                   << " did " << (pass ? "" : "NOT ")
                   << "pass the cuts.");
  return pass;
}
#else
bool ISF::FaserGenParticleGenericFilter::pass(const HepMC::GenParticle& particle) const
{
  bool pass = true;
  HepMC::ConstGenVertexPtr productionVertex = particle.production_vertex();
  const auto* position = productionVertex ? &productionVertex->position() : nullptr;

  if (!position || 
    (std::max(position->x(),position->y()) <= m_maxTransverseDistance && 
     position->z()>= m_minLongitudinalPosition && 
     position->z()<= m_maxLongitudinalPosition)) {
    pass = check_cuts_passed(particle);
  }

  const auto momentum = particle.momentum();
  ATH_MSG_VERBOSE( "GenParticle '" << particle << "' with "
                   << (productionVertex ? "pos: r=" + std::to_string(productionVertex->position().perp()) : "")
                   << ", mom: eta=" << momentum.eta() << " phi=" << momentum.phi()
                   << " did " << (pass ? "" : "NOT ")
                   << "pass the cuts.");
  return pass;
}
#endif


/** Check whether the given particle passes all configure cuts or not */
#ifdef HEPMC3
bool ISF::FaserGenParticleGenericFilter::check_cuts_passed(const HepMC::ConstGenParticlePtr& particle) const {
  const auto momentum = particle?particle->momentum():HepMC::FourVector(0,0,0,0);
  int pdg = particle?particle->pdg_id():0;
#else
bool ISF::FaserGenParticleGenericFilter::check_cuts_passed(const HepMC::GenParticle &particle) const {
  const auto& momentum = particle.momentum();
  int pdg = particle.pdg_id();
#endif

  double ek = std::max(0.0, momentum.e() - momentum.m());
  double theta = momentum.theta();

  // check the particle pdg code
  if( m_pdgs.size() && std::find(std::begin(m_pdgs), std::end(m_pdgs), pdg) == std::end(m_pdgs) ) {
    return false;
  }

  // check the momentum cuts
  if (ek < m_minEnergy) {
    return false;
  }
  if (ek > m_maxEnergy) {
    return false;
  }

  // check the theta cuts
  if (theta<m_minTheta || theta>m_maxTheta) {
    return false;
  }

  // all cuts passed
  return true;
}
