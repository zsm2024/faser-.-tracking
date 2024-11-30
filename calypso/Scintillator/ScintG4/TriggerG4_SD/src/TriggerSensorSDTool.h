/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   Trigger Sensitive Detector Tool
 ****************************************************************/

#ifndef TRIGGERG4_SD_TRIGGERSENSORSDTOOL_H
#define TRIGGERG4_SD_TRIGGERSENSORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

class TriggerSensorSDTool : public SensitiveDetectorBase
{
 public:
  // Constructor
  TriggerSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  ~TriggerSensorSDTool() { /* If all goes well we do not own myHitColl here */ }

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() const override final;
};

#endif //TRIGGERG4_SD_TRIGGERSENSORSDTOOL_H
