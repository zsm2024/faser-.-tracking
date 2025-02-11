/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IFaserTruthSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef FASERISF_INTERFACES_IFASERTRUTHSVC_H
#define FASERISF_INTERFACES_IFASERTRUTHSVC_H 1

// framework includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

// DetectorDescription
#include "FaserDetDescr/FaserRegion.h"

// forward declarations
class G4Step;

namespace ISF {

  class IFaserTruthIncident;
    
  /** @ class IFaserTruthSvc
  
      Interface for the central truth record service to be used by
      the different simulation services.
  
      @ author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
     */
    class IFaserTruthSvc : virtual public IInterface { 

      public: 
        /// Creates the InterfaceID and interfaceID() method
        DeclareInterfaceID(IFaserTruthSvc, 1, 0);
      
        /** virtual desctructor */
        virtual ~IFaserTruthSvc() { }

        /** Register a truth incident */
        virtual void registerTruthIncident( IFaserTruthIncident& truthincident, bool saveAllChildren = false) const = 0;
        
        /** Initialize the Truth Svc at the beginning of each event */
        virtual StatusCode initializeTruthCollection() = 0;
        
        /** Finalize the Truth Svc at the end of each event*/
        virtual StatusCode releaseEvent() = 0;
  }; 
}

#endif //> !FASERISF_INTERFACES_IFASERTRUTHSVC_H

