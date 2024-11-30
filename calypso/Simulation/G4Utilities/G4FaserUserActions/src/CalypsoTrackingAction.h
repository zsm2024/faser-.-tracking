/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FaserUserActions_CalypsoTrackingAction_H
#define G4FaserUserActions_CalypsoTrackingAction_H

#include "AthenaBaseComps/AthMessaging.h"

#include "G4UserTrackingAction.hh"

namespace G4UA
{

  /// @class CalypsoTrackingAction
  /// @brief User action for pre/post tracking truth handling.
  ///
  class CalypsoTrackingAction : public G4UserTrackingAction, public AthMessaging
  {

    public:

      /// Constructor
      CalypsoTrackingAction(MSG::Level lvl, int secondarySavingLevel, int subDetVolLevel);

      /// @brief Called before tracking a new particle.
      ///
      /// If the track meets certain conditions, we save it in the
      /// EventInformation and possibly construct a new AtlasTrajectory
      /// which will be used for writing out truth particles later.
      virtual void PreUserTrackingAction(const G4Track*) override final;

      /// @brief Called after tracking a particle.
      ///
      /// Here we reset the AtlasTrajectory if it was created.
      virtual void PostUserTrackingAction(const G4Track*) override final;

    private:

      /// The saving level for secondaries.
      int m_secondarySavingLevel;
      /// The level in the G4 volume hierarchy at which can we find the sub-detector name
      int m_subDetVolLevel;

  }; // class CalypsoTrackingAction

} // namespace G4UA

#endif
