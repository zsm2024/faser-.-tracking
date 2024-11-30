/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "FaserMCTruthBase/FaserTruthStrategyManager.h"

// Framework includes
#include "AthenaBaseComps/AthMsgStreamMacros.h"

// Geant4 Includes
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"

// Truth-related includes
#include "FaserMCTruth/FaserG4EventUserInfo.h"
// #include "FaserMCTruth/FaserTrackHelper.h"

// ISF includes
#include "FaserISF_Interfaces/IFaserTruthSvc.h"
#include "FaserISF_Event/FaserISFParticle.h"

// DetectorDescription
#include "FaserDetDescr/FaserRegionHelper.h"
#include "FaserISF_Geant4Event/FaserGeant4TruthIncident.h"
#include "FaserISF_Geant4Event/FaserISFG4GeoHelper.h"

FaserTruthStrategyManager::FaserTruthStrategyManager()
  : m_truthSvc(nullptr)
{
}

FaserTruthStrategyManager* FaserTruthStrategyManager::GetStrategyManager()
{
  static FaserTruthStrategyManager theMgr;
  return &theMgr;
}

void FaserTruthStrategyManager::SetISFTruthSvc(ISF::IFaserTruthSvc *truthSvc)
{
  m_truthSvc = truthSvc;
}

bool FaserTruthStrategyManager::CreateTruthIncident(const G4Step* aStep, int subDetVolLevel) const
{
  FaserDetDescr::FaserRegion geoID = iGeant4::FaserISFG4GeoHelper::nextGeoId(aStep, subDetVolLevel);

  auto* eventInfo = static_cast<FaserG4EventUserInfo*> (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());

  // This is pretty ugly and but necessary because the Geant4TruthIncident
  // requires an ISFParticle at this point.
  // TODO: cleanup Geant4TruthIncident to not require an ISFParticle instance any longer
  const Amg::Vector3D myPos(0,0,0);
  const Amg::Vector3D myMom(0,0,0);
  double myMass = 0.0;
  double myCharge = 0.0;
  int    myPdgCode = 0;
  int    mystatus = 3333;
  double myTime =0.;
  const ISF::FaserDetRegionSvcIDPair origin(geoID, ISF::fUndefinedSimID);
  int myBCID = 0;
  ISF::FaserISFParticle myISFParticle(myPos, myMom, myMass, myCharge, myPdgCode, mystatus, myTime, origin, myBCID);

  iGeant4::FaserGeant4TruthIncident truth(aStep, myISFParticle, geoID, eventInfo);

  m_truthSvc->registerTruthIncident(truth);
  return false;
}

