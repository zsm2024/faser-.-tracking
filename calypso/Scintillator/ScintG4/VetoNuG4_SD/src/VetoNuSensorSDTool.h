/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   VetoNu Sensitive Detector Tool
 ****************************************************************/

#ifndef VETONUG4_SD_VETONUSENSORSDTOOL_H
#define VETONUG4_SD_VETONUSENSORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

class VetoNuSensorSDTool : public SensitiveDetectorBase
{
 public:
  // Constructor
  VetoNuSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  ~VetoNuSensorSDTool() { /* If all goes well we do not own myHitColl here */ }

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() const override final;
};

#endif //VETOG4_SD_VETOSENSORSDTOOL_H
