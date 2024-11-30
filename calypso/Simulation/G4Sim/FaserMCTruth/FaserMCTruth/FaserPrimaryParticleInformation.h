/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FaserPrimaryParticleInformation_H
#define FaserPrimaryParticleInformation_H

#include "G4VUserPrimaryParticleInformation.hh"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "TruthUtils/MagicNumbers.h"
#include "CxxUtils/checker_macros.h"

namespace ISF {
  class FaserISFParticle;
}

class FaserPrimaryParticleInformation: public G4VUserPrimaryParticleInformation {
public:
	FaserPrimaryParticleInformation();
	FaserPrimaryParticleInformation(HepMC::GenParticlePtr, ISF::FaserISFParticle* isp=0);
	HepMC::ConstGenParticlePtr GetHepMCParticle() const;
	HepMC::GenParticlePtr GetHepMCParticle() { return m_theParticle; }
	int GetParticleBarcode() const;
	int GetParticleUniqueID() const;
	void SuggestBarcode(int bc);
	void SetParticle(HepMC::GenParticlePtr);
	void Print() const {}
	int GetRegenerationNr() {return  m_regenerationNr;}
	void SetRegenerationNr(int i) {m_regenerationNr=i;}

	void SetISFParticle(ISF::FaserISFParticle* isp);
	const ISF::FaserISFParticle* GetISFParticle() const;
	ISF::FaserISFParticle* GetISFParticle() { return m_theISFParticle; }

private:
	HepMC::GenParticlePtr m_theParticle;
	ISF::FaserISFParticle* m_theISFParticle;

    int m_regenerationNr{0};
    mutable int m_barcode ATLAS_THREAD_SAFE = HepMC::INVALID_PARTICLE_BARCODE;
    mutable int m_uniqueID ATLAS_THREAD_SAFE = HepMC::INVALID_PARTICLE_BARCODE;

};

#endif
