/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Elmar Ritsch
 * @date October 2016
 * @brief A generic particle filter tool for HepMC::GenParticle types
 */

#ifndef FASERISF_HEPMC_FASERGENPARTICLEGENERICFILTER_H
#define FASERISF_HEPMC_FASERGENPARTICLEGENERICFILTER_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_HepMC_Interfaces/IGenParticleFilter.h"

namespace ISF {

// ISF forward declarations
class FaserISFParticle;

/// used to store a list of PDG particle codes
typedef std::vector<int>      PDGCodes;


/**
 * @class FaserGenParticleGenericFilter
 * @headerfile FaserGenParticleGenericFilter.h
 *
 * @brief Core Athena algorithm for the Integrated Simulation Framework
 *
 * This GenParticle filter provides a general way of selecting/filtering out particles
 * during GenEvent read-in.
 */
  class FaserGenParticleGenericFilter : public extends<AthAlgTool, IGenParticleFilter> {

  public:
    /// Constructor with framework parameters
    FaserGenParticleGenericFilter( const std::string& t, const std::string& n, const IInterface* p );

    /// Empty Destructor
    ~FaserGenParticleGenericFilter(){}

    /// Athena algtool's Hooks
    StatusCode  initialize();
    StatusCode  finalize();

    /// Interface method that returns whether the given particle passes all cuts or not
#ifdef HEPMC3
    bool pass(const HepMC::ConstGenParticlePtr& particle) const;
#else
    bool pass(const HepMC::GenParticle& particle) const;
#endif

  private:
    /// Check whether the given particle passes all configure cuts or not
#ifdef HEPMC3
    bool check_cuts_passed(const HepMC::ConstGenParticlePtr& particle) const;
#else
    bool check_cuts_passed(const HepMC::GenParticle& particle) const;
#endif

    /// the cuts defined by the use
    double        m_minTheta;   //!< min polar angle
    double        m_maxTheta;   //!< max polar angle
    double        m_minEnergy;  //!< min kinetic energy cut
    double        m_maxEnergy;  //!< max kinetic energy cut
    PDGCodes      m_pdgs;       //!< list of accepted particle PDG IDs (any accepted if empty)

    /// Geometrical region (=transverse distance from z-axis) within which this filter is applicable
    double        m_maxTransverseDistance;
    /// Range along z-axis that filter applies
    double        m_minLongitudinalPosition;
    double        m_maxLongitudinalPosition;
};

} // ISF namespace


#endif //> !FASERISF_HEPMC_FASERGENPARTICLEGENERICFILTER_H
