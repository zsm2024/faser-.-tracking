/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header for my class
#include "Pythia8Decayer.h"

#include "FaserMCTruth/FaserTrackInformation.h"

// For passing things around
#include "CLHEP/Vector/LorentzVector.h"
#include "G4Track.hh"
#include "G4DynamicParticle.hh"
#include "G4ParticleTable.hh"
#include "G4DecayProducts.hh"
#include "G4VProcess.hh"

Pythia8Decayer::Pythia8Decayer( const std::string s )
 : G4VExtDecayer(s)
{
  // In the constructor, make a decayer instance, so that it's initialized here and not in the event loop
  std::string docstring = Pythia8_i::xmlpath();
  m_decayer = std::make_unique<Pythia8::Pythia>(docstring);

  m_decayer->readString("ProcessLevel:all = off"); 

  m_decayer->readString("ProcessLevel:resonanceDecays=on");
    
  // shut off Pythia8 (default) verbosty
  //
  m_decayer->readString("Init:showAllSettings=false");
  m_decayer->readString("Init:showChangedSettings=false");
  m_decayer->readString("Init:showAllParticleData=false");
  m_decayer->readString("Init:showChangedParticleData=false");
  //
  // specify how many Py8 events to print out, at either level
  // in this particular case print out a maximum of 10 events
  //
  m_decayer->readString("Next:numberShowProcess = 0" );
  m_decayer->readString("Next:numberShowEvent = 10");
           
  m_decayer->init();
   
  // shut off decays of pi0's as we want Geant4 to handle them
  // if other immediate decay products should be handled by Geant4,
  // their respective decay modes should be shut off as well
  //
  m_decayer->readString("111:onMode = off");
}

G4DecayProducts* Pythia8Decayer::ImportDecayProducts(const G4Track& aTrack){

   m_decayer->event.reset();
   
   G4DecayProducts* dproducts = nullptr;   
   
   G4ParticleDefinition* pd = aTrack.GetDefinition();
   int    pdgid   = pd->GetPDGEncoding();
   
   // check if pdgid is consistent with Pythia8 particle table
   //   
   if ( !m_decayer->particleData.findParticle( pdgid ) )
   {
      G4cout << " can NOT find pdgid = " << pdgid 
             << " in Pythia8::ParticleData" << G4endl;
      return dproducts;
   }
   
   if ( !m_decayer->particleData.canDecay(pdgid) )
   {
      G4cout << " Particle of pdgid = " << pdgid 
             << " can NOT be decayed by Pythia8" << G4endl;
      return dproducts;
   }
   
   // NOTE: Energy should be in GeV 

   m_decayer->event.append( pdgid, 1, 0, 0, 
                           aTrack.GetMomentum().x() / CLHEP::GeV, 
                           aTrack.GetMomentum().y() / CLHEP::GeV,  
                           aTrack.GetMomentum().z() / CLHEP::GeV,
                           aTrack.GetDynamicParticle()->GetTotalEnergy() / CLHEP::GeV,
                           pd->GetPDGMass() / CLHEP::GeV );

   // specify polarization, if any
   
   // special logic for primary taus(anti-taus), assumed to have polarization -1(+1), respectively
   // verified from the polarization info in Genie output
   double spinup;
   FaserTrackInformation* info = dynamic_cast<FaserTrackInformation*>(aTrack.GetUserInformation());
   if (info != nullptr && abs(pdgid) == 15 && (info->GetClassification() == TrackClassification::Primary || info->GetClassification() == TrackClassification::RegeneratedPrimary))
   {
       G4cout << "Primary tau decay identified." << G4endl;
       spinup = (pdgid > 0 ? -1 : +1);
   }
   else
   // NOTE: while in Py8 polarization is a double variable , 
   //       in reality it's expected to be -1, 0., or 1 in case of "external" tau's, 
   //       similar to LHA SPINUP; see Particle Decays, Hadron and Tau Decays in docs at
   //       https://pythia.org/manuals/pythia8305/Welcome.html
   //       so it's not able to handle anything like 0.99, thus we're rounding off    
   {
       spinup = round( std::cos( aTrack.GetPolarization().angle( aTrack.GetMomentumDirection() ) ) );
   }
   G4cout << "Using " << aTrack.GetParticleDefinition()->GetParticleName() << " helicity " << spinup << " for Pythia8 decay." << G4endl;
   m_decayer->event.back().pol( spinup );

   int npart_before_decay = m_decayer->event.size();
   
   m_decayer->next();
   
   int npart_after_decay = m_decayer->event.size();
   
   // create & fill up decay products
   //
   dproducts = new G4DecayProducts(*(aTrack.GetDynamicParticle()));
   
   // create G4DynamicParticle out of each m_decayer->event entry (except the 1st one)
   // and push into dproducts
   
   for ( int ip=npart_before_decay; ip<npart_after_decay; ++ip )
   {
      
      // only select final state decay products (direct or via subsequent decays);
      // skip all others
      //
      // NOTE: in general, final state decays products will have 
      //       positive status code between 91 and 99 
      //       (in case such information could be of interest in the future)
      //
      if ( m_decayer->event[ip].status() < 0 ) continue;
            
      // should we also skip neutrinos ???
      // if so, skip products with abs(m_decayer->event[ip].id()) of 12, 14, or 16
            
      G4ParticleDefinition* pddec = 
         G4ParticleTable::GetParticleTable()->FindParticle( m_decayer->event[ip].id() );
      if ( !pddec ) continue; // maybe we should print out a warning !
      G4ThreeVector momentum = G4ThreeVector( m_decayer->event[ip].px() * CLHEP::GeV,
                                              m_decayer->event[ip].py() * CLHEP::GeV,
                                              m_decayer->event[ip].pz() * CLHEP::GeV ); 
      dproducts->PushProducts( new G4DynamicParticle( pddec, momentum) ); 
   }
   dproducts->DumpInfo();
   return dproducts;

}
