/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASERUSERACTIONS_G4UA_FASERLOOPERKILLER_H
#define G4FASERUSERACTIONS_G4UA_FASERLOOPERKILLER_H

#include "GaudiKernel/ServiceHandle.h"

// Infrastructure includes
#include "G4UserSteppingAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

class StoreGateSvc;

namespace G4UA
{

  /// @class FaserLooperKiller
  /// @brief A user action to kill looping tracks.
  ///
  class FaserLooperKiller final : public G4UserSteppingAction, public AthMessaging
  {

    public:

      struct Config
      {
        int MaxSteps=1000000;
        int PrintSteps=100;
        int VerboseLevel=1;
        bool AbortEvent=true;
        bool SetError=false;
      };

      struct Report
      {
        long killed_tracks=0;
        void merge(const Report& rep){
          killed_tracks+=rep.killed_tracks;
        }
      };

      FaserLooperKiller(const Config& config);

      virtual void UserSteppingAction(const G4Step*) override;
      /// Retrieve  results
      const Report& getReport() const
      { return m_report; }

    private:

      typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
      /// Pointer to StoreGate (event store by default)
      mutable StoreGateSvc_t m_evtStore;
      /// Pointer to StoreGate (detector store by default)
      mutable StoreGateSvc_t m_detStore;

      /// My configuration options
      Config m_config;
      Report m_report;
      int m_count_steps;

  }; // class FaserLooperKiller

}

#endif
