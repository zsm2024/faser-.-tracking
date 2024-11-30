/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASERUSERACTIONS_CALYPSOSTACKINGACTIONTOOL_H
#define G4FASERUSERACTIONS_CALYPSOSTACKINGACTIONTOOL_H

// STL includes
#include <string>

// G4Atlas includes
#include "G4AtlasInterfaces/IUserActionTool.h"
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "CalypsoStackingAction.h"


namespace G4UA
{

  /// @class CalypsoStackingActionTool
  /// @brief Tool which manages the CalypsoStackingAction
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class CalypsoStackingActionTool : public UserActionToolBase<CalypsoStackingAction>
  {

    public:

      /// Standard constructor
      CalypsoStackingActionTool(const std::string& type, const std::string& name,
                               const IInterface* parent);

      /// Initialize the tool
      virtual StatusCode initialize() override final;

    protected:

      // Setup the user action for current thread
      virtual std::unique_ptr<CalypsoStackingAction>
      makeAndFillAction(G4AtlasUserActions& actionLists) override final;

    private:

      /// Configuration parameters
      CalypsoStackingAction::Config m_config;

      /// Use the debug version of the stacking action
      bool m_useDebugAction;

  }; // class CalypsoStackingActionTool

} // namespace G4UA

#endif
