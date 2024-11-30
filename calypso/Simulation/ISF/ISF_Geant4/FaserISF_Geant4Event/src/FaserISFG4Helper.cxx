/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "FaserISF_Geant4Event/FaserISFG4Helper.h"

// Geant4 includes
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

// G4Atlas includes
#include "FaserMCTruth/FaserEventInformation.h"
#include "FaserMCTruth/FaserTrackBarcodeInfo.h"
#include "FaserMCTruth/FaserTrackInformation.h"

// ISF includes
#include "FaserISF_Event/FaserISFParticle.h"

// ATLAS GeoPrimitves (Amg::Vector3D etc)
#include "GeoPrimitives/GeoPrimitives.h"


/** convert the given G4Track into an ISFParticle */
ISF::FaserISFParticle*
iGeant4::FaserISFG4Helper::convertG4TrackToISFParticle(const G4Track& aTrack,
                                                       const ISF::FaserISFParticle& parent,
                                                       ISF::TruthBinding* truth)
{
  const G4ThreeVector& g4pos = aTrack.GetPosition();
  const double         gTime = aTrack.GetGlobalTime();
  const Amg::Vector3D  position(g4pos.x(),g4pos.y(),g4pos.z());
  
  const G4ThreeVector& g4mom = aTrack.GetMomentum();
  const Amg::Vector3D  momentum(g4mom.x(),g4mom.y(),g4mom.z());
  
  const G4ParticleDefinition &particleDefinition = *aTrack.GetDefinition();
  double mass    = particleDefinition.GetPDGMass();
  double charge  = particleDefinition.GetPDGCharge();
  int    pdgID   = particleDefinition.GetPDGEncoding();

  Barcode::ParticleBarcode barcode = (truth && truth->getTruthParticle() ) ?  HepMC::barcode(truth->getTruthParticle()) : Barcode::fUndefinedBarcode;


  ISF::FaserISFParticle *isp = new ISF::FaserISFParticle( position,
                                                momentum,
                                                mass,
                                                charge,
                                                pdgID,
                                                1,
                                                gTime,
                                                parent,
                                                barcode,
                                                truth
                                               );

  return isp;
}


/** return a valid UserInformation object of the G4Track for use within the ISF */
FaserVTrackInformation *
iGeant4::FaserISFG4Helper::getISFTrackInfo(const G4Track& aTrack) 
{
  FaserVTrackInformation* trackInfo = static_cast<FaserVTrackInformation*>(aTrack.GetUserInformation());
  return trackInfo;
}


/** link the given G4Track to the given ISFParticle */
FaserTrackInformation*
iGeant4::FaserISFG4Helper::attachTrackInfoToNewG4Track( G4Track& aTrack,
                                                        ISF::FaserISFParticle& baseIsp,
                                                        TrackClassification classification,
                                                        HepMC::GenParticlePtr nonRegeneratedTruthParticle)
{
  if ( aTrack.GetUserInformation() ) {
    G4ExceptionDescription description;
    description << G4String("FaserISFG4Helper::attachTrackInfoToNewG4Track: ")
                << "Trying to attach new FaserTrackInformation object to G4Track which already has a TrackUserInformation attached (trackID: "
                << aTrack.GetTrackID() << ", track pos: "<<aTrack.GetPosition() << ", mom: "<<aTrack.GetMomentum()
                << ", parentID " << aTrack.GetParentID() << ")";
    G4Exception("FaserISFG4Helper::attachTrackInfoToNewG4Track", "TrackUserInformationAlreadyExists", FatalException, description);
    return nullptr;
  }

  auto* truthBinding = baseIsp.getTruthBinding();
  if ( !truthBinding ) {
    G4ExceptionDescription description;
    description << G4String("FaserISFG4Helper::attachTrackInfoToNewG4Track: ")
                << "No TruthBinding present in base FaserISFParticle (trackID: "
                << aTrack.GetTrackID() << ", track pos: "<<aTrack.GetPosition() << ", mom: "<<aTrack.GetMomentum()
                << ", parentID " << aTrack.GetParentID() << ", FaserISFParticle: "<<baseIsp<<")";
    G4Exception("FaserISFG4Helper::attachTrackInfoToNewG4Track", "NoISFTruthBinding", FatalException, description);
    return nullptr;
  }

  FaserTrackInformation *trackInfo = new FaserTrackInformation( nonRegeneratedTruthParticle, &baseIsp );
  auto primaryTruthParticle   = truthBinding->getPrimaryTruthParticle();

  trackInfo->SetPrimaryHepMCParticle( primaryTruthParticle );
  trackInfo->SetClassification( classification );
  aTrack.SetUserInformation( trackInfo );

  return trackInfo;
}

/** return pointer to current EventInformation */
FaserEventInformation*
iGeant4::FaserISFG4Helper::getEventInformation()
{
  return ( static_cast<FaserEventInformation*> (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation()) );
}

