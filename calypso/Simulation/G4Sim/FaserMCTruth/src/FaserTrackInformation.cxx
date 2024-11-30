/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserMCTruth/FaserTrackInformation.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"

FaserTrackInformation::FaserTrackInformation()
  : m_regenerationNr(0)
  , m_theParticle(nullptr)
  , m_theBaseISFParticle(nullptr)
  , m_returnedToISF(false)
{
}

FaserTrackInformation::FaserTrackInformation(HepMC::GenParticlePtr p, ISF::FaserISFParticle* baseIsp)
  : m_regenerationNr(0)
  , m_theParticle(p)
  , m_theBaseISFParticle(baseIsp)
  , m_returnedToISF(false)
{
}

int FaserTrackInformation::GetParticleBarcode() const
{
  if (m_barcode != HepMC::INVALID_PARTICLE_BARCODE) return m_barcode;
  if (m_theParticle) {
    m_barcode = HepMC::barcode(m_theParticle);
    return m_barcode;
  }
  return 0;
}

int FaserTrackInformation::GetParticleUniqueID() const
{
  if (m_uniqueID != HepMC::INVALID_PARTICLE_BARCODE) return m_uniqueID;
  if (m_theParticle) {
    HepMC::ConstGenParticlePtr particle = m_theParticle;
    m_uniqueID = HepMC::uniqueID(particle);
    return m_uniqueID;
  }
  return 0;
}

int FaserTrackInformation::GetParticleStatus() const
{
  if (m_theParticle) {
    return m_theParticle->status();
  }
  return 0;
}

void FaserTrackInformation::SetParticle(HepMC::GenParticlePtr p)
{
  m_theParticle=p;
  m_barcode = HepMC::INVALID_PARTICLE_BARCODE;
  m_uniqueID = HepMC::INVALID_PARTICLE_BARCODE;
}

void FaserTrackInformation::SetBaseISFParticle(ISF::FaserISFParticle* p)
{
  m_theBaseISFParticle=p;
}

