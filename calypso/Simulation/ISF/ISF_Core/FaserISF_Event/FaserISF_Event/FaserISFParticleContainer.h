/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserISFParticleContainer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
#ifndef FASERISF_EVENT_FASERISFPARTICLECONTAINER_H
#define FASERISF_EVENT_FASERISFPARTICLECONTAINER_H

// STL includes
#include <list>

// forward declarations
namespace ISF {
  class FaserISFParticle;
}

namespace ISF {
  /** generic ISFParticle container (not necessarily a std::list!) */
  typedef std::list<ISF::FaserISFParticle*>                FaserISFParticleContainer;
  typedef std::list<const ISF::FaserISFParticle*>          ConstFaserISFParticleContainer;
}

#endif // FASERISF_EVENT_FASERISFPARTICLECONTAINER_H

