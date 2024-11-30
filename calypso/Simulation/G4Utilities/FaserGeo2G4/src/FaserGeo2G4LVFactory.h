/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEO2G4_FaserGeo2G4LVFactory_h
#define FASERGEO2G4_FaserGeo2G4LVFactory_h

#include "GeoModelKernel/GeoVPhysVol.h"

class G4LogicalVolume;
class GeoLogVol;

class FaserGeo2G4LVFactory 
{
 public:
  FaserGeo2G4LVFactory();
  G4LogicalVolume* Build(const PVConstLink,
			 bool&) const;
};

#endif
