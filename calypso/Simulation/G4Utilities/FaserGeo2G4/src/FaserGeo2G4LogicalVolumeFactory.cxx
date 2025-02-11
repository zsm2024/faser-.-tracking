/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserGeo2G4LogicalVolumeFactory.h"
#include "FaserGeo2G4SolidFactory.h"
#include "FaserGeoMaterial2G4/FaserGeo2G4MaterialFactory.h"

#include "GeoModelKernel/GeoLogVol.h"

#include "G4LogicalVolume.hh"
#include "G4Material.hh"

#include "SimHelpers/ServiceAccessor.h"

FaserGeo2G4LogicalVolumeFactory::FaserGeo2G4LogicalVolumeFactory()
{
}

G4LogicalVolume *FaserGeo2G4LogicalVolumeFactory::Build(const GeoLogVol* theLog) const
{
  static FaserGeo2G4SolidFactory theSolidFactory;
  static FaserGeo2G4MaterialFactory theMaterialFactory;
  //
  // Get Material from GeoModel
  //
  std::string n= theLog->getName();
  G4LogicalVolume *theG4Log=0;
  G4Material* theG4Mat=theMaterialFactory.Build(theLog->getMaterial());

  G4VSolid * theG4Solid = theSolidFactory.Build(theLog->getShape(),theLog->getName());

  theG4Log = new G4LogicalVolume(theG4Solid,
                                 theG4Mat,
                                 theLog->getName(),
                                 0,0,0); ;

  return theG4Log;
}
