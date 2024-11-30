/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASERUSERACTIONS_CALYPSOSTACKINGACTION_H
#define G4FASERUSERACTIONS_CALYPSOSTACKINGACTION_H

#include "G4UserStackingAction.hh"

class FaserPrimaryParticleInformation;

namespace G4UA
{

  /// @class CalypsoStackingAction
  /// @brief Standard ATLAS stacking action functionality.
  /// Mostly taken from the old CalypsoStackingAction implementation, but
  /// adapted for the new user action design for multi-threading.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class CalypsoStackingAction : public G4UserStackingAction
  {

    public:

      /// Configuration option struct for CalypsoStackingAction.
      struct Config
      {
        /// Flag to toggle killing neutrinos at tracking stage
        bool killAllNeutrinos;
        /// Photon energy cut
        double photonEnergyCut;
        /// Apply the Neutron Russian Roulette
        // bool applyNRR;
        // /// Energy threshold for the Neutron Russian Roulette
        // double russianRouletteNeutronThreshold;
        // /// Weight for the Neutron Russian Roulette
        // double russianRouletteNeutronWeight;
        // /// Apply the Photon Russian Roulette
        // bool applyPRR;
        // /// Energy threshold for the Photon Russian Roulette
        // double russianRoulettePhotonThreshold;
        // /// Weight for the Photon Russian Roulette
        // double russianRoulettePhotonWeight;
        /// Is this an ISF job
        bool isISFJob;
      };
      
      /// Constructor with configuration
      CalypsoStackingAction(const Config& config);

      /// @brief Classify a new track.
      /// Result can be fUrgent, fWaiting, fPostpone, or fKill.
      virtual G4ClassificationOfNewTrack
      ClassifyNewTrack(const G4Track* track) override;

    protected:

      /// @brief Configuration options
      Config m_config;

      /// @brief Identify track as a neutrino.
      /// It might be useful to move this kind of functionality
      /// into some standalong helper function(s).
      bool isNeutrino(const G4Track*) const;

      /// @brief Identify track as a photon.
      bool isGamma(const G4Track*) const;

      /// @brief Identify track as a neutron.
      bool isNeutron(const G4Track*) const;

      /// @brief obtain the PrimaryParticleInformation from the current G4Track
      FaserPrimaryParticleInformation* getPrimaryParticleInformation(const G4Track *track) const;

    //   // one over m_config.russianRouletteNeutronWeight
    //   double m_oneOverWeightNeutron;
      
    //   // one over m_config.russianRoulettePhotonWeight
    //   double m_oneOverWeightPhoton;

  }; // class CalypsoStackingAction

} // namespace G4UA

#endif
