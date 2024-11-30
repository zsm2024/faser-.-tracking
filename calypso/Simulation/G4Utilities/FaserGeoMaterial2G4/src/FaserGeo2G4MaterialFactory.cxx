/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/checker_macros.h"
#include "FaserGeoMaterial2G4/FaserGeo2G4MaterialFactory.h"
#include "FaserGeo2G4ElementFactory.h"
#include "FaserGeoMaterial2G4/FaserGeo2G4MatPropTableFactory.h"

#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelFaserUtilities/GeoExtendedMaterial.h"
#include "GeoModelFaserUtilities/GeoMaterialPropertiesTable.h"

#include "G4Material.hh"

#include <mutex>
#include <unordered_map>

namespace {
  typedef std::unordered_map<const GeoMaterial* , G4Material*> matList;
  typedef std::unordered_map<std::string, const GeoMaterial*> matNames;
}

FaserGeo2G4MaterialFactory::FaserGeo2G4MaterialFactory() :
  AthMessaging("FaserGeo2G4MaterialFactory")
{
}

G4Material* FaserGeo2G4MaterialFactory::Build(const GeoMaterial* geoMaterial)
{
  // Material caches and mutex
  static matList geoMaterialToG4Material ATLAS_THREAD_SAFE;
  static matNames geoMaterialNameToObject ATLAS_THREAD_SAFE;

  // For now just use a global lock. If this turns out to be a bottleneck
  // switch to a concurrent map or similar.
  static std::mutex matLock;
  std::scoped_lock lock(matLock);

  //
  // Check if this material has already been defined.
  //
  const auto itr = geoMaterialToG4Material.find(geoMaterial);
  if(itr != geoMaterialToG4Material.end()) {
    return itr->second;
  }

  const std::string& geoMaterialName = geoMaterial->getName();
  const int nelements = geoMaterial->getNumElements();

  // Different actions depending whether we are dealing with
  // standard or extended materials

  const GeoExtendedMaterial* extMat = dynamic_cast<const GeoExtendedMaterial*>(geoMaterial);
  G4Material* g4Material{};

  if(extMat) {
    G4State state = kStateUndefined;

    switch(extMat->getState())
      {
      case stateUndefined:
        state = kStateUndefined;
        break;
      case stateSolid:
        state = kStateSolid;
        break;
      case stateLiquid:
        state = kStateLiquid;
        break;
      case stateGas:
        state = kStateGas;
        break;
      default:
        break;
      }

    const double temperature = extMat->getTemperature();
    const double pressure = extMat->getPressure();

    g4Material= new G4Material(geoMaterialName,
                               extMat->getDensity(),
                               nelements,
                               state,
                               temperature,
                               pressure);

    // Build G4MaterialPropertiesTable if needed
    const GeoMaterialPropertiesTable* geoPropTable = extMat->GetMaterialPropertiesTable();

    if(geoPropTable) {
      FaserGeo2G4MatPropTableFactory tFactory;
      G4MaterialPropertiesTable* g4PropTable = tFactory.Build(geoPropTable);
      if(g4PropTable) {
        g4Material->SetMaterialPropertiesTable(g4PropTable);
      }
    }
  }
  else {
    g4Material= new G4Material(geoMaterialName,
                               geoMaterial->getDensity(),
                               nelements);
  }

  static FaserGeo2G4ElementFactory eFactory ATLAS_THREAD_SAFE;  // locked above
  for (int ii = 0; ii< nelements; ii++)  {
    G4Element* g4Element = eFactory.Build(geoMaterial->getElement(ii));
    g4Material->AddElement(g4Element, geoMaterial->getFraction(ii));
  }

  const G4MaterialTable & theMaterialTable = *(g4Material->GetMaterialTable());
  const G4String& g4MaterialName = g4Material->GetName();
  std::vector<size_t> copyIndex{};
  for(size_t i=0; i< theMaterialTable.size(); ++i) {
    if(theMaterialTable[i]->GetName() == g4MaterialName) {
      copyIndex.push_back(i);
    }
  }
  if ( copyIndex.size() > 1 ) {
    ATH_MSG_WARNING ( "Details of all G4Materials named " << g4MaterialName << " in the G4MaterialTable.");
    for (size_t index : copyIndex) {
      ATH_MSG_WARNING ( "G4Material at position "<< index<<" in the G4MaterialTable: \n" << *(theMaterialTable[index]));
    }
  }


  geoMaterialToG4Material[geoMaterial]=g4Material;

  // Check if we have the situation when on GeoModel side two different
  // materials share the same name.
  // Print a WARNING message if so.
  const auto [it, inserted] = geoMaterialNameToObject.try_emplace(geoMaterialName, geoMaterial);
  if(!inserted) {
    ATH_MSG_WARNING ( "!!! On GeoModel side two different materials share the name: " << geoMaterialName );
  }

  return g4Material;
}

