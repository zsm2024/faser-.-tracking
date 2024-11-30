/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASERUSERACTIONS_CALYPSODEBUGSTACKINGACTION_H
#define G4FASERUSERACTIONS_CALYPSODEBUGSTACKINGACTION_H

// Base class
#include "CalypsoStackingAction.h"


namespace G4UA
{

  /// @class CalypsoDebugStackingAction
  /// @brief Debug version of the CalypsoStackingAction
  /// used for validation of Russian Roulette algorimts.
  /// It can ensure that there is no randomization in
  /// simulation caused by turning the Russian Roulette
  /// on or off.
  ///
  /// @author Miha Muskinja <Miha.Muskinja@cern.ch>
  ///
  class CalypsoDebugStackingAction : public CalypsoStackingAction
  {

    public:

      /// Constructor with configuration
      CalypsoDebugStackingAction(const Config& config);

      /// @brief Classify a new track.
      /// Result can be fUrgent, fWaiting, fPostpone, or fKill.
      virtual G4ClassificationOfNewTrack
      ClassifyNewTrack(const G4Track* track) override final;

  }; // class CalypsoDebugStackingAction

} // namespace G4UA

#endif
