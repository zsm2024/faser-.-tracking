/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserMCTruth/FaserVTrackInformation.h"

FaserVTrackInformation::FaserVTrackInformation(TrackClassification tc):m_classify(tc)
{
}

void FaserVTrackInformation::SetPrimaryHepMCParticle(HepMC::GenParticlePtr p)
{
  m_thePrimaryParticle=p;
}

bool FaserVTrackInformation::GetReturnedToISF() const
{
  return false;
}

void FaserVTrackInformation::SetParticle(HepMC::GenParticlePtr /*p*/)
{
  // you should not call this, perhaps throw an exception?
  std::cerr<<"ERROR  VTrackInformation::SetParticle() not supported  "<<std::endl;
 
}

void FaserVTrackInformation::SetBaseISFParticle(ISF::FaserISFParticle* /*p*/)
{
  // you should not call this, perhaps throw an exception?
  std::cerr<<"ERROR  FaserVTrackInformation::SetBaseISFParticle() not supported  "<<std::endl;
 
}

void FaserVTrackInformation::SetReturnedToISF(bool)
{
  std::cerr<<"ERROR  FaserVTrackInformation::SetReturnedToISF() not supported  "<<std::endl;
}

