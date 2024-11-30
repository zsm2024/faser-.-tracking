/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   Preshower Sensitive Detector Tool
 ****************************************************************/

#ifndef PRESHOWERG4_SD_PRESHOWERSENSORSDTOOL_H
#define PRESHOWERG4_SD_PRESHOWERSENSORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

class PreshowerSensorSDTool : public SensitiveDetectorBase
{
 public:
  // Constructor
  PreshowerSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  ~PreshowerSensorSDTool() { /* If all goes well we do not own myHitColl here */ }

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() const override final;
};

#endif //PRESHOWERG4_SD_PRESHOWERSENSORSDTOOL_H
