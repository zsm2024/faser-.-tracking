/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// Ecal Sensitive Detector.
// The Hits are processed here. For every hit I get the position and
// an information on the sensor in which the interaction happened
//

// class headers
#include "EcalSensorSD.h"

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

EcalSensorSD::EcalSensorSD( const std::string& name, const std::string& hitCollectionName )
  : G4VSensitiveDetector( name )
  , m_HitColl( hitCollectionName )
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EcalSensorSD::Initialize(G4HCofThisEvent *)
{
  if (!m_HitColl.isValid()) m_HitColl = std::make_unique<CaloHitCollection>();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool EcalSensorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*ROhist*/)
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
  // const G4AffineTransform transformation = myTouch->GetHistory()->GetTopTransform();
  // G4int targetDepth = (myTouch->GetVolume(0)->GetName().find("FiberPb") != std::string::npos ? 5 : 3);
  //const G4AffineTransform topTransformation = myTouch->GetHistory()->GetTopTransform();
  const G4AffineTransform transformation = myTouch->GetHistory()->GetTransform(kModuleDepth);
  G4ThreeVector localPosition1 = transformation.TransformPoint(coord1);
  G4ThreeVector localPosition2 = transformation.TransformPoint(coord2);
  // G4String vName = myTouch->GetVolume(targetDepth)->GetName();
  // if (targetDepth == 5)
  // {
  //   std::cout << "Fiber hit at global position: " << coord1 << " in " << vName <<std::endl;
  // }
  // else
  // {
  //   std::cout << "Plane hit at global position:" << coord1 << " in " << vName  << std::endl;
  // }
  // for (G4int depth = 0; depth <= myTouch->GetHistoryDepth(); depth++)
  // {
  //   G4ThreeVector dbgPosition = myTouch->GetHistory()->GetTransform(depth).TransformPoint(coord1);
  //   std::cout << "depth: " << depth << " = " << dbgPosition << " (" << myTouch->GetVolume(depth)->GetName() << ")" << std::endl;
  // }

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

  // Birk's Law Correction

  const G4Track*              const track    = aStep->GetTrack      () ;
  const G4ParticleDefinition* const particle = track->GetDefinition () ;
  //const double                      charge   = particle->GetPDGCharge  () ; 
  const G4StepPoint* const          preStep  = aStep->GetPreStepPoint () ;
  const G4MaterialCutsCouple* const material = preStep->GetMaterialCutsCouple () ;


  //double ecorrected = edep;

  double ecorrected = edep * birkCorrection( particle , track->GetKineticEnergy(), material ) ;

  // Non-uniformity correction

  ecorrected *= localNonUniformity(lP1[0], lP1[1]) ;

  // Now Navigate the history to know in what detector the step is:
  // and finally set the ID of det element in which the hit is.
  //
  //G4int History;
  //
  // Get station and plate
  //
  int row = 0;
  int module = 0;
  this->indexMethod(myTouch, row, module);
  // get the HepMcParticleLink from the TrackHelper
  FaserTrackHelper trHelp(aStep->GetTrack());
  m_HitColl->Emplace(lP1,
                     lP2,
                     ecorrected,
                     aStep->GetPreStepPoint()->GetGlobalTime(),//use the global time. i.e. the time from the beginning of the event
                     trHelp.GetParticleLink(),
                     row,module);
  return true;
}

void EcalSensorSD::indexMethod(const G4TouchableHistory *myTouch, 
                              int &row, int &module) {

  G4int targetDepth = (myTouch->GetVolume(0)->GetName().find("FiberPb") != std::string::npos ? kFiberDepth : kModuleDepth);
  module = myTouch->GetVolume(targetDepth)->GetCopyNo();
  row    = myTouch->GetVolume(targetDepth+1)->GetCopyNo();

  return;
}
