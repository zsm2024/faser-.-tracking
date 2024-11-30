/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   SCT Sensitive Detector class
****************************************************************/

#ifndef FASERSCT_G4_SD_FASERSCTSENSORSD_H
#define FASERSCT_G4_SD_FASERSCTSENSORSD_H

// Base class
#include "G4VSensitiveDetector.hh"

// For the hits
#include "TrackerSimEvent/FaserSiHitCollection.h"
#include "StoreGate/WriteHandle.h"

// G4 needed classes
class G4Step;
class G4TouchableHistory;

class FaserSctSensorSD : public G4VSensitiveDetector
{
public:
  // Constructor
  FaserSctSensorSD(const std::string& name, const std::string& hitCollectionName);

  // Destructor
  ~FaserSctSensorSD() { /* If all goes well we do not own myHitColl here */ }

  // Deal with each G4 hit
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;

  // For setting up the hit collection
  void Initialize(G4HCofThisEvent*) override final;

  /** Templated method to stuff a single hit into the sensitive detector class.  This
      could get rather tricky, but the idea is to allow fast simulations to use the very
      same SD classes as the standard simulation. */
  template <class... Args> void AddHit(Args&&... args){ m_HitColl->Emplace( args... ); }

private:
  void indexMethod(const G4TouchableHistory *myTouch, int &station, int &plane, int &row, int &module, int &sensor);
protected:
  // The hits collection
  SG::WriteHandle<FaserSiHitCollection> m_HitColl;
};

#endif //FASERSCT_G4_SD_FASERSCTSENSORSD_H
