/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Emulsion Sensitive Detector Tool.
//

// class header
#include "EmulsionSensorSDTool.h"

// package includes
#include "EmulsionSensorSD.h"

// STL includes
#include <exception>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EmulsionSensorSDTool::EmulsionSensorSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* EmulsionSensorSDTool::makeSD() const
{
  ATH_MSG_DEBUG( "Creating Emulsion SD: " << name() );

  EmulsionSensorSD* ecsd = new EmulsionSensorSD(name(), m_outputCollectionNames[0]);

  return ecsd;
}

