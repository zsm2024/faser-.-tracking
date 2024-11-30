/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FaserTrackInformation_H
#define FaserTrackInformation_H

#include "FaserVTrackInformation.h"
#include "TruthUtils/MagicNumbers.h"
#include "CxxUtils/checker_macros.h"

namespace ISF {
  class FaserISFParticle;
}

class FaserTrackInformation: public FaserVTrackInformation {
public:
	FaserTrackInformation();
	FaserTrackInformation(HepMC::GenParticlePtr, ISF::FaserISFParticle* baseIsp=0);
	virtual HepMC::ConstGenParticlePtr GetHepMCParticle() const override { return m_theParticle; };
	virtual HepMC::GenParticlePtr GetHepMCParticle() override { return m_theParticle; };
	virtual const ISF::FaserISFParticle *GetBaseISFParticle() const override { return m_theBaseISFParticle; };
	virtual ISF::FaserISFParticle *GetBaseISFParticle() override { return m_theBaseISFParticle; };
	virtual int GetParticleBarcode() const override;
  	virtual int GetParticleUniqueID() const override;
  	virtual int GetParticleStatus() const override;
   	virtual void SetParticle(HepMC::GenParticlePtr) override;
	virtual void SetBaseISFParticle(ISF::FaserISFParticle*) override;
	virtual void SetReturnedToISF(bool returned) override {m_returnedToISF=returned;};
	virtual bool GetReturnedToISF() const override {return m_returnedToISF;};
	void SetRegenerationNr(int i) {m_regenerationNr=i;};
	int GetRegenerationNr() const {return m_regenerationNr;};
private:
	int m_regenerationNr;
	HepMC::GenParticlePtr m_theParticle;
    mutable int m_barcode ATLAS_THREAD_SAFE = HepMC::INVALID_PARTICLE_BARCODE; // TODO Drop this once UniqueID and Status are used instead
    mutable int m_uniqueID ATLAS_THREAD_SAFE = HepMC::INVALID_PARTICLE_BARCODE;
	ISF::FaserISFParticle *m_theBaseISFParticle;
	bool m_returnedToISF;
};

#endif
