/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEO2G4_FaserGeo2G4AssemblyFactory_h
#define FASERGEO2G4_FaserGeo2G4AssemblyFactory_h

#include "GeoModelKernel/GeoVPhysVol.h"

#include <map>
#include <memory>

class FaserGeo2G4AssemblyVolume;

class FaserGeo2G4AssemblyFactory 
{
 public:
  FaserGeo2G4AssemblyFactory();
  FaserGeo2G4AssemblyVolume* Build(const PVConstLink thePhys,
			                  bool& descend);
 
 private:
  std::map<const GeoVPhysVol*, std::unique_ptr<FaserGeo2G4AssemblyVolume>, std::less<const GeoVPhysVol*> > m_assemblyMap;
};

#endif
