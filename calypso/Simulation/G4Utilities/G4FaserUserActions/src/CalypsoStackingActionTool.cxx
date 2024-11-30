/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CalypsoDebugStackingAction.h"
#include "CalypsoStackingActionTool.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  CalypsoStackingActionTool::
  CalypsoStackingActionTool(const std::string& type, const std::string& name,
                           const IInterface* parent)
    : UserActionToolBase<CalypsoStackingAction>(type, name, parent),
      m_config { /*killAllNeutrinos*/ false,
                 /*photonEnergyCut*/ -1.,
                //  /*applyNRR*/                      false,
                //  /*russianRouletteNeutronThreshold*/ -1.,
                //  /*russianRouletteNeutronWeight*/    -1.,
                //  /*applyPRR*/                      false,
                //  /*russianRoulettePhotonThreshold*/  -1.,
                //  /*russianRoulettePhotonWeight*/     -1.,
                 /*isISFJob*/ false
      },
      m_useDebugAction(false)
  {
    declareProperty("KillAllNeutrinos", m_config.killAllNeutrinos,
                    "Toggle killing of all neutrinos");
    declareProperty("PhotonEnergyCut", m_config.photonEnergyCut,
                    "Energy threshold for tracking photons");
    // declareProperty("ApplyNRR", m_config.applyNRR,
    //                 "Apply the Neutron Russian Roulette");
    // declareProperty("NRRThreshold", m_config.russianRouletteNeutronThreshold,
    //                 "Energy threshold for the Neutron Russian Roulette");
    // declareProperty("NRRWeight", m_config.russianRouletteNeutronWeight,
    //                 "Weight for the Neutron Russian Roulette");
    // declareProperty("ApplyPRR", m_config.applyPRR,
    //                 "Apply the Photon Russian Roulette");
    // declareProperty("PRRThreshold", m_config.russianRoulettePhotonThreshold,
    //                 "Energy threshold for the Photon Russian Roulette");
    // declareProperty("PRRWeight", m_config.russianRoulettePhotonWeight,
    //                 "Weight for the Photon Russian Roulette");
    declareProperty("IsISFJob", m_config.isISFJob, "");
    declareProperty("UseDebugAction", m_useDebugAction);
  }

  //---------------------------------------------------------------------------
  // Initialize
  //---------------------------------------------------------------------------
  StatusCode CalypsoStackingActionTool::initialize()
  {
    ATH_MSG_DEBUG( "Initializing " << name() );
    ATH_MSG_DEBUG( "KillAllNeutrinos: " << m_config.killAllNeutrinos );
    ATH_MSG_DEBUG( "PhotonEnergyCut: " << m_config.photonEnergyCut );
    // ATH_MSG_DEBUG( "RussianRouletteNeutronThreshold: " << m_config.russianRouletteNeutronThreshold );
    // ATH_MSG_DEBUG( "RussianRouletteNeutronWeight: " << m_config.russianRouletteNeutronWeight );
    // ATH_MSG_DEBUG( "RussianRoulettePhotonThreshold: " << m_config.russianRoulettePhotonThreshold );
    // ATH_MSG_DEBUG( "RussianRoulettePhotonWeight: " << m_config.russianRoulettePhotonWeight );
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Create the action on request
  //---------------------------------------------------------------------------
  std::unique_ptr<CalypsoStackingAction>
  CalypsoStackingActionTool::makeAndFillAction(G4AtlasUserActions& actionLists)
  {
    ATH_MSG_DEBUG("Creating a CalypsoStackingAction");
    // Create and configure the action plugin.
    std::unique_ptr<CalypsoStackingAction> action{nullptr};
    if (m_useDebugAction) {
      action = std::make_unique<CalypsoDebugStackingAction>(m_config);
    } else {
      action = std::make_unique<CalypsoStackingAction>(m_config);
    }
    actionLists.stackingActions.push_back( action.get() );
    return action;
  }

}
