/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEOMATERIAL2G4_FaserGeo2G4MaterialFactory_h
#define FASERGEOMATERIAL2G4_Fasereo2G4MaterialFactory_h

#include "G4Material.hh"
#include "GeoModelKernel/GeoMaterial.h"
#include "AthenaBaseComps/AthMessaging.h"

#include <map>
#include <string>

class FaserGeo2G4MaterialFactory : public AthMessaging {
public:
  FaserGeo2G4MaterialFactory();
  G4Material* Build(const GeoMaterial*);
};

#endif

