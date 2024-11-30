/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserExtParameterisedVolumeBuilder.h"
#include "FaserGeo2G4AssemblyFactory.h"

typedef std::unordered_map<std::string, std::unique_ptr<FaserVolumeBuilder>> BuilderMap;

BuilderMap InitializeBuilders(FaserGeo2G4AssemblyFactory* assemblyFactory)
{
  BuilderMap map;
  map.emplace("Extended_Parameterised_Volume_Builder",
              std::make_unique<FaserExtParameterisedVolumeBuilder>("Extended_Parameterised_Volume_Builder", assemblyFactory));
  return map;
}
