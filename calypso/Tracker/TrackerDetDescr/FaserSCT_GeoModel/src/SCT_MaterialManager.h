/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_MATERIALMANAGER_H
#define FASERSCT_MATERIALMANAGER_H

// SCT_MaterialManager. This provides an interface to the TrackerMaterialManager which in turn
// is an interface to GeoModel Material Manager with some additional functionality.
#include "TrackerGeoModelUtils/TrackerMaterialManager.h"

#include <memory>
#include <string>

class GeoMaterial;
class GeoElement;
class TrackerMaterialManager;
class SCT_DataBase;

class SCT_MaterialManager
{

public:

  SCT_MaterialManager(SCT_DataBase* db);

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

  std::unique_ptr<TrackerMaterialManager> m_materialManager;
  const GeoMaterial* m_gasMaterial;

};


#endif // FASERSCT_MATERIALMANAGER_H
