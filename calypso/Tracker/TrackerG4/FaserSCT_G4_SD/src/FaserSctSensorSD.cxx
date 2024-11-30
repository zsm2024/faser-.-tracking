/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// SCT Sensitive Detector.
// The Hits are processed here. For every hit I get the position and
// an information on the sensor in which the interaction happened
//

// class headers
#include "FaserSctSensorSD.h"

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

FaserSctSensorSD::FaserSctSensorSD( const std::string& name, const std::string& hitCollectionName )
  : G4VSensitiveDetector( name )
  , m_HitColl( hitCollectionName )
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void FaserSctSensorSD::Initialize(G4HCofThisEvent *)
{
  if (!m_HitColl.isValid()) m_HitColl = std::make_unique<FaserSiHitCollection>();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool FaserSctSensorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
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
  const G4TouchableHistory* myTouch = dynamic_cast<const G4TouchableHistory*>(aStep->GetPreStepPoint()->GetTouchable());
  // for (G4int level = 0; level <= myTouch->GetHistoryDepth(); level++)
  // {
  //   G4cout << "Tracker hit at level " << level << 
  //     " in physical volume named " << myTouch->GetVolume(level)->GetName() << 
  //     " with logical volume name " << myTouch->GetVolume(level)->GetLogicalVolume()->GetName() << 
  //     " and copy number " <<          myTouch->GetVolume(level)->GetCopyNo() << G4endl;
  // }
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
  int plane = 0;
  int row = 0;
  int module = 0;
  int sensor = 0;
  this->indexMethod(myTouch, station, plane, row, module, sensor);
  // get the HepMcParticleLink from the TrackHelper
  FaserTrackHelper trHelp(aStep->GetTrack());
  m_HitColl->Emplace(lP1,
                     lP2,
                     edep,
                     aStep->GetPreStepPoint()->GetGlobalTime(),//use the global time. i.e. the time from the beginning of the event
                     trHelp.GetParticleLink(),
                     station,plane,row,module,sensor);
  return true;
}

void FaserSctSensorSD::indexMethod(const G4TouchableHistory *myTouch, 
                              int &station, int &plane, int &row, int &module, int &sensor) {


  sensor = myTouch->GetVolume(1)->GetCopyNo();
  int rawModule = myTouch->GetVolume(2)->GetCopyNo();
  row = rawModule%4;
  int halfPlaneSide = rawModule/4;
  // row = myTouch->GetVolume(2)->GetCopyNo();  // module # within half-plane is also row #
  // int halfPlaneSide = myTouch->GetVolume(3)->GetCopyNo();  // 0 = upstream, 1 = downstream
  module = (2*halfPlaneSide - 1) * (2*(row%2) - 1); 
  plane = myTouch->GetVolume(3)->GetCopyNo();
  station = myTouch->GetVolume(4)->GetCopyNo();
   
  return;
}
