/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_FASERISFG4HELPER_H
#define ISF_GEANT4TOOLS_FASERISFG4HELPER_H

// Barcode includes
#include "BarcodeEvent/Barcode.h"

// ISF Includes
#include "FaserISF_Event/FaserISFParticle.h"

// MCTruth includes
#include "FaserMCTruth/FaserVTrackInformation.h"

// forward declarations
#include "AtlasHepMC/GenParticle_fwd.h"

namespace ISF {
  class TruthBinding;
}
class FaserVTrackInformation;
class FaserTrackInformation;
class FaserEventInformation;
class G4Track;

namespace iGeant4 {

class FaserISFG4Helper {

 public:
  FaserISFG4Helper() = delete;
  
  /** convert the given G4Track into an ISFParticle */
  static ISF::FaserISFParticle* convertG4TrackToISFParticle(const G4Track& aTrack,
                                                       const ISF::FaserISFParticle& parent,
                                                       ISF::TruthBinding* truth = nullptr);
  
  /** return a valid UserInformation object of the G4Track for use within the ISF */
  static FaserVTrackInformation* getISFTrackInfo(const G4Track& aTrack);
  
  /** link the given G4Track to the given ISFParticle */
  static void setG4TrackInfoFromBaseISFParticle( G4Track& aTrack,
                                                 const ISF::FaserISFParticle& baseIsp,
                                                 bool setReturnToISF=false );
  
  /** attach a new TrackInformation object to the given new (!) G4Track
   *  (the G4Track must not have a UserInformation object attached to it) */
  static FaserTrackInformation* attachTrackInfoToNewG4Track( G4Track& aTrack,
                                   ISF::FaserISFParticle& baseIsp,
                                   TrackClassification classification,
                                   HepMC::GenParticlePtr nonRegeneratedTruthParticle = nullptr);
  
  /** return pointer to current EventInformation */
  static FaserEventInformation* getEventInformation();
 
 private:
 
};
}

#endif // ISF_GEANT4TOOLS_FASERISFG4HELPER_H
