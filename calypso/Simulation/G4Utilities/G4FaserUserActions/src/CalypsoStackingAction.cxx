/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System includes
#include <iostream>
#include <memory>
#include <string>

// Local includes
#include "CalypsoStackingAction.h"

// Truth includes
#include "FaserMCTruth/FaserPrimaryParticleInformation.h"
#include "FaserMCTruth/FaserTrackInformation.h"
#include "FaserMCTruth/FaserTrackBarcodeInfo.h"
#include "FaserMCTruth/FaserG4EventUserInfo.h"

// Geant4 includes
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4NeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4AntiNeutrinoTau.hh"
#include "G4Gamma.hh"
#include "G4Neutron.hh"


namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  CalypsoStackingAction::CalypsoStackingAction(const Config& config):
    m_config(config) //,
    // m_oneOverWeightNeutron(0),
    // m_oneOverWeightPhoton(0)
  {
    // calculate this division only once
    // if (m_config.applyNRR)
    //   m_oneOverWeightNeutron = 1./m_config.russianRouletteNeutronWeight;

    // // calculate this division only once
    // if (m_config.applyPRR)
    //   m_oneOverWeightPhoton = 1./m_config.russianRoulettePhotonWeight;
  }

  //---------------------------------------------------------------------------
  // Classify a new track
  //---------------------------------------------------------------------------
  G4ClassificationOfNewTrack
  CalypsoStackingAction::ClassifyNewTrack(const G4Track* track)
  {
    // Kill neutrinos if enabled
    if(m_config.killAllNeutrinos && isNeutrino(track)) {
      return fKill;
    }

    // Kill super-low-E photons
    const double safeCut = 0.00005;
    double totalE = track->GetTotalEnergy();
    if(isGamma(track) && totalE < safeCut) {
      return fKill;
    }

    // TODO: Why is this here? Can I remove it?
    G4Event* ev = G4EventManager::GetEventManager()->GetNonconstCurrentEvent();
    FaserG4EventUserInfo* eventInfo __attribute__ ((unused)) =
      static_cast<FaserG4EventUserInfo*> (ev->GetUserInformation());

    // Neutron Russian Roulette
    // if (m_config.applyNRR && isNeutron(track) &&
    //     track->GetWeight() < m_config.russianRouletteNeutronWeight && // do not re-Roulette particles
    //     track->GetKineticEnergy() < m_config.russianRouletteNeutronThreshold) {
    //   // shoot random number
    //   if ( CLHEP::RandFlat::shoot() > m_oneOverWeightNeutron ) {
    //     // Kill (w-1)/w neutrons
    //     return fKill;
    //   }
    //   // Weight the rest 1/w neutrons with a weight of w
    //   mutableTrack->SetWeight(m_config.russianRouletteNeutronWeight);
    // }

    // // Photon Russian Roulette
    // if (m_config.applyPRR && isGamma(track) && track->GetOriginTouchable() &&
    //     track->GetOriginTouchable()->GetVolume()->GetName().substr(0, 3) == "LAr" && // only for photons created in LAr
    //     track->GetWeight() < m_config.russianRoulettePhotonWeight && // do not re-Roulette particles
    //     track->GetKineticEnergy() < m_config.russianRoulettePhotonThreshold) {
    //   // shoot random number
    //   if ( CLHEP::RandFlat::shoot() > m_oneOverWeightPhoton ) {
    //     // Kill (w-1)/w photons
    //     return fKill;
    //   }
    //   // Weight the rest 1/w neutrons with a weight of w
    //   mutableTrack->SetWeight(m_config.russianRoulettePhotonWeight);
    // }

    // Handle primary particles
    if(track->GetParentID() == 0) { // Condition for Primaries
      // Extract the PrimaryParticleInformation
      FaserPrimaryParticleInformation* ppi = this->getPrimaryParticleInformation(track);
      // Fill some information for this track
      if(ppi) {
        if (!m_config.isISFJob) {
          // don't do anything
          auto part = ppi->GetHepMCParticle();
          if(part) {
            // OK, we got back to HepMC
            std::unique_ptr<FaserTrackInformation> ti = std::make_unique<FaserTrackInformation>(part);
            ti->SetRegenerationNr(0);
            ti->SetClassification(Primary);
            // regNr=0 and classify=Primary are default values anyway
            track->SetUserInformation(ti.release()); /// Pass ownership to mutableTrack
          }
          // What does this condition mean?
          else if(ppi->GetParticleBarcode() >= 0 && ppi->GetParticleBarcode() >= 0) {
            // PrimaryParticleInformation should at least provide a barcode
            std::unique_ptr<FaserTrackBarcodeInfo> bi = std::make_unique<FaserTrackBarcodeInfo>(ppi->GetParticleUniqueID(), ppi->GetParticleBarcode());
            track->SetUserInformation(bi.release()); /// Pass ownership to mutableTrack
          }
        } // no ISFParticle attached
      } // has PrimaryParticleInformation
    }
    // Secondary track; decide whether to save or kill
    else if( isGamma(track) &&
             m_config.photonEnergyCut > 0 &&
             totalE < m_config.photonEnergyCut )
      {
        return fKill;
      }
    return fUrgent;
  }

  FaserPrimaryParticleInformation* CalypsoStackingAction::getPrimaryParticleInformation(const G4Track *track) const
  {
    const G4DynamicParticle* dp = track->GetDynamicParticle();
    if(dp) {
      const G4PrimaryParticle* pp = nullptr;
      pp = dp->GetPrimaryParticle();
      if(pp) {
        // Extract the PrimaryParticleInformation
        return dynamic_cast<FaserPrimaryParticleInformation*>
          ( pp->GetUserInformation() );
      }
    }
    return nullptr;
  }

  //---------------------------------------------------------------------------
  // Identify track definition
  //---------------------------------------------------------------------------
  bool CalypsoStackingAction::isNeutrino(const G4Track* track) const
  {
    auto particleDef = track->GetParticleDefinition();
    return (particleDef == G4NeutrinoE::NeutrinoEDefinition()           ||
            particleDef == G4AntiNeutrinoE::AntiNeutrinoEDefinition()   ||
            particleDef == G4NeutrinoMu::NeutrinoMuDefinition()         ||
            particleDef == G4AntiNeutrinoMu::AntiNeutrinoMuDefinition() ||
            particleDef == G4NeutrinoTau::NeutrinoTauDefinition()       ||
            particleDef == G4AntiNeutrinoTau::AntiNeutrinoTauDefinition());
  }

  //---------------------------------------------------------------------------
  bool CalypsoStackingAction::isGamma(const G4Track* track) const
  {
    return track->GetParticleDefinition() == G4Gamma::Gamma();
  }

  //---------------------------------------------------------------------------
  bool CalypsoStackingAction::isNeutron(const G4Track* track) const
  {
    return track->GetParticleDefinition() == G4Neutron::Neutron();
  }

} // namespace G4UA
