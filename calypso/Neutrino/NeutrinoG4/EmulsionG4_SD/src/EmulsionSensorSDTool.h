/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   Emulsion Sensitive Detector Tool
 ****************************************************************/

#ifndef EMULSIONG4_SD_EMULSIONSENSORSDTOOL_H
#define EMULSIONG4_SD_EMULSIONSENSORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

class EmulsionSensorSDTool : public SensitiveDetectorBase
{

public:
  // Constructor
  EmulsionSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor
  ~EmulsionSensorSDTool() { /* If all goes well we do not own myHitColl here */ }

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() const override final;

private:

};

#endif //EMULSIONG4_SD_EMULSIONSENSORSDTOOL_H
