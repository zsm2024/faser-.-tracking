/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
  SCT Sensitive Detector Tool
 ****************************************************************/

#ifndef FASERSCT_G4_SD_FASERSCTSENSORSDTOOL_H
#define FASERSCT_G4_SD_FASERSCTSENSORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

class FaserSctSensorSDTool : public SensitiveDetectorBase
{
 public:
  // Constructor
  FaserSctSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  ~FaserSctSensorSDTool() { /* If all goes well we do not own myHitColl here */ }

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() const override final;
};

#endif //FASERSCT_G4_SD_FASERSCTSENSORSDTOOL_H
