/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   Emulsion Sensitive Detector class
****************************************************************/

#ifndef EMULSIONG4_SD_EMULSIONSENSORSD_H
#define EMULSIONG4_SD_EMULSIONSENSORSD_H

// Base class
#include "G4VSensitiveDetector.hh"

// For the hits
#include "NeutrinoSimEvent/NeutrinoHitCollection.h"
#include "StoreGate/WriteHandle.h"

// G4 needed classes
class G4Step;
class G4TouchableHistory;

// G4 needed classes
#include "Geant4/G4EnergyLossTables.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4MaterialCutsCouple.hh"

class EmulsionSensorSD : public G4VSensitiveDetector
{
public:
  // Constructor
  EmulsionSensorSD(const std::string& name, const std::string& hitCollectionName);

  // Destructor
  ~EmulsionSensorSD() { /* If all goes well we do not own myHitColl here */ }

  // Deal with each G4 hit
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;

  // For setting up the hit collection
  void Initialize(G4HCofThisEvent*) override final;

  /** Templated method to stuff a single hit into the sensitive detector class.  This
      could get rather tricky, but the idea is to allow fast simulations to use the very
      same SD classes as the standard simulation. */
  template <class... Args> void AddHit(Args&&... args){ m_HitColl->Emplace( args... ); }

private:
  void indexMethod(const G4TouchableHistory *myTouch, int &module, int &base, int &film);

protected:
  // The hits collection
  SG::WriteHandle<NeutrinoHitCollection> m_HitColl;
  
};

#endif //EMULSIONG4_SD_EMULSIONSENSORSD_H
