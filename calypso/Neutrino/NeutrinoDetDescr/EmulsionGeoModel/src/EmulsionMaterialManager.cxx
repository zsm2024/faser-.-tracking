/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EmulsionMaterialManager.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"
#include "EmulsionDataBase.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <iostream>

// Constructor 
EmulsionMaterialManager::EmulsionMaterialManager(EmulsionDataBase* db)
{
  // Get my material manager.
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StoreGateSvc* detStore;
  StatusCode sc = svcLocator->service("DetectorStore", detStore );
  if (sc.isFailure()) {
    std::cout << "Could not locate DetectorStore" << std::endl;
    return;
  }

  m_materialManager = std::make_unique<NeutrinoMaterialManager>("EmulsionMaterialManager", db->athenaComps());
  m_materialManager->addWeightTable(db->weightTable(), "emulsion");
  m_materialManager->addScalingTable(db->scalingTable());

  loadMaterials();

  m_gasMaterial = m_materialManager->getMaterial("std::Air");
}

// Add materials not yet in the database 
void
EmulsionMaterialManager::loadMaterials()
{
}

const GeoElement* 
EmulsionMaterialManager::getElement(const std::string & elementName) const
{
  return m_materialManager->getElement(elementName);
}

const GeoMaterial* 
EmulsionMaterialManager::getMaterial(const std::string & materialName) const
{
  return m_materialManager->getMaterial(materialName);
}

void
EmulsionMaterialManager::addMaterial(GeoMaterial* material)
{
  return m_materialManager->addMaterial(material);
}

const GeoMaterial* 
EmulsionMaterialManager::getMaterial(const std::string & originalMaterial, 
                                 double density,  
                                 const std::string & newName)
{
  
  return m_materialManager->getMaterial(originalMaterial, density, newName);
}

const GeoMaterial *
EmulsionMaterialManager::getMaterialForVolume(const std::string & materialName, double volume)
{
  return m_materialManager->getMaterialForVolume(materialName, volume);
}



const GeoMaterial* 
EmulsionMaterialManager::gasMaterial() const
{
  return m_gasMaterial;
}
