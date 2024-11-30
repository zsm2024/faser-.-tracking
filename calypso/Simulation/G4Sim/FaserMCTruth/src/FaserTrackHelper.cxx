/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserMCTruth/FaserTrackHelper.h"
#include "FaserISF_Event/FaserISFParticle.h"

FaserTrackHelper::FaserTrackHelper(const G4Track* t)
{
  m_trackInfo=static_cast<FaserTrackInformation *>(t->GetUserInformation());
}
bool FaserTrackHelper::IsPrimary() const
{
  if (m_trackInfo==0) return false;
  return m_trackInfo->GetClassification()==Primary;
}
bool FaserTrackHelper::IsRegeneratedPrimary() const
{
  if (m_trackInfo==0) return false;
  return m_trackInfo->GetClassification()==RegeneratedPrimary;
}
bool FaserTrackHelper::IsRegisteredSecondary() const
{
  if (m_trackInfo==0) return false;
  return m_trackInfo->GetClassification()==RegisteredSecondary;
}
bool FaserTrackHelper::IsSecondary() const
{
  if (m_trackInfo==0) return true;
  return m_trackInfo->GetClassification()==Secondary;
}
int FaserTrackHelper::GetBarcode() const
{
  if (m_trackInfo==0 || m_trackInfo->GetHepMCParticle()==0) return 0;
  return m_trackInfo->GetParticleBarcode();
}

HepMcParticleLink FaserTrackHelper::GetParticleLink()
{
  int barcode = this->GetBarcode();
  return HepMcParticleLink(barcode);
  //HepMcParticleLink * originalParticleLink(nullptr);
  //if (this->GetTrackInformation() && this->GetTrackInformation()->GetBaseISFParticle())
  //  {
  //    originalParticleLink = this->GetTrackInformation()->GetBaseISFParticle()->getParticleLink();
  //  }
  //return (originalParticleLink) ? HepMcParticleLink(barcode, originalParticleLink->eventIndex(), originalParticleLink->getEventCollection()) : HepMcParticleLink(barcode);
}
