/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IFaserTruthStrategy.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASERISF_HEPMC_INTERFACES_IFASERTRUTHSTRATEGY_H
#define FASERISF_HEPMC_INTERFACES_IFASERTRUTHSTRATEGY_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
 
// forward declarations

#include "AtlasHepMC/GenParticle_fwd.h"

namespace ISF {
  class IFaserTruthIncident;

  /**
   @class IFaserTruthStrategy
      
   @todo : class description
       
   @author Elmar.Ritsch -at- cern.ch
   */
     
  class IFaserTruthStrategy : virtual public IAlgTool {
    public:
       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(IFaserTruthStrategy, 1, 0);
      
      /** true if the IFaserTruthStrategy implementation applies to the given IFaserTruthIncident */
      virtual bool pass( IFaserTruthIncident& incident) const = 0;
      /** returns true if this truth strategy should be applied to the
          region indicated by geoID*/
      virtual bool appliesToRegion(unsigned short geoID) const = 0;
  };

} // end of namespace

#endif // ISF_HEPMC_INTERFACES_ITRUTHSTRATEGY_H
