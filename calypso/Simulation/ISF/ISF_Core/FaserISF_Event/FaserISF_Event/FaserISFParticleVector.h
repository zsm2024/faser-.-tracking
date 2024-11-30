/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserISFParticleVector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
#ifndef FASERISF_EVENT_FASERISFPARTICLEVECTOR_H
#define FASERISF_EVENT_FASERISFPARTICLEVECTOR_H

// STL includes
#include <vector>

// forward declarations
namespace ISF {
  class FaserISFParticle;
}

namespace ISF {
  /** ISFParticle vector */
  typedef std::vector<ISF::FaserISFParticle *>             FaserISFParticleVector;
  typedef std::vector<const ISF::FaserISFParticle *>       ConstFaserISFParticleVector;
}

#endif // FASERISF_EVENT_FASERISFPARTICLEVECTOR_H

