/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Local includes
#include "G4FaserAlg.h"
#include "G4FaserFluxRecorder.h"
#include "G4FaserAlg/G4FaserActionInitialization.h"

#include "AthenaKernel/RNGWrapper.h"
#include "CxxUtils/checker_macros.h"

// Can we safely include all of these?
#include "G4FaserAlg/G4FaserMTRunManager.h"
#include "G4FaserAlg/G4FaserWorkerRunManager.h"
#include "G4FaserAlg/G4FaserUserWorkerThreadInitialization.h"
#include "G4FaserAlg/G4FaserRunManager.h"

// Geant4 includes
#include "G4StateManager.hh"
#include "G4TransportationManager.hh"
#include "G4RunManagerKernel.hh"
#include "G4EventManager.hh"
#include "G4Navigator.hh"
#include "G4PropagatorInField.hh"
#include "G4TrackingManager.hh"
#include "G4StackManager.hh"
#include "G4UImanager.hh"
#include "G4ScoringManager.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VModularPhysicsList.hh"
#include "G4ParallelWorldPhysics.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4GDMLParser.hh"

// CLHEP includes
#include "CLHEP/Random/RandomEngine.h"

// Athena includes
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
// #include "EventInfo/EventInfo.h"
#include "FaserMCTruthBase/FaserTruthStrategyManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/IThreadInitTool.h"
#include "GeneratorObjects/HepMcParticleLink.h"

// call_once mutexes
#include <mutex>
static std::once_flag initializeOnceFlag;
static std::once_flag finalizeOnceFlag;
static std::once_flag releaseGeoModelOnceFlag;

/////////////////////////////////////////////////////////////////////////////


G4FaserAlg::G4FaserAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
  // Verbosities
  declareProperty("Verbosities", m_verbosities);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4FaserAlg::initialize()
{
  ATH_MSG_DEBUG("Start of initialize()");

  // Create the scoring manager if requested
  if (m_recordFlux) G4ScoringManager::GetScoringManager();

  ATH_CHECK( m_userActionSvc.retrieve() );

  // One-time initialization
  try {
    std::call_once(initializeOnceFlag, &G4FaserAlg::initializeOnce, this);
  }
  catch(const std::exception& e) {
    ATH_MSG_ERROR("Failure in G4FaserAlg::initializeOnce: " << e.what());
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_rndmGenSvc.retrieve() );

  ATH_CHECK(m_senDetTool.retrieve());
  // ATH_CHECK(m_fastSimTool.retrieve());

  // Truth
  ATH_CHECK( m_truthRecordSvc.retrieve() );
  ATH_MSG_INFO( "- Using ISF TruthRecordSvc : " << m_truthRecordSvc.typeAndName() );
  // ATH_CHECK( m_geoIDSvc.retrieve() );
  // ATH_MSG_INFO( "- Using ISF GeoIDSvc       : " << m_geoIDSvc.typeAndName() );

  FaserTruthStrategyManager* sManager = FaserTruthStrategyManager::GetStrategyManager();
  sManager->SetISFTruthSvc( &(*m_truthRecordSvc) );
  // sManager->SetISFGeoIDSvc( &(*m_geoIDSvc) );
  
  // I/O
  ATH_CHECK( m_eventInfoKey.initialize() );
  ATH_CHECK( m_inputTruthCollectionKey.initialize());
  ATH_CHECK( m_outputTruthCollectionKey.initialize());

  ATH_CHECK(m_inputConverter.retrieve());

  // G4LogicalVolumeStore* logicalVolumeStore = G4LogicalVolumeStore::GetInstance();
  // for (auto* ilv : *logicalVolumeStore ) 
  // {
  //   ATH_MSG_ALWAYS("Found logical volume " << ilv->GetName());
  // }


  ATH_MSG_DEBUG("End of initialize()");
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void G4FaserAlg::initializeOnce()
{
  // Assign physics list
  if(m_physListSvc.retrieve().isFailure()) {
    throw std::runtime_error("Could not initialize FASER PhysicsListSvc!");
  }
  for (const auto& action_tool : m_actionTools) {
    if (m_userActionSvc->addActionTool(action_tool).isFailure()) {
      throw std::runtime_error("Failed to add action tool "+action_tool.name());
    }
  }

  // Create the (master) run manager
  if(m_useMT) {
#ifdef G4MULTITHREADED
    auto* runMgr = G4FaserMTRunManager::GetG4FaserMTRunManager();
    m_physListSvc->SetPhysicsList();
    runMgr->SetDetGeoSvc( m_detGeoSvc.typeAndName() );
    // runMgr->SetFastSimMasterTool(m_fastSimTool.typeAndName() );
    runMgr->SetPhysListSvc( m_physListSvc.typeAndName() );
    // Worker Thread initialization used to create worker run manager on demand.
    std::unique_ptr<G4FaserUserWorkerThreadInitialization> workerInit =
      std::make_unique<G4FaserUserWorkerThreadInitialization>();
    workerInit->SetDetGeoSvc( m_detGeoSvc.typeAndName() );
    // workerInit->SetSDMasterTool( m_senDetTool.typeAndName() );
    // workerInit->SetFastSimMasterTool( m_fastSimTool.typeAndName() );
    runMgr->SetUserInitialization( workerInit.release() );
    std::unique_ptr<G4FaserActionInitialization> actionInitialization =
      std::make_unique<G4FaserActionInitialization>(&*m_userActionSvc);
    runMgr->SetUserInitialization(actionInitialization.release());
#else
    throw std::runtime_error("Trying to use multi-threading in non-MT build!");
#endif
  }
  // Single-threaded run manager
  else {
    auto* runMgr ATLAS_THREAD_SAFE = G4FaserRunManager::GetG4FaserRunManager();

    // auto* rm = G4RunManager::GetRunManager();
    // if(!rm) {
    //   throw std::runtime_error("Run manager retrieval has failed");
    // }

    // if (rm != runMgr)
    // {
    //   throw std::runtime_error("Different Run managers");
    // }

    // ATH_MSG_ALWAYS("Setting Physics List");
    // m_physListSvc->CreatePhysicsList();
    // auto physList = m_physListSvc->GetPhysicsList();
    // rm->SetUserInitialization(physList);
    m_physListSvc->SetPhysicsList();
    // ATH_MSG_ALWAYS("Physics List Set");
    runMgr->SetRecordFlux( m_recordFlux, std::make_unique<G4FaserFluxRecorder>() );
    runMgr->SetLogLevel( int(msg().level()) ); // Synch log levels
    runMgr->SetDetGeoSvc( m_detGeoSvc.typeAndName() );
    // runMgr->SetFastSimMasterTool(m_fastSimTool.typeAndName() );
    runMgr->SetPhysListSvc(m_physListSvc.typeAndName() );
    std::unique_ptr<G4FaserActionInitialization> actionInitialization =
      std::make_unique<G4FaserActionInitialization>(&*m_userActionSvc);
    runMgr->SetUserInitialization(actionInitialization.release());
}

  // G4 user interface commands
  G4UImanager *ui = G4UImanager::GetUIpointer();

  // Load custom libraries
  if (!m_libList.empty()) {
    ATH_MSG_INFO("G4FaserAlg specific libraries requested ");
    std::string temp="/load "+m_libList;
    ui->ApplyCommand(temp);
  }
  // Load custom physics
  if (!m_physList.empty()) {
    ATH_MSG_INFO("requesting a specific physics list "<< m_physList);
    std::string temp="/Physics/GetPhysicsList "+m_physList;
    ui->ApplyCommand(temp);
  }
  // Load custom magnetic field
  if (!m_fieldMap.empty()) {
    ATH_MSG_INFO("requesting a specific field map "<< m_fieldMap);
    ATH_MSG_INFO("the field is initialized straight away");
    std::string temp="/MagneticField/Select "+m_fieldMap;
    ui->ApplyCommand(temp);
    ui->ApplyCommand("/MagneticField/Initialize");
  }

  // Send UI commands
  ATH_MSG_DEBUG("G4 Command: Trying at the end of initializeOnce()");
  for (auto g4command : m_g4commands) {
    int returnCode = ui->ApplyCommand( g4command );
    commandLog(returnCode, g4command);
  }

  // Code from G4AtlasSvc
  auto* rm = G4RunManager::GetRunManager();
  if(!rm) {
    throw std::runtime_error("Run manager retrieval has failed");
  }
  rm->Initialize();     // Initialization differs slightly in multi-threading.
  // TODO: add more details about why this is here.
  if(!m_useMT && rm->ConfirmBeamOnCondition()) {
    rm->RunInitialization();
  }

  ATH_MSG_INFO( "retrieving the Detector Geometry Service" );
  if(m_detGeoSvc.retrieve().isFailure()) {
    throw std::runtime_error("Could not initialize ATLAS DetectorGeometrySvc!");
  }

  if(m_userLimitsSvc.retrieve().isFailure()) {
    throw std::runtime_error("Could not initialize ATLAS UserLimitsSvc!");
  }
  
  if (!m_gdmlFilename.empty())
  {
    G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
    G4LogicalVolume* world = store->GetVolume("Faser::Faser");
    G4GDMLParser parser {};
    parser.Write(m_gdmlFilename.value(), world);
  }

  if (m_activateParallelGeometries) {
    G4VModularPhysicsList* thePhysicsList=dynamic_cast<G4VModularPhysicsList*>(m_physListSvc->GetPhysicsList());
    if (!thePhysicsList) {
      throw std::runtime_error("Failed dynamic_cast!! this is not a G4VModularPhysicsList!");
    }
#if G4VERSION_NUMBER >= 1010
    std::vector<std::string>& parallelWorldNames=m_detGeoSvc->GetParallelWorldNames();
    for (auto& it: parallelWorldNames) {
      thePhysicsList->RegisterPhysics(new G4ParallelWorldPhysics(it,true));
    }
#endif
  }

  return;
}

void G4FaserAlg::initializeG4()
{
  if (m_verbosities.size()>0) {
    G4TransportationManager *tm = G4TransportationManager::GetTransportationManager();
    G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
    G4EventManager *em = G4EventManager::GetEventManager();

    auto itr = m_verbosities.end();
    if ((itr = m_verbosities.find("Navigator")) != m_verbosities.end()) {
      tm->GetNavigatorForTracking()->SetVerboseLevel( atof(itr->second.data()) );
    }
    if ((itr = m_verbosities.find("Propagator")) != m_verbosities.end()) {
      tm->GetPropagatorInField()->SetVerboseLevel( atof(itr->second.data()) );
    }
    if ((itr = m_verbosities.find("Tracking")) != m_verbosities.end()) {
      rmk->GetTrackingManager()->SetVerboseLevel( atof(itr->second.data()) );
    }
    if ((itr = m_verbosities.find("Stepping")) != m_verbosities.end()) {
      rmk->GetTrackingManager()->GetSteppingManager()->
        SetVerboseLevel( atof(itr->second.data()) );
    }
    if ((itr = m_verbosities.find("Stacking")) != m_verbosities.end()) {
      rmk->GetStackManager()->SetVerboseLevel( atof(itr->second.data()) );
    }
    if ((itr = m_verbosities.find("Event")) != m_verbosities.end()) {
      em->SetVerboseLevel( atof(itr->second.data()) );
    }
  } // End of the setting of verbosities

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4FaserAlg::finalize()
{
  ATH_MSG_DEBUG(std::endl<<std::endl<<std::endl);
  ATH_MSG_INFO("++++++++++++  G4FaserAlg finalized  ++++++++++++" <<std::endl<<std::endl);

  // One time finalization
  try {
    std::call_once(finalizeOnceFlag, &G4FaserAlg::finalizeOnce, this);
  }
  catch(const std::exception& e) {
    ATH_MSG_ERROR("Failure in G4FaserAlg::finalizeOnce: " << e.what());
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void G4FaserAlg::finalizeOnce()
{
  ATH_MSG_DEBUG("\t terminating the current G4 run");
  auto runMgr = G4RunManager::GetRunManager();
  runMgr->RunTermination();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode G4FaserAlg::execute()
{
  static int n_Event=0;
  ATH_MSG_DEBUG("++++++++++++  G4FaserAlg execute  ++++++++++++");

#ifdef G4MULTITHREADED
  // In some rare cases, TBB may create more physical worker threads than
  // were requested via the pool size.  This can happen at any time.
  // In that case, those extra threads will not have had the thread-local
  // initialization done, leading to a crash.  Try to detect that and do
  // the initialization now if needed.
  if (G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume() == nullptr)
  {
    ToolHandle<IThreadInitTool> ti ("G4ThreadInitTool", nullptr);
    ATH_CHECK( ti.retrieve() );
    ti->initThread();
  }
#endif

  n_Event += 1;

  if (n_Event<=10 || (n_Event%100) == 0) {
    ATH_MSG_ALWAYS("G4FaserAlg: Event num. "  << n_Event << " start processing");
  }

  // tell TruthService we're starting a new event
  ATH_CHECK( m_truthRecordSvc->initializeTruthCollection() );

  // Release GeoModel Geometry if necessary
  if (m_releaseGeoModel) {
    try {
      std::call_once(releaseGeoModelOnceFlag, &G4FaserAlg::releaseGeoModel, this);
    }
    catch(const std::exception& e) {
      ATH_MSG_ERROR("Failure in G4FaserAlg::releaseGeoModel: " << e.what());
      return StatusCode::FAILURE;
    }
  }

  const EventContext& ctx = Gaudi::Hive::currentContext();
  // Set the RNG to use for this event. We need to reset it for MT jobs
  // because of the mismatch between Gaudi slot-local and G4 thread-local RNG.
  ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this, m_randomStreamName);
  rngWrapper->setSeed( m_randomStreamName,  ctx);
  G4Random::setTheEngine(*rngWrapper);

  ATH_MSG_DEBUG("Calling SimulateG4Event");

  ATH_CHECK(m_senDetTool->BeginOfAthenaEvent());

  SG::ReadHandle<McEventCollection> inputTruthCollection(m_inputTruthCollectionKey);
  if (!inputTruthCollection.isValid()) {
    ATH_MSG_FATAL("Unable to read input GenEvent collection " << inputTruthCollection.name() << " in store " << inputTruthCollection.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found input GenEvent collection " << inputTruthCollection.name() << " in store " << inputTruthCollection.store());
  // create copy
  SG::WriteHandle<McEventCollection> outputTruthCollection(m_outputTruthCollectionKey);
  ATH_CHECK(outputTruthCollection.record(std::make_unique<McEventCollection>(*inputTruthCollection)));
  if (!outputTruthCollection.isValid()) {
    ATH_MSG_FATAL("Unable to record output GenEvent collection " << outputTruthCollection.name() << " in store " << outputTruthCollection.store());
    return StatusCode::FAILURE;

  }
  ATH_MSG_DEBUG("Recorded output GenEvent collection " << outputTruthCollection.name() << " in store " << outputTruthCollection.store());

  std::unique_ptr<McEventCollection> shadowTruth{};
  shadowTruth = std::make_unique<McEventCollection>();
  
  G4Event *inputEvent{};
  ATH_CHECK( m_inputConverter->convertHepMCToG4Event(*outputTruthCollection, inputEvent, *shadowTruth, HepMcParticleLink::find_enumFromKey(outputTruthCollection.name())) );

  bool abort = false;
  // Worker run manager
  // Custom class has custom method call: ProcessEvent.
  // So, grab custom singleton class directly, rather than base.
  // Maybe that should be changed! Then we can use a base pointer.
  if(m_useMT) {
#ifdef G4MULTITHREADED
    auto* workerRM = G4FaserWorkerRunManager::GetG4FaserWorkerRunManager();
    abort = workerRM->ProcessEvent(inputEvent);
#else
    ATH_MSG_ERROR("Trying to use multi-threading in non-MT build!");
    return StatusCode::FAILURE;
#endif
  }
  else {
    auto* workerRM = G4FaserRunManager::GetG4FaserRunManager();
    abort = workerRM->ProcessEvent(inputEvent);
  }
  if (abort) {
    ATH_MSG_WARNING("Event was aborted !! ");
    ATH_MSG_WARNING("Simulation will now go on to the next event ");
    if (m_killAbortedEvents) {
      ATH_MSG_WARNING("setFilterPassed is now False");
      setFilterPassed(false);
    }
    if (m_flagAbortedEvents) {
      SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey, ctx);
      if (!eventInfo.isValid()) {
        ATH_MSG_FATAL( "Failed to retrieve xAOD::EventInfo while trying to update the error state!" );
        return StatusCode::FAILURE;
      }
      else {
        eventInfo->updateErrorState(xAOD::EventInfo::Core,xAOD::EventInfo::Error);
        ATH_MSG_WARNING( "Set error state in xAOD::EventInfo!" );
      }
    }
  }

  // Register all of the collections if there are any new-style SDs
  ATH_CHECK(m_senDetTool->EndOfAthenaEvent());
  // ATH_CHECK(m_fastSimTool->EndOfAthenaEvent());

  ATH_CHECK( m_truthRecordSvc->releaseEvent() );

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void G4FaserAlg::releaseGeoModel()
{
  ISvcLocator *svcLocator = Gaudi::svcLocator(); // from Bootstrap
  IGeoModelSvc *geoModel(nullptr);
  if(svcLocator->service("GeoModelSvc",geoModel).isFailure()) {
    ATH_MSG_WARNING( " ----> Unable to retrieve GeoModelSvc" );
  }
  else {
    if(geoModel->clear().isFailure()) {
      ATH_MSG_WARNING( " ----> GeoModelSvc::clear() failed" );
    }
    else {
      ATH_MSG_INFO( " ----> GeoModelSvc::clear() succeeded " );
    }
  }
  m_releaseGeoModel=false; // Don't do that again...
  return;
}

void G4FaserAlg::commandLog(int returnCode, const std::string& commandString) const
{
  switch(returnCode) {
  case 0: { ATH_MSG_DEBUG("G4 Command: " << commandString << " - Command Succeeded"); } break;
  case 100: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Command Not Found!"); } break;
  case 200: {
    auto* stateManager = G4StateManager::GetStateManager();
    ATH_MSG_DEBUG("G4 Command: " << commandString << " - Illegal Application State (" <<
                    stateManager->GetStateString(stateManager->GetCurrentState()) << ")!");
  } break;
  case 300: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Parameter Out of Range!"); } break;
  case 400: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Parameter Unreadable!"); } break;
  case 500: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Parameter Out of Candidates!"); } break;
  case 600: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Alias Not Found!"); } break;
  default: { ATH_MSG_ERROR("G4 Command: " << commandString << " - Unknown Status!"); } break;
  }

}