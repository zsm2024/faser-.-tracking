/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4EVENT_FASERISFG4GEOHELPER_H
#define ISF_GEANT4EVENT_FASERISFG4GEOHELPER_H


#include "FaserDetDescr/FaserRegion.h"
#include "G4Step.hh"

namespace iGeant4 {

class FaserISFG4GeoHelper 
{

 public:
  FaserISFG4GeoHelper() = delete;

  static FaserDetDescr::FaserRegion nextGeoId(const G4Step* aStep, int truthVolLevel);
  static bool checkVolumeDepth(G4LogicalVolume* logicalVol, int volLevel, int depth=0);

  private:
  static G4LogicalVolume *s_vetoLV, *s_triggerLV, *s_preshowerLV, *s_vetonuLV, *s_sctLV, *s_dipoleLV, *s_ecalLV, *s_emulsionLV, *s_trenchLV;

};

}
#endif
