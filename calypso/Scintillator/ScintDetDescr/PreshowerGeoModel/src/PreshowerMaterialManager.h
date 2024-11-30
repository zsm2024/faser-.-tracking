/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRESHOWERMATERIALMANAGER_H
#define PRESHOWERMATERIALMANAGER_H

// PreshowerMaterialManager. This provides an interface to the ScintMaterialManager which in turn
// is an interface to GeoModel Material Manager with some additional functionality.
#include "ScintGeoModelUtils/ScintMaterialManager.h"

#include <memory>
#include <string>

class GeoMaterial;
class GeoElement;
class ScintMaterialManager;
class PreshowerDataBase;

class PreshowerMaterialManager
{

public:

  PreshowerMaterialManager(PreshowerDataBase* db);

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

  std::unique_ptr<ScintMaterialManager> m_materialManager;
  const GeoMaterial* m_gasMaterial;

};


#endif // PRESHOWERMATERIALMANAGER_H
