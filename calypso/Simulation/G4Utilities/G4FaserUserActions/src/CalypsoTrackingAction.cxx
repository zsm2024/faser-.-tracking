/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CalypsoTrackingAction.h"

#include <iostream>

#include "G4Event.hh"
#include "G4EventManager.hh"

#include "FaserMCTruth/FaserG4EventUserInfo.h"
#include "FaserMCTruth/FaserPrimaryParticleInformation.h"
#include "FaserMCTruth/FaserTrackHelper.h"
#include "FaserMCTruth/FaserTrackInformation.h"
#include "FaserMCTruthBase/FaserTrajectory.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  CalypsoTrackingAction::CalypsoTrackingAction(MSG::Level lvl,
                                             int secondarySavingLevel, int subDetVolLevel)
    : AthMessaging("CalypsoTrackingAction")
    , m_secondarySavingLevel(secondarySavingLevel)
    , m_subDetVolLevel(subDetVolLevel)
  {
    setLevel(lvl);
  }

  //---------------------------------------------------------------------------
  // Pre-tracking action.
  //---------------------------------------------------------------------------
  void CalypsoTrackingAction::PreUserTrackingAction(const G4Track* track)
  {
    ATH_MSG_DEBUG("Starting to track a new particle");

    // Use the TrackHelper code to identify the kind of particle.
    FaserTrackHelper trackHelper(track);

    // Condition for storing the GenParticle in the EventInformation for later.
    if (trackHelper.IsPrimary() || trackHelper.IsRegisteredSecondary())
    {
      // Why a const_cast???
      // This is an ugly way to communicate the GenParticle...
      HepMC::GenParticlePtr part = trackHelper.GetTrackInformation()->GetHepMCParticle();

      // Assign the GenParticle to the EventInformation.
      FaserG4EventUserInfo* eventInfo = static_cast<FaserG4EventUserInfo*>
        (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());
      if (trackHelper.IsPrimary()) eventInfo->SetCurrentPrimary(part);
      eventInfo->SetCurrentlyTraced(part);
    }

    // Condition for creating a trajectory object to store truth.
    if (trackHelper.IsPrimary() ||
        (trackHelper.IsRegisteredSecondary() && m_secondarySavingLevel>1) ||
        (trackHelper.IsSecondary() && m_secondarySavingLevel>2))
    {
      ATH_MSG_DEBUG("Preparing a FaserTrajectory for saving truth");

      // Create a new AtlasTrajectory for this particle
      FaserTrajectory* trajectory = new FaserTrajectory(track, m_subDetVolLevel);

      // Assign the trajectory to the tracking manager.
      // TODO: consider caching the tracking manager once to reduce overhead.
      auto trkMgr = G4EventManager::GetEventManager()->GetTrackingManager();
      //trajectory->setTrackingManager(trkMgr);
      trkMgr->SetStoreTrajectory(true);
      trkMgr->SetTrajectory(trajectory);
    }
  }

  //---------------------------------------------------------------------------
  // Post-tracking action.
  //---------------------------------------------------------------------------
  void CalypsoTrackingAction::PostUserTrackingAction(const G4Track* /*track*/)
  {
    ATH_MSG_DEBUG("Finished tracking a particle");

    // We are done tracking this particle, so reset the trajectory.
    // TODO: consider caching the tracking manager once to reduce overhead.
    G4EventManager::GetEventManager()->GetTrackingManager()->
      SetStoreTrajectory(false);
  }

} // namespace G4UA
