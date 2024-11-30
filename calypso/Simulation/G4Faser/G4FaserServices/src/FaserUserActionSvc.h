/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef G4FASERSERVICES__G4UA_FASERUSERACTIONSVC_H
#define G4FASERSERVICES__G4UA_FASERUSERACTIONSVC_H


// System includes
#include <thread>

// Framework includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"

// G4Atlas includes
#include "G4AtlasRunAction.h"
#include "G4AtlasEventAction.h"
#include "G4AtlasStackingAction.h"
#include "G4AtlasTrackingAction.h"
#include "G4AtlasSteppingAction.h"
#include "G4AtlasInterfaces/IUserActionTool.h"
#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasTools/ThreadActionHolder.h"


namespace G4UA
{

  /// @class FaserUserActionSvc
  /// @brief A service which manages the user actions for G4 simulation
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class FaserUserActionSvc : public extends<AthService, IUserActionSvc>
  {

    public:

      /// Standard constructor
      FaserUserActionSvc(const std::string& name, ISvcLocator* pSvcLocator);

      /// Initialize the service
      StatusCode initialize() override;

      /// Initialize the user actions for the current thread
      StatusCode initializeActions() override final;

      StatusCode getSecondaryActions( std::vector< G4UserSteppingAction* >& actions ) override final;

      StatusCode addActionTool(const ToolHandle<IUserActionTool>& service_tool) override final;
    private:

      /// @name Handles to ATLAS action tools
      /// @{

      /// User action tools
      ToolHandleArray<IUserActionTool> m_userActionTools;

      /// @}

      /// @name ATLAS Geant4 user actions
      /// @{

      /// Thread-local run action
      ThreadActionHolder<G4AtlasRunAction> m_runActions;
      /// Thread-local event action
      ThreadActionHolder<G4AtlasEventAction> m_eventActions;
      /// Thread-local stacking action
      ThreadActionHolder<G4AtlasStackingAction> m_stackingActions;
      /// Thread-local tracking action
      ThreadActionHolder<G4AtlasTrackingAction> m_trackingActions;
      /// Thread-local stepping action
      ThreadActionHolder<G4AtlasSteppingAction> m_steppingActions;

      /// @}

  }; // class FaserUserActionSvc

}

#endif

