/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserISF_Geant4Event/FaserISFG4GeoHelper.h"

// Athena includes
#include "FaserDetDescr/FaserRegion.h"

//Geant4
#include "G4LogicalVolumeStore.hh"
#include "G4Step.hh"
#include "globals.hh"

#include "G4TransportationManager.hh"
#include "SimHelpers/StepHelper.h"

#include <iostream>

G4LogicalVolume* iGeant4::FaserISFG4GeoHelper::s_sctLV = nullptr;
G4LogicalVolume* iGeant4::FaserISFG4GeoHelper::s_vetoLV = nullptr;
G4LogicalVolume* iGeant4::FaserISFG4GeoHelper::s_vetonuLV = nullptr;
G4LogicalVolume* iGeant4::FaserISFG4GeoHelper::s_triggerLV = nullptr;
G4LogicalVolume* iGeant4::FaserISFG4GeoHelper::s_preshowerLV = nullptr;
G4LogicalVolume* iGeant4::FaserISFG4GeoHelper::s_ecalLV = nullptr;
G4LogicalVolume* iGeant4::FaserISFG4GeoHelper::s_dipoleLV = nullptr;
G4LogicalVolume* iGeant4::FaserISFG4GeoHelper::s_emulsionLV = nullptr;
G4LogicalVolume* iGeant4::FaserISFG4GeoHelper::s_trenchLV = nullptr;

//________________________________________________________________________
FaserDetDescr::FaserRegion
iGeant4::FaserISFG4GeoHelper::nextGeoId(const G4Step* aStep, int truthVolLevel)
{

  if (s_sctLV == nullptr && s_vetoLV == nullptr && s_triggerLV == nullptr && s_preshowerLV == nullptr && s_vetonuLV == nullptr && s_ecalLV == nullptr && s_emulsionLV == nullptr && s_dipoleLV == nullptr && s_trenchLV == nullptr) // Initialize
  { 
    G4LogicalVolumeStore * lvs = G4LogicalVolumeStore::GetInstance();
    for (size_t i = 0; i < lvs->size(); ++i) {
      G4LogicalVolume* thisLV = (*lvs)[i];
      if ( thisLV == nullptr ) { continue; }
      G4String thisName = thisLV->GetName();
      if ( ( s_sctLV == nullptr && thisName == "SCT::Station" ) || thisName == "SCT::SCT" ) { s_sctLV = thisLV; }
      else if ( ( s_vetoLV == nullptr && thisName == "Veto::VetoStationA" ) || thisName == "Veto::Veto" ) { s_vetoLV = thisLV; }
      else if ( ( s_vetonuLV == nullptr && thisName == "VetoNu::VetoNuStationA" ) || thisName == "VetoNu::VetoNu" ) { s_vetonuLV = thisLV; }
      else if ( ( s_triggerLV == nullptr && thisName == "Trigger::TriggerStationA" ) || thisName == "Trigger::Trigger" ) { s_triggerLV = thisLV; }
      else if ( ( s_preshowerLV == nullptr && thisName == "Preshower::PreshowerStationA" ) || thisName == "Preshower::Preshower" ) { s_preshowerLV = thisLV; }
      else if ( thisName == "Ecal::Ecal" ) { s_ecalLV = thisLV; }
      else if ( thisName == "Dipole::Dipole" ) { s_dipoleLV = thisLV; }
      else if ( ( s_emulsionLV == nullptr && thisName == "Emulsion::EmulsionStationA" ) || thisName == "Emulsion::Emulsion" )  { s_emulsionLV = thisLV; }
      else if ( thisName == "Trench::Trench" ) { s_trenchLV = thisLV; }
    }

    const auto& worldVolume = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume();
    FaserISFG4GeoHelper::checkVolumeDepth(worldVolume, truthVolLevel);
  }

  FaserDetDescr::FaserRegion nextGeoID = truthVolLevel > 1 ? FaserDetDescr::fFaserCavern
                                                           : FaserDetDescr::fUndefinedFaserRegion;

  const G4StepPoint *postStep = aStep->GetPostStepPoint();
  bool leavingG4World       = postStep->GetStepStatus()==fWorldBoundary;

  if ( leavingG4World ) {
    nextGeoID = FaserDetDescr::fFaserCavern;
    return nextGeoID;
  }

  // If in mother volume, treat as cavern
  if (G4StepHelper::postStepBranchDepth(aStep)<truthVolLevel) {
    nextGeoID = FaserDetDescr::fFaserCavern;
    return nextGeoID;
  }

  G4LogicalVolume* postStepVolume = G4StepHelper::getPostStepLogicalVolume(aStep, truthVolLevel);
  if (s_sctLV != nullptr && s_sctLV == postStepVolume )
  {
    nextGeoID = FaserDetDescr::fFaserTracker;
  }
  else if ((s_vetoLV != nullptr && s_vetoLV == postStepVolume) || 
           (s_vetonuLV != nullptr && s_vetonuLV == postStepVolume) ||
           (s_triggerLV != nullptr && s_triggerLV == postStepVolume) || 
           (s_preshowerLV != nullptr && s_preshowerLV == postStepVolume))
  {
    nextGeoID = FaserDetDescr::fFaserScintillator;
  }
  else if (s_ecalLV != nullptr && s_ecalLV == postStepVolume)
  {
    nextGeoID = FaserDetDescr::fFaserCalorimeter;
  }
  else if (s_emulsionLV != nullptr && s_emulsionLV == postStepVolume)
  {
    nextGeoID = FaserDetDescr::fFaserNeutrino;
  }
  else if (s_dipoleLV != nullptr && s_dipoleLV == postStepVolume)
  {
    nextGeoID = FaserDetDescr::fFaserDipole;
  }
  else if (s_trenchLV != nullptr && s_trenchLV == postStepVolume)
  {
    nextGeoID = FaserDetDescr::fFaserTrench;
  }
  else
  {
    nextGeoID = FaserDetDescr::fFaserCavern;
  }
  
  return nextGeoID;
}

bool iGeant4::FaserISFG4GeoHelper::checkVolumeDepth(G4LogicalVolume* lv, int volLevel, int depth) 
{
  if (lv==nullptr) { return false; }

  bool Cavern = false;
  // Check the volumes rather explicitly

  if ( ((s_sctLV != nullptr) && (lv->GetName() == s_sctLV->GetName())) ||
       ((s_vetoLV != nullptr) && (lv->GetName() == s_vetoLV->GetName())) ||
       ((s_vetonuLV != nullptr) && (lv->GetName() == s_vetonuLV->GetName())) ||
       ((s_triggerLV != nullptr) && (lv->GetName() == s_triggerLV->GetName())) ||
       ((s_preshowerLV != nullptr) && (lv->GetName() == s_preshowerLV->GetName())) ||
       ((s_dipoleLV != nullptr) && (lv->GetName() == s_dipoleLV->GetName())) ||
       ((s_ecalLV != nullptr) && (lv->GetName() == s_ecalLV->GetName())) ||
       ((s_emulsionLV != nullptr) && (lv->GetName() == s_emulsionLV->GetName())) ||
       ((s_trenchLV != nullptr) && (lv->GetName() == s_trenchLV->GetName())) ) {
    if(depth!=volLevel) {
      G4ExceptionDescription description;
      description << "Volume " << lv->GetName() << " at depth " << depth << " instead of depth " << volLevel;
      G4Exception("iGeant4::FaserISFG4GeoHelper", "checkVolumeDepth", FatalException, description);
      return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
    }
    G4cout << "Volume " << lv->GetName() << " is correctly registered at depth " << depth << G4endl;
  }
  // else if ( lv->GetName()=="BeamPipe::BeamPipeCentral" ) { // Things that are supposed to be one deeper
  //   if (depth!=volLevel+1) {
  //     G4ExceptionDescription description;
  //     description << "Volume " << lv->GetName() << " at depth " << depth << " instead of depth " << volLevel+1;
  //     G4Exception("iGeant4::FaserISFG4GeoHelper", "checkVolumeDepth", FatalException, description);
  //     return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
  //   }
  //   //ATH_MSG_DEBUG("Volume " << lv->GetName() << " is correctly registered at depth " << depth);
  // }
  else if ( lv->GetName().find("CavernInfra")!=std::string::npos ) { // Things that are supposed to be one shallower
    if (depth==volLevel-1) {
      Cavern=true;
    G4cout << "Volume " << lv->GetName() << " is correctly registered at depth " << depth << G4endl;
      // Note: a number of volumes exist with "CavernInfra" in the name at the wrong depth, so we just need to
      //   check that there's at least one at the right depth
    } // Check of volume level
  } // Check of volume name

  // Going through the volume depth
  for (size_t i=0; i<lv->GetNoDaughters(); ++i) {
    Cavern = Cavern || checkVolumeDepth( lv->GetDaughter(i)->GetLogicalVolume() , volLevel , depth+1 );
  }
  if (depth==0 && !Cavern && volLevel>1) {
    G4ExceptionDescription description;
    description << "No CavernInfra volume registered at depth " << volLevel-1;
    G4Exception("iGeant4::FaserISFG4GeoHelper", "checkVolumeDepth", FatalException, description);
    return false; //The G4Exception call above should abort the job, but Coverity does not seem to pick this up.
  }

  return Cavern;
}
