/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Veto Sensitive Detector Tool.
//

// class header
#include "VetoSensorSDTool.h"

// package includes
#include "VetoSensorSD.h"

// STL includes
#include <exception>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

VetoSensorSDTool::VetoSensorSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* VetoSensorSDTool::makeSD() const
{
  ATH_MSG_DEBUG( "Creating Veto SD: " << name() );
  return new VetoSensorSD(name(), m_outputCollectionNames[0]);
}

