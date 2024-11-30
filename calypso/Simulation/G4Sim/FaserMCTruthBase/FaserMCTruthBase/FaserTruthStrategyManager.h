/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FaserMCTruthBase_FaserTruthStrategyManager_H
#define FaserMCTruthBase_FaserTruthStrategyManager_H

// ISF include
#include "FaserISF_Interfaces/IFaserTruthSvc.h"

/// Forward declarations
class G4Step;


/// @brief Singleton class for creating truth incidents.
/// This class is gradually being refactored out of existence.

class FaserTruthStrategyManager
{

public:

  /// Retrieve the singleton instance
  static FaserTruthStrategyManager* GetStrategyManager();

  /// Returns true if any of the truth strategies return true
  bool CreateTruthIncident(const G4Step*, int subDetVolLevel) const;

  /// Define which ISF TruthService to use
  void SetISFTruthSvc(ISF::IFaserTruthSvc *truthSvc);

private:
  FaserTruthStrategyManager();
  FaserTruthStrategyManager(const FaserTruthStrategyManager&) = delete;
  FaserTruthStrategyManager& operator=(const FaserTruthStrategyManager&) = delete;

  /// ISF Services the TruthStrategyManager talks to
  ISF::IFaserTruthSvc* m_truthSvc;
};

#endif
