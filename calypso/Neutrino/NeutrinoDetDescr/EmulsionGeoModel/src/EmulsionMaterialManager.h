/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMULSIONMATERIALMANAGER_H
#define EMULSIONMATERIALMANAGER_H

// EmulsionMaterialManager. This provides an interface to the NeutrinoMaterialManager which in turn
// is an interface to GeoModel Material Manager with some additional functionality.
#include "NeutrinoGeoModelUtils/NeutrinoMaterialManager.h"

#include <memory>
#include <string>

class GeoMaterial;
class GeoElement;
class NeutrinoMaterialManager;
class EmulsionDataBase;

class EmulsionMaterialManager
{

public:

  EmulsionMaterialManager(EmulsionDataBase* db);

  const GeoMaterial* getMaterial(const std::string & materialName) const;
  const GeoElement* getElement(const std::string & elementName) const;

  const GeoMaterial* getMaterial(const std::string & originalMaterial, 
				 double density,  
				 const std::string & newName = "");
  const GeoMaterial *getMaterialForVolume(const std::string & materialName, double volume);

  // Default gas material
  const GeoMaterial* gasMaterial() const;

private:
  void loadMaterials();
  void addMaterial(GeoMaterial* material);

  std::unique_ptr<NeutrinoMaterialManager> m_materialManager;
  const GeoMaterial* m_gasMaterial;

};


#endif // EMULSIONMATERIALMANAGER_H
