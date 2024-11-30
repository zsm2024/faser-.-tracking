/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEO2G4_FaserSingleLogicalVolumeFactory_h
#define FASERGEO2G4_FaserSingleLogicalVolumeFactory_h

#include "AthenaBaseComps/AthMessaging.h"

class G4LogicalVolume;
class GeoLogVol;

#include <string>
#include <map>

class FaserSingleLogicalVolumeFactory : public AthMessaging {
public:
  FaserSingleLogicalVolumeFactory();
  G4LogicalVolume* Build ATLAS_NOT_THREAD_SAFE (const GeoLogVol*);
 private:
};

#endif
