/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserMCTruth/FaserPrimaryParticleInformation.h"

FaserPrimaryParticleInformation::FaserPrimaryParticleInformation()
{
}

FaserPrimaryParticleInformation::FaserPrimaryParticleInformation(HepMC::GenParticlePtr p, ISF::FaserISFParticle* isp):m_theParticle(p),m_theISFParticle(isp)
{
}

int FaserPrimaryParticleInformation::GetParticleBarcode() const
{
  if (m_barcode !=  HepMC::INVALID_PARTICLE_BARCODE) return m_barcode;
  if (m_theParticle) {
      m_barcode = HepMC::barcode(m_theParticle);
      return m_barcode;
  }
  return 0;
}

int FaserPrimaryParticleInformation::GetParticleUniqueID() const
{
  if (m_uniqueID !=  HepMC::INVALID_PARTICLE_BARCODE) return m_uniqueID;
  if (m_theParticle) {
    HepMC::ConstGenParticlePtr particle = m_theParticle;
    m_uniqueID = HepMC::uniqueID(particle);
    return m_uniqueID;
  }
  return 0;
}

void FaserPrimaryParticleInformation::SetParticle(HepMC::GenParticlePtr p)
{
  m_theParticle=p;
  m_barcode = HepMC::INVALID_PARTICLE_BARCODE;
  m_uniqueID = HepMC::INVALID_PARTICLE_BARCODE;
}

void FaserPrimaryParticleInformation::SetISFParticle(ISF::FaserISFParticle* p)
{
  m_theISFParticle=p;
}

HepMC::ConstGenParticlePtr FaserPrimaryParticleInformation::GetHepMCParticle() const
{
  return m_theParticle;
}