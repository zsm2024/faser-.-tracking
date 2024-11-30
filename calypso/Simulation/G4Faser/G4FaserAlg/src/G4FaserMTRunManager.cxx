/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Hide multi-threading classes from builds without G4MT
#include "G4Types.hh"
#ifdef G4MULTITHREADED

#include "G4FaserAlg/G4FaserMTRunManager.h"

#include "G4StateManager.hh"
#include "G4GeometryManager.hh"
#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4MTRunManagerKernel.hh"

#include "G4IonTable.hh"
#include "G4ParticleTable.hh"
#include "G4CascadeInterface.hh"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

G4FaserMTRunManager::G4FaserMTRunManager()
  : G4MTRunManager()
  , AthMessaging("G4FaserMTRunManager")
  , m_detGeoSvc("DetectorGeometrySvc", "G4FaserMTRunManager")
  , m_physListSvc("FaserPhysicsListSvc", "G4FaserMTRunManager")
  , m_fastSimTool("FastSimulationMasterTool")
{}


G4FaserMTRunManager* G4FaserMTRunManager::GetG4FaserMTRunManager ATLAS_NOT_THREAD_SAFE ()
{
  static G4FaserMTRunManager* thisManager = nullptr;
  if (!thisManager) { thisManager = new G4FaserMTRunManager; }
  return thisManager;
}


void G4FaserMTRunManager::Initialize()
{
  ATH_MSG_INFO("Initialize");
  // Set up geometry and physics in base class.
  G4RunManager::Initialize();
  // Construct scoring worlds
  ConstructScoringWorlds();
  // Run initialization in G4RunManager.
  // Normally done in BeamOn.
  RunInitialization();
  // Setup physics decay channels.
  // Normally done in InitializeEventLoop, from DoEventLoop, from BeamOn.
  GetMTMasterRunManagerKernel()->SetUpDecayChannels();
  // Setup UI commands
  PrepareCommandsStack();
}


void G4FaserMTRunManager::InitializeGeometry()
{
  ATH_MSG_INFO("InitializeGeometry");

  // Retrieve detector geo service
  if (m_detGeoSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not retrieve the DetectorGeometrySvc");
    G4ExceptionDescription description;
    description << "InitializeGeometry: Failed to retrieve IDetectorGeometrySvc.";
    G4Exception("G4FaserMTRunManager", "CouldNotRetrieveDetGeoSvc",
                FatalException, description);
    abort(); // to keep Coverity happy
  }

  // Create/assign detector construction
  SetUserInitialization( m_detGeoSvc->GetDetectorConstruction() );
  if (userDetector) {
    G4RunManager::InitializeGeometry();
  }
  else {
    // Shouldn't we abort here?
    ATH_MSG_WARNING("User Detector not set!!! Geometry NOT initialized!!!");
  }

  // Setup the sensitive detectors on master.
  /*ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
    if (svcLocator->service("SensitiveDetectorSvc", m_senDetSvc).isFailure()){
    ATH_MSG_ERROR ( "Could not retrieve the SD service" );
    throw "CouldNotRetrieveSDService";
    }*/
}

void G4FaserMTRunManager::InitializePhysics()
{
  ATH_MSG_INFO("InitializePhysics");
  kernel->InitializePhysics();

  /// todo: find out if we need the special Bertini handling here
  G4CascadeInterface::Initialize();
  physicsInitialized = true;

  // Grab the physics list tool and set the extra options
  if (m_physListSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not retrieve the physics list tool");
    G4ExceptionDescription description;
    description << "InitializePhysics: Failed to retrieve IPhysicsListSvc.";
    G4Exception("G4FaserMTRunManager", "CouldNotRetrievePLTool",
                FatalException, description);
    abort(); // to keep Coverity happy
  }
  m_physListSvc->SetPhysicsOptions();

  // Setup the fast simulations
  const std::string methodName = "G4FaserMTRunManager::InitializePhysics";
  if(m_fastSimTool.retrieve().isFailure()) {
    throw GaudiException("Could not retrieve FastSims master tool",
                         methodName, StatusCode::FAILURE);
  }
  if(m_fastSimTool->initializeFastSims().isFailure()) {
    throw GaudiException("Failed to initialize FastSims for master thread",
                         methodName, StatusCode::FAILURE);
  }

  // TODO: parallel worlds stuff here
}


// I suspect a lot of this could just be delegated to the base class.
// I wonder if something there breaks in Athena..
void G4FaserMTRunManager::RunTermination()
{
  // vanilla G4 calls a different method... why?
  CleanUpPreviousEvents();
  previousEvents->clear();

  if(userRunAction) { userRunAction->EndOfRunAction(currentRun); }

  delete currentRun;
  currentRun = nullptr;
  runIDCounter++;

  ATH_MSG_INFO( "Changing the state..." );
  G4StateManager* stateManager = G4StateManager::GetStateManager();
  stateManager->SetNewState(G4State_Idle);

  ATH_MSG_INFO( "Opening the geometry back up" );
  G4GeometryManager::GetInstance()->OpenGeometry();

  ATH_MSG_INFO( "Terminating the run...  State is " <<
                stateManager->GetStateString( stateManager->GetCurrentState() ) );
  kernel->RunTermination();
  ATH_MSG_INFO( "All done..." );

  userRunAction = nullptr;
  userEventAction = nullptr;
  userSteppingAction = nullptr;
  userStackingAction = nullptr;
  userTrackingAction = nullptr;
  // physicsList = nullptr;
  userDetector = nullptr;
  userPrimaryGeneratorAction = nullptr;
}

#endif // G4MULTITHREADED

