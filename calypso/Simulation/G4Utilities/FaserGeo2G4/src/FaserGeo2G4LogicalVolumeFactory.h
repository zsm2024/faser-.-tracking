/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEO2G4_FaserGeo2G4LogicalVolumeFactory_h
#define FASERGEO2G4_FaserGeo2G4LogicalVolumeFactory_h

class G4LogicalVolume;
class GeoLogVol;

class FaserGeo2G4LogicalVolumeFactory {
public:
  FaserGeo2G4LogicalVolumeFactory();
  G4LogicalVolume* Build(const GeoLogVol*) const;
};

#endif
