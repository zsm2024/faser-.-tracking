/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticlePositionFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASERISF_HEPMC_FASERGENPARTICLEPOSITIONFILTER_H
#define FASERISF_HEPMC_FASERGENPARTICLEPOSITIONFILTER_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// ISF includes
#include "ISF_HepMC_Interfaces/IGenParticleFilter.h"

// ISF includes
#include "FaserISF_Interfaces/IFaserGeoIDSvc.h"

namespace ISF {

  /** @class FaserGenParticlePositionFilter
  
      Particle filter by position, to be used for initial GenEvent read-in.
  
      @author Andreas.Salzburger -at- cern.ch
     */
  class FaserGenParticlePositionFilter : public extends<AthAlgTool, IGenParticleFilter> { 
      
    public: 
      //** Constructor with parameters */
      FaserGenParticlePositionFilter( const std::string& t, const std::string& n, const IInterface* p );
      
      /** Destructor */
      ~FaserGenParticlePositionFilter(){}

      /** Athena algtool's Hooks */
      StatusCode  initialize();
      StatusCode  finalize();

      /** does the given particle pass the filter? */
#ifdef HEPMC3
      bool pass(const HepMC::ConstGenParticlePtr& particle) const;
#else
      bool pass(const HepMC::GenParticle& particle) const;
#endif 
	  
	private:
      ServiceHandle<IFaserGeoIDSvc>     m_geoIDSvc;
      std::vector<int>                  m_checkRegion;
  }; 
  
}


#endif //> !FASERISF_HEPMC_FASERGENPARTICLEPOSITIONFILTER_H
