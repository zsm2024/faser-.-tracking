/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FaserUserActions_CalypsoTrackingActionTool_H
#define G4FaserUserActions_CalypsoTrackingActionTool_H

// G4Atlas includes
#include "G4AtlasInterfaces/IUserActionTool.h"
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "CalypsoTrackingAction.h"

namespace G4UA
{

  /// @class CalypsoTrackingActionTool
  /// @brief Tool which manages the CalypsoTrackingAction
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class CalypsoTrackingActionTool : public UserActionToolBase<CalypsoTrackingAction>
  {

    public:

      /// Standard constructor
      CalypsoTrackingActionTool(const std::string& type, const std::string& name,
                               const IInterface* parent);

      /// Initialize the tool (just for debugging printout)
      virtual StatusCode initialize() override;

    protected:

      /// Setup the user action for current thread
      virtual std::unique_ptr<CalypsoTrackingAction>
      makeAndFillAction(G4AtlasUserActions& actionLists) override final;

    private:

      /// The saving level for secondaries.
      int m_secondarySavingLevel;
      /// The level in the G4 volume hierarchy at which can we find the sub-detector name
      int m_subDetVolLevel;

  }; // class CalypsoTrackingActionTool

} // namespace G4UA

#endif
