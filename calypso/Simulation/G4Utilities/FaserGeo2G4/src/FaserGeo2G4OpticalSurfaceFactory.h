/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEO2G4_FaserGeo2G4OpticalSurfaceFactory_h
#define FASERGEO2G4_FaserGeo2G4OpticalSurfaceFactory_h

#include <map>

class GeoOpticalSurface;
class G4OpticalSurface;

typedef std::map<const GeoOpticalSurface* , G4OpticalSurface*, std::less<const GeoOpticalSurface*> > Geo2G4OptSurfaceMap;

class FaserGeo2G4OpticalSurfaceFactory
{
 public:
  FaserGeo2G4OpticalSurfaceFactory();

  G4OpticalSurface* Build(const GeoOpticalSurface*);
 private:
  Geo2G4OptSurfaceMap m_definedOptSurfaces;
};

#endif
