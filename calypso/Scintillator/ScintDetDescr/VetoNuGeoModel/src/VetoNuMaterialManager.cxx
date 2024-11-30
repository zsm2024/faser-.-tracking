/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoNuMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"
#include "VetoNuDataBase.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <iostream>

// Constructor 
VetoNuMaterialManager::VetoNuMaterialManager(VetoNuDataBase* db)
{
  // Get my material manager.
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StoreGateSvc* detStore;
  StatusCode sc = svcLocator->service("DetectorStore", detStore );
  if (sc.isFailure()) {
    std::cout << "Could not locate DetectorStore" << std::endl;
    return;
  }

  m_materialManager = std::make_unique<ScintMaterialManager>("VetoNuMaterialManager", db->athenaComps());
  m_materialManager->addWeightTable(db->weightTable(), "veto");
  m_materialManager->addScalingTable(db->scalingTable());

  loadMaterials();

  m_gasMaterial = m_materialManager->getMaterial("std::Air");
}

// Add materials not yet in the database 
void
VetoNuMaterialManager::loadMaterials()
{
}

const GeoElement* 
VetoNuMaterialManager::getElement(const std::string & elementName) const
{
  return m_materialManager->getElement(elementName);
}

const GeoMaterial* 
VetoNuMaterialManager::getMaterial(const std::string & materialName) const
{
  return m_materialManager->getMaterial(materialName);
}

void
VetoNuMaterialManager::addMaterial(GeoMaterial* material)
{
  return m_materialManager->addMaterial(material);
}

const GeoMaterial* 
VetoNuMaterialManager::getMaterial(const std::string & originalMaterial, 
                                 double density,  
                                 const std::string & newName)
{
  
  return m_materialManager->getMaterial(originalMaterial, density, newName);
}

const GeoMaterial *
VetoNuMaterialManager::getMaterialForVolume(const std::string & materialName, double volume)
{
  return m_materialManager->getMaterialForVolume(materialName, volume);
}



const GeoMaterial* 
VetoNuMaterialManager::gasMaterial() const
{
  return m_gasMaterial;
}
