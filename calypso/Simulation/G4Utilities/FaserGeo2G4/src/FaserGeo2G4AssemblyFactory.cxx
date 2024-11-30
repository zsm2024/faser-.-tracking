/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserGeo2G4AssemblyFactory.h"
#include "FaserGeo2G4AssemblyVolume.h"

#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"

#include <iostream>

FaserGeo2G4AssemblyFactory::FaserGeo2G4AssemblyFactory()
{
}

FaserGeo2G4AssemblyVolume* FaserGeo2G4AssemblyFactory::Build(const PVConstLink thePhys,
                                                   bool& descend)
{
  const GeoLogVol* theLog = thePhys->getLogVol();

  // Check if the assembly was used in GeoModel as a leaf node
  if(thePhys->getNChildVols() == 0)
    throw std::runtime_error(std::string("In GeoModel description the assembly ") + theLog->getName()
                             + std::string(" has no children!"));

  descend = true;

  // Search for the assembly in the map
  auto it = m_assemblyMap.find(&(*thePhys));
  if(it == m_assemblyMap.end())
    {
      auto pair = m_assemblyMap.emplace(&(*thePhys), std::make_unique<FaserGeo2G4AssemblyVolume> ());
      return pair.first->second.get();
    }
  else
    {
      descend = false;
      return it->second.get();
    }
}
