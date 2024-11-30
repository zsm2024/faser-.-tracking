/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "Pythia8DecayerPhysicsTool.h"
// package headers
#include "Pythia8Decayer.h"
// Geant4 headers
#include "globals.hh"
#include "G4ParticleTable.hh"
#include "G4VProcess.hh"
#include "G4ProcessManager.hh"
#include "G4Decay.hh"
#include "G4DecayTable.hh"

// STL headers
#include <string>

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Pythia8DecayerPhysicsTool::Pythia8DecayerPhysicsTool( const std::string& type,
                                                      const std::string& nam,
                                                      const IInterface* parent )
  : G4VPhysicsConstructor{nam}, base_class( type, nam , parent )
{
   m_physicsOptionType = G4AtlasPhysicsOption::Type::GlobalProcesses;
}

//=============================================================================
// Destructor
//=============================================================================

Pythia8DecayerPhysicsTool::~Pythia8DecayerPhysicsTool()
{
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode Pythia8DecayerPhysicsTool::initialize( )
{
  ATH_MSG_DEBUG("Pythia8DecayerPhysicsTool initialize( )");
  this->SetPhysicsName(name());
  return StatusCode::SUCCESS;
}

Pythia8DecayerPhysicsTool* Pythia8DecayerPhysicsTool::GetPhysicsOption()
{
  return this;
}


void Pythia8DecayerPhysicsTool::ConstructParticle()
{
    // nothing to do here
}
void Pythia8DecayerPhysicsTool::ConstructProcess()
{
  ATH_MSG_DEBUG("Pythia8DecayerPhysicsTool::ConstructProcess() called ");

   Pythia8Decayer* extDecayer = new Pythia8Decayer(name());

   auto particleIterator=GetParticleIterator();
   particleIterator->reset();
   while ((*particleIterator)())
   {    
      G4ParticleDefinition* particle = particleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();    
      G4ProcessVector* processVector = pmanager->GetProcessList();
      for ( size_t i=0; i<processVector->length(); ++i ) 
      {    
         G4Decay* decay = dynamic_cast<G4Decay*>((*processVector)[i]);
         if ( decay ) 
         {
            // remove native/existing decay table for
            // a)tau's 
            // b) B+/- 
            // and replace with external decayer
            if ( std::abs(particle->GetPDGEncoding()) == 15 ||
                 std::abs(particle->GetPDGEncoding()) == 521 )
            {
               if ( particle->GetDecayTable() )
               {
                  delete particle->GetDecayTable();
                  particle->SetDecayTable(nullptr);
               }
               decay->SetExtDecayer(extDecayer);
               G4cout << "Setting ext decayer for: " 
                <<  particleIterator->value()->GetParticleName()
                << G4endl;
            }
            // now set external decayer to all particles 
            // that don't yet have a decay table
            if ( !particle->GetDecayTable() )
            {
               decay->SetExtDecayer(extDecayer);
               G4cout << "Setting ext decayer for: " 
                <<  particleIterator->value()->GetParticleName()
                << G4endl;
            }
         }
      }              
   }

   return;
   
}
