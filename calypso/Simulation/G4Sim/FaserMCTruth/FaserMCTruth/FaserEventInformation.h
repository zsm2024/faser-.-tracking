/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FaserEventInformation_H
#define FaserEventInformation_H

#include "AtlasHepMC/GenEvent.h"
#include "G4ThreeVector.hh"
#include "G4VUserEventInformation.hh"

class FaserEventInformation: public G4VUserEventInformation {
public:
        FaserEventInformation(): G4VUserEventInformation(),m_nrOfPrimaryParticles(0),
                        m_nrOfPrimaryVertices(0),m_secondaryParticleBarCode(200000),
                        m_secondaryVertexBarCode(-200000),m_theEvent(0),
                        m_currentPrimary(0),m_currentlyTraced(0),
                        m_last_processed_barcode(0),m_last_processed_step(0) {}
        HepMC::GenEvent* GetHepMCEvent() ;
        void SetHepMCEvent(HepMC::GenEvent*);
        int GetNrOfPrimaryParticles() const;
        void SetNrOfPrimaryParticles(int nr);
        int GetNrOfPrimaryVertices() const;
        void SetNrOfPrimaryVertices(int nr);
        void SetVertexPosition(const G4ThreeVector);
        const G4ThreeVector GetVertexPosition() const;
        void Print() const {}

        void SetCurrentPrimary(HepMC::GenParticle *p) {m_currentPrimary=p;}

        void SetCurrentlyTraced(HepMC::GenParticle *p) {m_currentlyTraced=p;}

        HepMC::GenParticle *GetCurrentPrimary() const {return m_currentPrimary;}

        HepMC::GenParticle *GetCurrentlyTraced() const {return m_currentlyTraced;}
        int SecondaryParticleBarCode() {m_secondaryParticleBarCode++;
                                        return m_secondaryParticleBarCode;}
        int SecondaryVertexBarCode() {m_secondaryVertexBarCode--;
                                      return m_secondaryVertexBarCode;}
        int GetLastProcessedBarcode() const { return m_last_processed_barcode; }
        void SetLastProcessedBarcode(int b) { m_last_processed_barcode = b; }
        int GetLastProcessedStep() const { return m_last_processed_step; }
        void SetLastProcessedStep(int s) { m_last_processed_step = s; }

private:
        G4ThreeVector m_vertexPosition;
        int m_nrOfPrimaryParticles;
        int m_nrOfPrimaryVertices;
        int m_secondaryParticleBarCode;
        int m_secondaryVertexBarCode;
        HepMC::GenEvent *m_theEvent;
        HepMC::GenParticle *m_currentPrimary;
        HepMC::GenParticle *m_currentlyTraced;
        // These two are used by calibration hits as event-level flags
        // They correspond to the last barcode and step processed by an SD
        // Both are needed, because a particle might have only one step
        int m_last_processed_barcode;
        int m_last_processed_step;
};

#endif
