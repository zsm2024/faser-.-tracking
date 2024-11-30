/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VetoNu Sensitive Detector Tool.
//

// class header
#include "VetoNuSensorSDTool.h"

// package includes
#include "VetoNuSensorSD.h"

// STL includes
#include <exception>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

VetoNuSensorSDTool::VetoNuSensorSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* VetoNuSensorSDTool::makeSD() const
{
  ATH_MSG_DEBUG( "Creating VetoNu SD: " << name() );
  return new VetoNuSensorSD(name(), m_outputCollectionNames[0]);
}

