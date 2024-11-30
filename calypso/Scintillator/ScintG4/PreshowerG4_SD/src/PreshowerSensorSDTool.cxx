/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Preshower Sensitive Detector Tool.
//

// class header
#include "PreshowerSensorSDTool.h"

// package includes
#include "PreshowerSensorSD.h"

// STL includes
#include <exception>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PreshowerSensorSDTool::PreshowerSensorSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* PreshowerSensorSDTool::makeSD() const
{
  ATH_MSG_DEBUG( "Creating Preshower SD: " << name() );
  return new PreshowerSensorSD(name(), m_outputCollectionNames[0]);
}

