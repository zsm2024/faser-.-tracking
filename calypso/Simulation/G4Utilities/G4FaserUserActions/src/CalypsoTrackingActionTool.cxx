/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CalypsoTrackingActionTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  CalypsoTrackingActionTool::
  CalypsoTrackingActionTool(const std::string& type, const std::string& name,
                           const IInterface* parent)
    : UserActionToolBase<CalypsoTrackingAction>(type, name, parent)
    , m_secondarySavingLevel(2)
    , m_subDetVolLevel(1)
  {
    declareProperty("SecondarySavingLevel", m_secondarySavingLevel,
      "Three valid options: 1 - Primaries; 2 - StoredSecondaries(default); 3 - All");
    declareProperty("SubDetVolumeLevel", m_subDetVolLevel,
      "The level in the G4 volume hierarchy at which can we find the sub-detector name");
  }

  //---------------------------------------------------------------------------
  // Initialize - temporarily here for debugging
  //---------------------------------------------------------------------------
  StatusCode CalypsoTrackingActionTool::initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<CalypsoTrackingAction>
  CalypsoTrackingActionTool::makeAndFillAction(G4AtlasUserActions& actionLists)
  {
    ATH_MSG_DEBUG("Constructing a CalypsoTrackingAction");
    // Create and configure the action plugin.
    auto action = std::make_unique<CalypsoTrackingAction>(
        msg().level(), m_secondarySavingLevel, m_subDetVolLevel );
    actionLists.trackingActions.push_back( action.get() );
    return action;
  }

}
