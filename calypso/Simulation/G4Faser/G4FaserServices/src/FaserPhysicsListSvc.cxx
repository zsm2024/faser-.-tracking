/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserPhysicsListSvc.h"

#include "G4VUserPhysicsList.hh"
#include "G4StateManager.hh"
#include "G4RunManager.hh"
#include "G4EmParameters.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"
// #include "G4AntiNeutron.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include "G4PhysicsLists/AtlasPhysListFactory.h"

#include <limits>

FaserPhysicsListSvc::FaserPhysicsListSvc(const std::string& name, ISvcLocator* pSvcLocator)
  : base_class(name,pSvcLocator)
{
}


StatusCode FaserPhysicsListSvc::initialize( )
{
  ATH_MSG_DEBUG("FaserPhysicsListSvc::initialize()");
  if (m_phys_option.size())
    {
      ATH_MSG_INFO( "Initializing list of " <<  m_phys_option.size() << " physics options"  );
      CHECK( m_phys_option.retrieve() );
    }

  if (m_phys_decay.size())
    {
      ATH_MSG_INFO( "Initializing list of " <<  m_phys_decay.size() << " Decays "  );
      CHECK( m_phys_decay.retrieve() );
    }

  return StatusCode::SUCCESS;
}


void FaserPhysicsListSvc::CreatePhysicsList()
{
  ATH_MSG_DEBUG("FaserPhysicsListSvc::CreatePhysicsList()");
  if (m_physicsListName.value() != ""){
    G4PhysListFactory factory;
    AtlasPhysListFactory Atlasfactory;
    if (factory.IsReferencePhysList(m_physicsListName.value()))
    {
      ATH_MSG_INFO("Creating Geant4 PhysicsList: " << m_physicsListName.value());
      m_physicsList = factory.GetReferencePhysList(m_physicsListName.value());
    }
    else if (Atlasfactory.IsReferencePhysList(m_physicsListName.value()))
    {
      ATH_MSG_INFO("Creating ATLAS PhysicsList: " << m_physicsListName.value());
      m_physicsList = Atlasfactory.GetReferencePhysList(m_physicsListName.value());
    }
  }

  if (!m_physicsList)
    {
      ATH_MSG_ERROR("Unable to initialize physics List: " << m_physicsList);
      throw "PhysicsListInitializationError";
    }
  // Call these as functions.  As this could be used as a base class, having
  //   these as separate functions lets someone who is inheriting from this use
  //   them...

  // sort m_phys_option list
  std::vector<IPhysicsOptionTool*> sortedPhysicsOptions;
  sortedPhysicsOptions.reserve(m_phys_option.size());
  // Manually sorting ToolHandleArray
  {
    // BSM Physics
    for (auto& physOptTool: m_phys_option) {
      if (physOptTool->GetOptionType() == G4AtlasPhysicsOption::Type::BSMPhysics) {
        sortedPhysicsOptions.push_back(&*physOptTool);
      }
    }

    // Add particles from the PDG Table not currently known to Geant4
    for (auto& physOptTool: m_phys_option) {
      if (physOptTool->GetOptionType() == G4AtlasPhysicsOption::Type::QS_ExtraParticles) {
        sortedPhysicsOptions.push_back(&*physOptTool);
      }
    }

    // Add MSC and Ionisation processes for specific particles (possibly merge with the next one?)
    for (auto& physOptTool: m_phys_option) {
      if (physOptTool->GetOptionType() == G4AtlasPhysicsOption::Type::QS_ExtraProc) {
        sortedPhysicsOptions.push_back(&*physOptTool);
      }
    }

    // G4StepLimitation, LUCID Op Process, TRT XTR process
    for (auto& physOptTool: m_phys_option) {
      if (physOptTool->GetOptionType() == G4AtlasPhysicsOption::Type::GlobalProcesses) {
        sortedPhysicsOptions.push_back(&*physOptTool);
      }
    }

    // Unknown
    for (auto& physOptTool: m_phys_option) {
      if (physOptTool->GetOptionType() == G4AtlasPhysicsOption::Type::UnknownType) {
        ATH_MSG_ERROR(physOptTool->name() << "set as UnknownType. This tool will not be used to modify the physics list of this job.");
      }
    }
  }

  //Register physics options to the G4VModularPhysicsList
  for (auto& physOptTool: sortedPhysicsOptions)
    {
      ATH_MSG_DEBUG("Registering " << physOptTool->name());
      m_physicsList->RegisterPhysics(physOptTool->GetPhysicsOption());
    }
  //Register decays to the G4VModularPhysicsList
  for (auto& physDecayTool: m_phys_decay)
    {
      ATH_MSG_DEBUG("Registering " << physDecayTool->name());
      m_physicsList->RegisterPhysics(physDecayTool->GetPhysicsOption());
    }

  //ConstructProcess();
  ATH_MSG_DEBUG("end of FaserPhysicsListSvc::CreatePhysicsList()");
}


G4VUserPhysicsList* FaserPhysicsListSvc::GetPhysicsList()
{
  if (!m_physicsList) {
    this->CreatePhysicsList();
  }
  return m_physicsList;
}


void FaserPhysicsListSvc::SetPhysicsList()
{
  if(!m_physicsList) {
    this->CreatePhysicsList();
  }
  G4RunManager::GetRunManager()->SetUserInitialization(m_physicsList);
}


void FaserPhysicsListSvc::SetPhysicsOptions()
{
  if (!m_physicsList)
    {
      ATH_MSG_WARNING("Physics list not initialized before calling ConstructProcess()");
      return;
    }

  if(m_generalCut.value() > 0. && std::abs(m_generalCut.value())>std::numeric_limits<double>::epsilon())
    {
      m_physicsList->SetDefaultCutValue(m_generalCut.value());
    }

  std::vector<std::string> g4commands;
  if (m_neutronTimeCut.value() > 0. && std::abs(m_neutronTimeCut.value())>std::numeric_limits<double>::epsilon())
    {
      std::ostringstream oss;
      oss<<"/physics_engine/neutron/timeLimit "<<m_neutronTimeCut.value()<<" ns";
      g4commands.push_back(oss.str());
    }

  if (m_neutronEnergyCut.value() > 0. && std::abs(m_neutronEnergyCut.value())>std::numeric_limits<double>::epsilon())
    {
      std::ostringstream oss;
      oss<<"/physics_engine/neutron/energyLimit "<<m_neutronEnergyCut.value()<<" MeV";
      g4commands.push_back(oss.str());
    }

  if(!g4commands.empty()) {
    // Send UI commands
    ATH_MSG_DEBUG("G4 Command: Trying in SetPhysicsOptions()");
    G4UImanager* ui = G4UImanager::GetUIpointer();
    for (const auto& g4command : g4commands) {
      int returnCode = ui->ApplyCommand( g4command );
      CommandLog(returnCode, g4command);
    }
  }

  G4EmParameters* emp = G4EmParameters::Instance();
  if (m_emMaxEnergy.value()>=0) emp->SetMaxEnergy(m_emMaxEnergy.value());
  if (m_emNumberOfBinsPerDecade.value()>=0) emp->SetNumberOfBinsPerDecade(m_emNumberOfBinsPerDecade.value());
  if (m_emMinEnergy.value()>=0) emp->SetMinEnergy(m_emMinEnergy.value());
  if (m_applyEMCuts.value())
    {
      emp->SetApplyCuts(true);
    }

//   if (m_unstableAntiNeutrons) {
//     G4AntiNeutron::Definition()->SetPDGStable(false);
//   }

  return;
}


void FaserPhysicsListSvc::CommandLog(int returnCode, const std::string& commandString) const
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
