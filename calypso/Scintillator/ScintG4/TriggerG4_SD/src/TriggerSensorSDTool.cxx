/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Trigger Sensitive Detector Tool.
//

// class header
#include "TriggerSensorSDTool.h"

// package includes
#include "TriggerSensorSD.h"

// STL includes
#include <exception>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TriggerSensorSDTool::TriggerSensorSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* TriggerSensorSDTool::makeSD() const
{
  ATH_MSG_DEBUG( "Creating Trigger SD: " << name() );
  return new TriggerSensorSD(name(), m_outputCollectionNames[0]);
}

