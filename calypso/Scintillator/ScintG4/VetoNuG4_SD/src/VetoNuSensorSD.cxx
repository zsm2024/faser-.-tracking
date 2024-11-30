/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// VetoNu Sensitive Detector.
// The Hits are processed here. For every hit I get the position and
// an information on the sensor in which the interaction happened
//

// class headers
#include "VetoNuSensorSD.h"

// athena includes
#include "FaserMCTruth/FaserTrackHelper.h"

// Geant4 includes
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

// CLHEP transform
#include "CLHEP/Geometry/Transform3D.h"

#include <memory> // For make unique

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

VetoNuSensorSD::VetoNuSensorSD( const std::string& name, const std::string& hitCollectionName )
  : G4VSensitiveDetector( name )
  , m_HitColl( hitCollectionName )
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void VetoNuSensorSD::Initialize(G4HCofThisEvent *)
{
  if (!m_HitColl.isValid()) m_HitColl = std::make_unique<ScintHitCollection>();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool VetoNuSensorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
{
  double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.) {
    if(aStep->GetTrack()->GetDefinition()!=G4Geantino::GeantinoDefinition() &&
       aStep->GetTrack()->GetDefinition()!=G4ChargedGeantino::ChargedGeantinoDefinition())
      return false;
  }
  edep *= CLHEP::MeV;
  //
  // Get the Touchable History:
  //
  const G4TouchableHistory *myTouch = dynamic_cast<const G4TouchableHistory*>(aStep->GetPreStepPoint()->GetTouchable());
  //
  // Get the hit coordinates. Start and End Point
  //
  G4ThreeVector coord1 = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector coord2 = aStep->GetPostStepPoint()->GetPosition();
  //
  // Calculate the local step begin and end position.
  // From a G4 FAQ:
  // http://geant4-hn.slac.stanford.edu:5090/HyperNews/public/get/geometry/17/1.html
  //
  const G4AffineTransform transformation = myTouch->GetHistory()->GetTopTransform();
  G4ThreeVector localPosition1 = transformation.TransformPoint(coord1);
  G4ThreeVector localPosition2 = transformation.TransformPoint(coord2);
  //
  // Get it into a vector in local coords and with the right units:
  //
  HepGeom::Point3D<double> lP1,lP2;
 
  // No funny business with coordinates like ATLAS...
  lP1[2] = localPosition1[2]*CLHEP::mm;
  lP1[1] = localPosition1[1]*CLHEP::mm;
  lP1[0] = localPosition1[0]*CLHEP::mm;

  lP2[2] = localPosition2[2]*CLHEP::mm;
  lP2[1] = localPosition2[1]*CLHEP::mm;
  lP2[0] = localPosition2[0]*CLHEP::mm;

  // Now Navigate the history to know in what detector the step is:
  // and finally set the ID of det element in which the hit is.
  //
  //G4int History;
  //
  // Get station and plate
  //
  int station = 0;
  int plate = 0;
  this->indexMethod(myTouch, station, plate);
  // get the HepMcParticleLink from the TrackHelper
  FaserTrackHelper trHelp(aStep->GetTrack());
  m_HitColl->Emplace(lP1,
                     lP2,
                     edep,
                     aStep->GetPreStepPoint()->GetGlobalTime(),//use the global time. i.e. the time from the beginning of the event
                     trHelp.GetParticleLink(),
                     3,station,plate);
  return true;
}

void VetoNuSensorSD::indexMethod(const G4TouchableHistory *myTouch, 
                              int &station, int &plate) {


  plate = myTouch->GetVolume()->GetCopyNo();
  const std::string stationName = myTouch->GetVolume(1)->GetLogicalVolume()->GetName();
  station = (stationName == "VetoNu::VetoNuStationA" ? 0 : 1 );
   
  return;
}
