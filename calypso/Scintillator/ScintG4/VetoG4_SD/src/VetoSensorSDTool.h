/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   Veto Sensitive Detector Tool
 ****************************************************************/

#ifndef VETOG4_SD_VETOSENSORSDTOOL_H
#define VETOG4_SD_VETOSENSORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

class VetoSensorSDTool : public SensitiveDetectorBase
{
 public:
  // Constructor
  VetoSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  ~VetoSensorSDTool() { /* If all goes well we do not own myHitColl here */ }

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() const override final;
};

#endif //VETOG4_SD_VETOSENSORSDTOOL_H
