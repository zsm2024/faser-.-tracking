/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SCT Sensitive Detector Tool.
//

// class header
#include "FaserSctSensorSDTool.h"

// package includes
#include "FaserSctSensorSD.h"

// STL includes
#include <exception>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FaserSctSensorSDTool::FaserSctSensorSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* FaserSctSensorSDTool::makeSD() const
{
  ATH_MSG_DEBUG( "Creating FaserSCT SD: " << name() );
  return new FaserSctSensorSD(name(), m_outputCollectionNames[0]);
}

