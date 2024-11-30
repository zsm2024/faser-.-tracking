/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "G4FaserAlg/G4FaserRunManager.h"


#include "G4GeometryManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParallelWorldScoringProcess.hh"
#include "G4RegionStore.hh"
#include "G4Run.hh"
#include "G4ScoringManager.hh"
#include "G4StateManager.hh"
#include "G4TransportationManager.hh"
#include "G4UImanager.hh"
#include "G4UserRunAction.hh"
#include "G4Version.hh"

#include <string>

G4FaserRunManager::G4FaserRunManager()
  : G4RunManager()
  , AthMessaging("G4FaserRunManager")
  , m_recordFlux(false)
  // , m_fastSimTool("FastSimulationMasterTool")
  , m_physListSvc("FaserPhysicsListSvc", "G4FaserRunManager")
  , m_detGeoSvc("DetectorGeometrySvc", "G4FaserRunManager")
  , m_volumeSmartlessLevel({})
{  }


G4FaserRunManager* G4FaserRunManager::GetG4FaserRunManager ATLAS_NOT_THREAD_SAFE ()
{
  static G4FaserRunManager *thisManager = nullptr;
  if (!thisManager) { thisManager = new G4FaserRunManager; } // Leaked
  return thisManager;
}


void G4FaserRunManager::Initialize()
{
  // ADA 11/28.2018: switch initialization order to meet ISF requirements
  // Call the base class Initialize method. This will call
  // InitializeGeometry and InitializePhysics.
  G4RunManager::Initialize();
}


void G4FaserRunManager::InitializeGeometry()
{
  ATH_MSG_DEBUG( "InitializeGeometry()" );
  if (m_detGeoSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR ( "Could not retrieve the DetectorGeometrySvc" );
    G4ExceptionDescription description;
    description << "InitializeGeometry: Failed to retrieve IDetectorGeometrySvc.";
    G4Exception("G4FaserRunManager", "CouldNotRetrieveDetGeoSvc", FatalException, description);
    abort(); // to keep Coverity happy
  }

  // Create/assign detector construction
  G4RunManager::SetUserInitialization(m_detGeoSvc->GetDetectorConstruction());
  if (userDetector) {
    G4RunManager::InitializeGeometry();
  }
  else {
    ATH_MSG_WARNING( " User Detector not set!!! Geometry NOT initialized!!!" );
  }
  return;
}


void G4FaserRunManager::EndEvent()
{
  ATH_MSG_DEBUG( "G4FaserRunManager::EndEvent" );
}


void G4FaserRunManager::InitializePhysics()
{
  ATH_MSG_INFO( "InitializePhysics()" );
  kernel->InitializePhysics();
  physicsInitialized = true;

  // Grab the physics list tool and set the extra options
  if (m_physListSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR ( "Could not retrieve the physics list tool" );
    G4ExceptionDescription description;
    description << "InitializePhysics: Failed to retrieve IPhysicsListSvc.";
    G4Exception("G4FaserRunManager", "CouldNotRetrievePLTool", FatalException, description);
    abort(); // to keep Coverity happy
  }
  m_physListSvc->SetPhysicsOptions();

  // // Fast simulations last
  // if (m_fastSimTool.retrieve().isFailure()) {
  //   ATH_MSG_ERROR ( "Could not retrieve the FastSim master tool" );
  //   G4ExceptionDescription description;
  //   description << "InitializePhysics: Failed to retrieve IFastSimulationMasterTool.";
  //   G4Exception("G4FaserRunManager", "CouldNotRetrieveFastSimMaster", FatalException, description);
  //   abort(); // to keep Coverity happy
  // }
  // if(m_fastSimTool->initializeFastSims().isFailure()) {
  //   G4ExceptionDescription description;
  //   description << "InitializePhysics: Call to IFastSimulationMasterTool::initializeFastSims failed.";
  //   G4Exception("G4FaserRunManager", "FailedToInitializeFastSims", FatalException, description);
  //   abort(); // to keep Coverity happy
  // }

  if (m_recordFlux) {
    m_fluxRecorder->InitializeFluxRecording();
  }

  return;
}

bool G4FaserRunManager::ProcessEvent(G4Event* event)
{

  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_GeomClosed);

  currentEvent = event;

  eventManager->ProcessOneEvent(currentEvent);
  if (currentEvent->IsAborted()) {
    ATH_MSG_WARNING( "G4FaserRunManager::ProcessEvent: Event Aborted at Detector Simulation level" );
    currentEvent = nullptr;
    return true;
  }

  if (m_recordFlux) { m_fluxRecorder->RecordFlux(currentEvent); }

  this->StackPreviousEvent(currentEvent);
  bool abort = currentEvent->IsAborted();
  currentEvent = nullptr;

  return abort;
}

void G4FaserRunManager::RunTermination()
{
  ATH_MSG_DEBUG( " G4FaserRunManager::RunTermination() " );
  if (m_recordFlux) {
    m_fluxRecorder->WriteFluxInformation();
  }
  
  this->CleanUpPreviousEvents();
  previousEvents->clear();

  if (userRunAction) {
    userRunAction->EndOfRunAction(currentRun);
  }

  delete currentRun;
  currentRun = nullptr;
  runIDCounter++;

  ATH_MSG_VERBOSE( "Changing the state..." );
  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_Idle);

  ATH_MSG_VERBOSE( "Opening the geometry back up" );
  G4GeometryManager::GetInstance()->OpenGeometry();

  ATH_MSG_VERBOSE( "Terminating the run...  State is " << stateManager->GetStateString( stateManager->GetCurrentState() ) );
  kernel->RunTermination();
  ATH_MSG_VERBOSE( "All done..." );

  userRunAction = nullptr;
  userEventAction = nullptr;
  userSteppingAction = nullptr;
  userStackingAction = nullptr;
  userTrackingAction = nullptr;
  userDetector = nullptr;
  userPrimaryGeneratorAction = nullptr;

  return;
}


