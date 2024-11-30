/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IFaserInputConverter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASERISF_INTERFACES_IFASERINPUTCONVERTER_H
#define FASERISF_INTERFACES_IFASERINPUTCONVERTER_H 1

// Gaudi
#include "GaudiKernel/IInterface.h"

//GeneratorObjects
#include "GeneratorObjects/HepMcParticleLink.h"

// StoreGate
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

// Simulation includes
#include "FaserISF_Event/FaserISFParticleContainer.h"

// forward declarations
class McEventCollection;
class G4Event;
#include "AtlasHepMC/GenEvent_fwd.h"

namespace ISF {

  class FaserISFParticle;

  /**
     @class IFaserInputConverter

     Interface to Athena service that converts an input McEventCollection
     into a container of FaserISFParticles.

     @author Elmar.Ritsch -at- cern.ch
  */

  class IFaserInputConverter : virtual public IInterface {
  public:

    /** Virtual destructor */
    virtual ~IFaserInputConverter(){}

    /** Tell Gaudi which InterfaceID we have */
    DeclareInterfaceID( ISF::IFaserInputConverter, 1, 0 );

    /** Convert selected particles from the given McEventCollection into ISFParticles
        and push them into the given ISFParticleContainer */
    virtual StatusCode convert(McEventCollection& inputGenEvents,
                               FaserISFParticleContainer& simParticles,
                               EBC_EVCOLL kindOfCollection=EBC_MAINEVCOLL) const = 0;

    /** Convert selected particles from the given McEventCollection into G4PrimaryParticles
        and push them into the given G4Event */
    virtual StatusCode convertHepMCToG4Event(McEventCollection& inputGenEvents,
                                             G4Event*& outputG4Event, McEventCollection& shadowGenEvents,
                                             EBC_EVCOLL kindOfCollection=EBC_MAINEVCOLL) const = 0;

    virtual StatusCode convertHepMCToG4EventLegacy(McEventCollection& inputGenEvents,
                                             G4Event*& outputG4Event, 
                                             EBC_EVCOLL kindOfCollection=EBC_MAINEVCOLL) const = 0;

    /** Converts vector of ISF::ISFParticles to G4Event */
    virtual G4Event* ISF_to_G4Event(const std::vector<ISF::FaserISFParticle*>& isp, HepMC::GenEvent *genEvent, HepMC::GenEvent *shadowGenEvent=nullptr, bool useHepMC=false) const = 0;

  };

} // end of ISF namespace

#endif // FASERISF_INTERFACES_IFASERINPUTCONVERTER_H
