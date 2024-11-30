/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FaserTrackHelper_H
#define FaserTrackHelper_H

#include "G4Track.hh"
#include "FaserMCTruth/FaserTrackInformation.h"
#include "GeneratorObjects/HepMcParticleLink.h"

class FaserTrackHelper {
public:
  FaserTrackHelper(const G4Track* t);
  bool IsPrimary() const ;
  bool IsRegeneratedPrimary() const;
  bool IsRegisteredSecondary() const ;
  bool IsSecondary() const ;
  int GetBarcode() const ;
  FaserTrackInformation * GetTrackInformation() {return m_trackInfo;}
  HepMcParticleLink GetParticleLink();
private:
  FaserTrackInformation *m_trackInfo;
};

#endif
