/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "FaserISF_Geant4Event/FaserGeant4TruthIncident.h"

// package includes
#include "FaserISF_Geant4Event/FaserISFG4Helper.h"

// Atlas G4 Helpers
#include "FaserMCTruth/FaserG4EventUserInfo.h"
#include "FaserMCTruth/FaserTrackBarcodeInfo.h"
#include "FaserMCTruth/FaserTrackHelper.h"
#include "FaserMCTruth/FaserTrackInformation.h"
#include "FaserMCTruth/FaserPrimaryParticleInformation.h"

// Units
#include "GaudiKernel/PhysicalConstants.h"

// HepMC includes
#include "AtlasHepMC/GenParticle.h"
#include "TruthUtils/MagicNumbers.h"

// Geant4 includes
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4VProcess.hh"

#include "G4TrackStatus.hh"
#include "G4ProcessType.hh"
#include "G4EmProcessSubType.hh"
#include "G4DynamicParticle.hh"
#include "G4PrimaryParticle.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"

// ISF includes
#include "FaserISF_Event/FaserISFParticle.h"



/*
  Comments:
  what about parent particle surviving (e.g. bremstrahlung)


  Existing code:
  Simulation/G4Sim/SimHelpers/src/StepHelper.cxx
  - provids convenient information of a G4Step
  Simulation/G4Utilities/G4TruthStrategies/src/BremsstrahlungStrategy.cxx
  - retrives information from a G4Step (via StepHelper)
  Simulation/G4Sim/FaserMCTruth/MCTruth/TruthStrategy.h
  - common base for different truth strategies
  Simulation/G4Sim/FaserMCTruth/src/EventInformation.cxx
  - stores HepMCevent in G4
  Simulation/G4Sim/FaserMCTruth/src/TrackInformation.cxx
  Simulation/G4Sim/FaserMCTruth/src/TrackHelper.cxx
  - store/manage barcode
*/


iGeant4::FaserGeant4TruthIncident::FaserGeant4TruthIncident( const G4Step *step,
                                               const ISF::FaserISFParticle& baseISP,
                                               FaserDetDescr::FaserRegion geoID,
                                               FaserG4EventUserInfo *faserG4EventUserInfo) :
  IFaserTruthIncident(geoID, step->GetSecondaryInCurrentStep()->size()), // switch to G4Step::GetNumberOfSecondariesInCurrentStep() once we're using G4 10.2 or later
  m_positionSet(false),
  m_position(),
  m_step(step),
  m_baseISP(baseISP),
  m_faserG4EvtUserInfo(faserG4EventUserInfo)
{
  // prepare children:
  prepareChildren();

  // calculate position:
  const G4StepPoint *postStepPoint = m_step->GetPostStepPoint();
  const G4ThreeVector         &pos = postStepPoint->GetPosition();
  const G4double              time = postStepPoint->GetGlobalTime()*Gaudi::Units::c_light;
  m_position.set( pos.x(), pos.y(), pos.z(), time );
}


const HepMC::FourVector& iGeant4::FaserGeant4TruthIncident::position() const 
{
  return m_position;
}

int iGeant4::FaserGeant4TruthIncident::physicsProcessCategory() const {
  const G4VProcess *process = m_step->GetPostStepPoint()->GetProcessDefinedStep();
  // TODO: need to check that G4ProcessSubTypes Match Barcode::PhysicsProcessCode
  return process->GetProcessType();
}

Barcode::PhysicsProcessCode iGeant4::FaserGeant4TruthIncident::physicsProcessCode() const {
  const G4VProcess *process = m_step->GetPostStepPoint()->GetProcessDefinedStep();
  // TODO: need to check that G4ProcessSubTypes Match Barcode::PhysicsProcessCode
  return process->GetProcessSubType();
}

double iGeant4::FaserGeant4TruthIncident::parentP2() const {
  const G4ThreeVector & mom= m_step->GetPreStepPoint()->GetMomentum();
  return mom.mag2();
}

double iGeant4::FaserGeant4TruthIncident::parentPt2() const {
  const G4ThreeVector & mom= m_step->GetPreStepPoint()->GetMomentum();
  return mom.perp2();
}

double iGeant4::FaserGeant4TruthIncident::parentEkin() const {
  return (m_step->GetPreStepPoint()->GetKineticEnergy()/CLHEP::MeV);
}

int iGeant4::FaserGeant4TruthIncident::parentPdgCode() const {
  return  m_step->GetTrack()->GetDefinition()->GetPDGEncoding();
}

Barcode::ParticleBarcode iGeant4::FaserGeant4TruthIncident::parentBarcode() {
  auto parent = parentParticle();

  return (parent) ? HepMC::barcode(parent) : Barcode::fUndefinedBarcode;
}

int iGeant4::FaserGeant4TruthIncident::parentStatus()  {
 auto parent = parentParticle();

 return (parent) ? parent->status() : 0;
}

HepMC::GenParticlePtr iGeant4::FaserGeant4TruthIncident::parentParticle() {
  return m_faserG4EvtUserInfo->GetCurrentlyTraced();
}

int iGeant4::FaserGeant4TruthIncident::parentBCID() const { 
  return m_baseISP.getBCID();
}


bool iGeant4::FaserGeant4TruthIncident::parentSurvivesIncident() const { 
  const G4Track *track = m_step->GetTrack();

  // check if particle is a alive in G4 or in ISF
  if ( particleAlive(track) ) {
    return true;
  } else {
    return false;
  }
}

HepMC::GenParticlePtr iGeant4::FaserGeant4TruthIncident::parentParticleAfterIncident(Barcode::ParticleBarcode newBarcode) {
  const G4Track *track = m_step->GetTrack();

  // check if particle is a alive in G4 or in ISF
  if ( !parentSurvivesIncident() ) {
    return nullptr;
  }

  // internally cache the HepMC representation of the parent particle
  // (to allow multiple calls to this method on the same instance)
  if ( !m_parentParticleAfterIncident ) {
    // create new HepMC particle, using momentum and energy
    // from G4DynamicParticle (which should be equivalent to postStep)
    m_parentParticleAfterIncident = convert(track, newBarcode, false);
    
    m_faserG4EvtUserInfo->SetCurrentlyTraced( m_parentParticleAfterIncident );
    
    // store (new) hepmc particle in track's UserInformation
    FaserTrackHelper       tHelper(track);
    FaserTrackInformation *tInfo = tHelper.GetTrackInformation();
    if (tInfo) {
      // do NOT update the TrackInformation for regenerated particles!
      // (most recent truth info is kept in AtlasG4EventUserInfo)
      //tInfo->SetParticle( m_parentParticleAfterIncident );
      int regenerationNr = tInfo->GetRegenerationNr();
      regenerationNr++;
      tInfo->SetRegenerationNr(regenerationNr);
      if ( tHelper.IsPrimary() ) { 
        tInfo->SetClassification(RegeneratedPrimary);
      }
    }

  } // <-- end if m_parentParticleAfterIncident is not filled


  // return the HepMC particle representation of the parent
  return m_parentParticleAfterIncident;
}

double iGeant4::FaserGeant4TruthIncident::childP2(unsigned short i) const {
  const G4ThreeVector & mom= m_children[i]->GetMomentum();
  return mom.mag2();
}

const G4ThreeVector iGeant4::FaserGeant4TruthIncident::childP(unsigned short i) const {
  const G4ThreeVector & mom= m_children[i]->GetMomentum();
  return mom;
}

double iGeant4::FaserGeant4TruthIncident::childPt2(unsigned short i) const {
  const G4ThreeVector & mom= m_children[i]->GetMomentum();
  return mom.perp2();
}

double iGeant4::FaserGeant4TruthIncident::childEkin(unsigned short i) const {
  return (m_children[i]->GetKineticEnergy()/CLHEP::MeV);
}

int iGeant4::FaserGeant4TruthIncident::childPdgCode(unsigned short i) const {
  return m_children[i]->GetDefinition()->GetPDGEncoding();
}

Barcode::ParticleBarcode  iGeant4::FaserGeant4TruthIncident::childBarcode(unsigned short index) const {
  // the G4Track instance for the current child particle
  const G4Track* track = m_children[index];
  // This should be a *secondary* track.  If it has a primary, it was a decay and
  //  we are running with quasi-stable particle simulation.  Note that if the primary
  //  track is passed in as a secondary that survived the interaction, then this was
  //  *not* a decay and we should not treat it in this way
  if (track->GetDynamicParticle() &&
      track->GetDynamicParticle()->GetPrimaryParticle() &&
      track->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation()){
    // Then the new particle should use the same barcode as the old one!!
    FaserPrimaryParticleInformation* ppi = dynamic_cast<FaserPrimaryParticleInformation*>( track->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation() );
    return ppi->GetParticleBarcode();
  }
  return 0;
}

void iGeant4::FaserGeant4TruthIncident::setAllChildrenBarcodes(Barcode::ParticleBarcode) {
  G4ExceptionDescription description;
  description << G4String("setAllChildrenBarcodes: ") + "Shared child particle barcodes are not implemented in ISF_Geant4 at this point.";
  G4Exception("iGeant4::Geant4TruthIncident", "NotImplemented", FatalException, description);
}

HepMC::GenParticlePtr iGeant4::FaserGeant4TruthIncident::childParticle(unsigned short i,
                                                                  Barcode::ParticleBarcode newBarcode) const {
  // the G4Track instance for the current child particle
  const G4Track* thisChildTrack = m_children[i];

  // NB: NOT checking if secondary is actually alive. Even with zero momentum,
  //     secondary could decay right away and create further particles which pass the
  //     truth strategies.

  HepMC::GenParticlePtr hepParticle = convert( thisChildTrack, newBarcode, true );
  FaserTrackHelper tHelper(thisChildTrack);
  FaserTrackInformation *trackInfo = tHelper.GetTrackInformation();

  // needed to make AtlasG4 work with ISF TruthService
  if (trackInfo==nullptr) {
    trackInfo = new FaserTrackInformation( hepParticle );
    thisChildTrack->SetUserInformation( trackInfo );
  }

  trackInfo->SetParticle(hepParticle);
  trackInfo->SetClassification(RegisteredSecondary);
  trackInfo->SetRegenerationNr(0);

  return hepParticle;
}


HepMC::GenParticlePtr iGeant4::FaserGeant4TruthIncident::updateChildParticle(unsigned short index,
                                                                        HepMC::GenParticlePtr existingChild) const {
  // the G4Track instance for the current child particle
  const G4Track* thisChildTrack = m_children[index];

  // NB: NOT checking if secondary is actually alive. Even with zero momentum,
  //     secondary could decay right away and create further particles which pass the
  //     truth strategies.

  const G4ThreeVector & mom =  thisChildTrack->GetMomentum();
  const double energy =  thisChildTrack->GetTotalEnergy();
  const int pdgCode = thisChildTrack->GetDefinition()->GetPDGEncoding();
  const HepMC::FourVector fourMomentum( mom.x(), mom.y(), mom.z(), energy);

  // This is the case for quasi-stable particle simulation
  if(existingChild->pdg_id()!=pdgCode) {
    G4ExceptionDescription description;
    description << G4String("updateChildParticle: ") + "Wrong PDG ID mismatch between G4Track and GenParticle";
    G4Exception("iGeant4::Geant4TruthIncident", "PDGIDMisMatch", FatalException, description);
  }
  existingChild->set_momentum(fourMomentum);

  FaserTrackHelper tHelper(thisChildTrack);
  FaserTrackInformation *trackInfo = tHelper.GetTrackInformation();

  // needed to make AtlasG4 work with ISF TruthService
  if(trackInfo==nullptr) {
    trackInfo = new FaserTrackInformation( existingChild );
    thisChildTrack->SetUserInformation( trackInfo );
  }

  trackInfo->SetParticle(existingChild);
  trackInfo->SetClassification(RegisteredSecondary);
  trackInfo->SetRegenerationNr(0);

  //FIXME!!
  return existingChild;
}


bool iGeant4::FaserGeant4TruthIncident::particleAlive(const G4Track *track) const {
  G4TrackStatus  trackStatus = track->GetTrackStatus();

  if ( trackStatus!=fAlive ) {
    // parent does not exist in G4 anymore after this step

    // check whether the particle was returned to ISF
    auto*    trackInfo = FaserISFG4Helper::getISFTrackInfo( *track );
    bool returnedToISF = trackInfo ? trackInfo->GetReturnedToISF() : false;
    if ( !returnedToISF ) {
      // particle was not sent to ISF either
      return false;
    }
  }

  return true;
}

#ifdef HEPMC3
HepMC::GenParticlePtr iGeant4::FaserGeant4TruthIncident::convert(const G4Track *track, const int, const bool secondary) const {
#else
HepMC::GenParticlePtr iGeant4::FaserGeant4TruthIncident::convert(const G4Track *track, const int barcode, const bool secondary) const {
#endif

  const G4ThreeVector & mom =  track->GetMomentum();
  const double energy =  track->GetTotalEnergy();
  const int pdgCode = track->GetDefinition()->GetPDGEncoding();
  const HepMC::FourVector fourMomentum( mom.x(), mom.y(), mom.z(), energy);

  const HepMC::GenParticlePtr parent = m_faserG4EvtUserInfo->GetCurrentlyTraced();
  int status = (secondary) ? 1 + HepMC::SIM_STATUS_THRESHOLD : parent->status() + HepMC::SIM_STATUS_INCREMENT;
  // Treat child particles of pre-defined decays differently
  if (this->interactionClassification() == ISF::QS_PREDEF_VTX) {
    const G4DynamicParticle* dynPart = track->GetDynamicParticle();
    bool hasPredefinedDecay = (dynPart && (nullptr!=(dynPart->GetPreAssignedDecayProducts())));
    status = (hasPredefinedDecay)? 2 : 1;
  }
  HepMC::GenParticlePtr newParticle = HepMC::newGenParticlePtr(fourMomentum, pdgCode, status);

#ifndef HEPMC3
  // This should be a *secondary* track.  If it has a primary, it was a decay and
  //  we are running with quasi-stable particle simulation.  Note that if the primary
  //  track is passed in as a secondary that survived the interaction, then this was
  //  *not* a decay and we should not treat it in this way
  if (secondary &&
      track->GetDynamicParticle() &&
      track->GetDynamicParticle()->GetPrimaryParticle() &&
      track->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation()){
    // Then the new particle should use the same barcode as the old one!!
    PrimaryParticleInformation* ppi = dynamic_cast<PrimaryParticleInformation*>( track->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation() );
    HepMC::suggest_barcode( newParticle, ppi->GetParticleBarcode() );
  } else {
    HepMC::suggest_barcode( newParticle, barcode );
  }
#endif

  return newParticle;
}


void iGeant4::FaserGeant4TruthIncident::prepareChildren() {

  const std::vector<const G4Track*>* tracks = m_step->GetSecondaryInCurrentStep();
  const int iSize = tracks->size();
  const int iLast = iSize - numberOfChildren() - 1; //NB can be -1.
  for(int i=iSize-1;i>iLast;i--) {
    m_children.push_back((*tracks)[i]);
  }
}

/**  The interaction classifications are described as follows:
     STD_VTX: interaction of a particle without a pre-defined decay;
     QS_SURV_VTX: a particle with a pre-defined decay under-going a
     non-destructive interaction;
     QS_DEST_VTX: a particle with a pre-defined decay under-going a
     destructive interaction other than its pre-defined decay;
     QS_PREDEF_VTX: a particle under-going its pre-defined decay */
ISF::InteractionClass_t iGeant4::FaserGeant4TruthIncident::interactionClassification() const {
  G4Track* track=m_step->GetTrack();
  const G4DynamicParticle* dynPart = track->GetDynamicParticle();
  bool parentIsQuasiStable = (nullptr!=(dynPart->GetPreAssignedDecayProducts()));
  const G4VProcess *process = m_step->GetPostStepPoint()->GetProcessDefinedStep();
  const int processType = process->GetProcessType();
  const int processSubType = process->GetProcessSubType();
  ISF::InteractionClass_t classification(ISF::STD_VTX);
  if(parentIsQuasiStable) {
    if(this->parentSurvivesIncident()) {
      classification = ISF::QS_SURV_VTX;
    }
    else if(processType==6 && processSubType==201) {
      classification = ISF::QS_PREDEF_VTX;
    }
    else {
      classification = ISF::QS_DEST_VTX;
    }
  }
  return classification;
  // end of code to determine interaction type
}

