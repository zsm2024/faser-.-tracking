/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EcalModule.h"

#include "EcalGeometryManager.h"
// #include "EcalMaterialManager.h"

// #include "EcalModuleParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "GeoModelXMLParser/XercesParser.h"

#include "CaloReadoutGeometry/EcalDetectorManager.h"
#include "CaloReadoutGeometry/CaloDetectorDesign.h"
#include "CaloReadoutGeometry/CaloDetectorElement.h"
#include "CaloReadoutGeometry/CaloDD_Defs.h"
#include "CaloReadoutGeometry/CaloCommonItems.h"

#include "PathResolver/PathResolver.h"

#include "GaudiKernel/SystemOfUnits.h"

using namespace CaloDD;

EcalModule::EcalModule(const std::string & name,
                     CaloDD::EcalDetectorManager* detectorManager,
                     const EcalGeometryManager* geometryManager,
                     EcalMaterialManager* materials)
  : EcalUniqueComponentFactory(name, detectorManager, geometryManager, materials),
    m_width{0.0}, m_height{0.0}, m_length{0.0},
    m_noElementWarning{true}
{

  std::string resolvedFile = PathResolver::find_file(geometryManager->options().gdmlFile(), "XMLPATH", PathResolver::RecursiveSearch);
  GeoModelTools::XercesParser xercesParser;
  xercesParser.ParseFileAndNavigate(resolvedFile);
  std::cout << "done parsing " << resolvedFile << std::endl;

  getParameters();

  m_logVolume = preBuild();
  m_logVolume->ref();
}

EcalModule::~EcalModule()
{
  m_logVolume->unref();
}

void
EcalModule::getParameters()
{
  const GeoBox* lvBox = dynamic_cast<const GeoBox*>(m_controller.retrieveLogicalVolume("_dd_Geometry_DownstreamRegion_Ecal_Modules_Outer").first->getShape());
  if (lvBox != nullptr)
  {
    std::cout << "Ecal logical volume dimensions: ( " << 2 * lvBox->getXHalfLength() << " , " << 2 * lvBox->getYHalfLength() << " , " << 2 * lvBox->getZHalfLength() << " )" << std::endl;
    m_width = 2 * lvBox->getXHalfLength();
    m_height = 2 * lvBox->getYHalfLength();
    m_length = 2 * lvBox->getZHalfLength();
  }

  m_detectorManager->numerology().setNumPmtsPerModule(m_pmtsPerModule);  
}

const GeoLogVol * 
EcalModule::preBuild()
{

  // Build the module. 

  const GeoLogVol* moduleLog = m_controller.retrieveLogicalVolume("_dd_Geometry_DownstreamRegion_Ecal_Modules_Outer").first;
  
  // Make the calo design for this plate
  makeDesign();

  m_detectorManager->setDesign(m_design);
  
  return moduleLog;
}


void
EcalModule::makeDesign()
{ 
    m_design = new CaloDetectorDesign(m_width, 
                                      m_height, 
                                      m_length,
                                      m_pmtsPerModule);
}



GeoVPhysVol * 
EcalModule::build(EcalIdentifier id)
{
  GeoFullPhysVol * module = new GeoFullPhysVol(m_logVolume); 
  GeoPhysVol* modulePhys = m_controller.retrieveLogicalVolume("_dd_Geometry_DownstreamRegion_Ecal_Modules_Outer").second;
  module->add(modulePhys);

  // Make detector element and add to collection
  // Only do so if we have a valid id helper.

  //id.print(); // for debugging only

  const CaloCommonItems* commonItems =  m_geometryManager->commonItems();

  if (commonItems->getIdHelper()) {

    CaloDetectorElement * detElement;

    detElement =  new CaloDetectorElement(id.getModuleId(), 
                                           m_design, 
                                           module,  
                                           commonItems);
    
    // Add the detector element.
    m_detectorManager->addDetectorElement(detElement);

  } else {
    if (m_noElementWarning) {
      std::cout << "WARNING!!!!: No Ecal id helper and so no elements being produced." << std::endl;
      m_noElementWarning = false;
    }
  }
  return module;
}
