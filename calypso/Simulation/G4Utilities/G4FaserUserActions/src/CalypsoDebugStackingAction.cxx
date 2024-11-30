/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System includes
#include <iostream>
#include <memory>
#include <string>

// Local includes
#include "CalypsoDebugStackingAction.h"

// Truth includes
#include "FaserMCTruth/FaserPrimaryParticleInformation.h"
#include "FaserMCTruth/FaserTrackInformation.h"
#include "FaserMCTruth/FaserTrackBarcodeInfo.h"
#include "FaserMCTruth/FaserG4EventUserInfo.h"

// Geant4 includes
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"


namespace G4UA
{

  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  CalypsoDebugStackingAction::CalypsoDebugStackingAction(const Config& config):
    CalypsoStackingAction(config) {}

  //---------------------------------------------------------------------------
  // Classify a new track
  //---------------------------------------------------------------------------
  G4ClassificationOfNewTrack
  CalypsoDebugStackingAction::ClassifyNewTrack(const G4Track* track)
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

    // Was track subject to a RR?
    // bool rouletted = false;

    // // Neutron Russian Roulette
    // if (m_config.russianRouletteNeutronThreshold > 0 && isNeutron(track) &&
    //     track->GetWeight() < m_config.russianRouletteNeutronWeight && // do not re-Roulette particles
    //     track->GetKineticEnergy() < m_config.russianRouletteNeutronThreshold) {
    //   // shoot random number
    //   if ( CLHEP::RandFlat::shoot() > m_oneOverWeightNeutron ) {
    //     if (m_config.applyNRR) {
    //       // Kill (w-1)/w neutrons
    //       return fKill;
    //     } else {
    //       // process them at the end of the stack
    //       return fWaiting;
    //     }
    //   }
    //   rouletted = true;
    //   // Weight the rest 1/w neutrons with a weight of w
    //   if (m_config.applyNRR) {
    //     mutableTrack->SetWeight(m_config.russianRouletteNeutronWeight);
    //   }
    // }

    // // Photon Russian Roulette
    // if (m_config.russianRoulettePhotonThreshold > 0 && isGamma(track) && track->GetOriginTouchable() &&
    //     track->GetOriginTouchable()->GetVolume()->GetName().substr(0, 3) == "LAr" && // only for photons created in LAr
    //     track->GetWeight() < m_config.russianRoulettePhotonWeight && // do not re-Roulette particles
    //     track->GetKineticEnergy() < m_config.russianRoulettePhotonThreshold) {
    //   // shoot random number
    //   if ( CLHEP::RandFlat::shoot() > m_oneOverWeightPhoton ) {
    //     if (m_config.applyPRR) {
    //       // Kill (w-1)/w photons
    //       return fKill;
    //     } else {
    //       // process them at the end of the stack
    //       return fWaiting;
    //     }
    //   }
    //   rouletted = true;
    //   // Weight the rest 1/w neutrons with a weight of w
    //   if (m_config.applyPRR) {
    //     mutableTrack->SetWeight(m_config.russianRoulettePhotonWeight);
    //   }
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
    // Put rouletted tracks at the end of the stack
    // if (rouletted)
    //   return fWaiting;
    // else
      return fUrgent;
  }

} // namespace G4UA
