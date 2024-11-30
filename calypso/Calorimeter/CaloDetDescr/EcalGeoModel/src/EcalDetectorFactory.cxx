/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// EcalDetectorFactory: This is the top level node 
//


#include "EcalDetectorFactory.h" 

#include "EcalDataBase.h"
#include "EcalIdentifier.h"
#include "EcalGeometryManager.h" 
#include "EcalMaterialManager.h"
#include "EcalGeneralParameters.h"
#include "CaloReadoutGeometry/Version.h" 
#include "CaloReadoutGeometry/CaloCommonItems.h" 
#include "CaloReadoutGeometry/CaloDD_Defs.h"
#include "CaloReadoutGeometry/CaloDetectorDesign.h" 

#include "EcalRow.h"
#include "EcalModule.h"
#include "EcalDataBase.h"
#include "EcalGeoModel/EcalGeoModelAthenaComps.h"

//
// GeoModel include files:
//
#include "GeoModelKernel/GeoMaterial.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoLogVol.h"  
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoIdentifierTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoVPhysVol.h"  
#include "GeoModelKernel/GeoTransform.h"  
#include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"

#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"



#include <iostream> 
#include <iomanip> 
#include <string>
 
using CaloDD::EcalDetectorManager; 
using CaloDD::CaloCommonItems; 

EcalDetectorFactory::EcalDetectorFactory(const EcalGeoModelAthenaComps * athenaComps,
					                     const EcalOptions & options)
  : CaloDD::DetectorFactoryBase(athenaComps),
    m_materials {nullptr},
    m_useDynamicAlignFolders(false),
    m_gdmlFile(options.gdmlFile())
{ 
  
  // Create the detector manager
  m_detectorManager = new EcalDetectorManager(detStore());
  msg(MSG::DEBUG) << "Created EcalDetectorManager" << endmsg;

  // Create the database
  m_db = new EcalDataBase{athenaComps};
  msg(MSG::DEBUG) << "Created EcalDataBase" << endmsg;

  // Create the material manager
  m_materials = new EcalMaterialManager{m_db};
  msg(MSG::DEBUG) << "Created EcalMaterialManager" << endmsg;

  // Create the geometry manager.
  m_geometryManager = new EcalGeometryManager{m_db};
  msg(MSG::DEBUG) << "Created EcalGeometryManager" << endmsg;
  m_geometryManager->setOptions(options);
  msg(MSG::DEBUG) << "Set options on EcalGeometryManager" << endmsg;

  m_useDynamicAlignFolders = options.dynamicAlignFolders();
 
  // Set Version information
  // Get the geometry tag
  DecodeFaserVersionKey versionKey(geoDbTagSvc(),"Ecal");
  IRDBRecordset_ptr switchSet
    = rdbAccessSvc()->getRecordsetPtr("EcalSwitches", versionKey.tag(), versionKey.node(),"FASERDD");
  const IRDBRecord    *switches   = (*switchSet)[0];
  msg(MSG::DEBUG) << "Retrieved EcalSwitches" << endmsg;

  std::string layout = "Final";
  std::string description;
  if (!switches->isFieldNull("LAYOUT")) {
    layout = switches->getString("LAYOUT");
  }
  if (!switches->isFieldNull("DESCRIPTION")) {
    description = switches->getString("DESCRIPTION");
  }

  std::string versionTag = rdbAccessSvc()->getChildTag("Ecal", versionKey.tag(), versionKey.node(),"FASERDD");
  std::string versionName = switches->getString("VERSIONNAME");
  int versionMajorNumber = 1;
  int versionMinorNumber = 0;
  int versionPatchNumber = 0;
  CaloDD::Version version(versionTag,
                           versionName, 
                           layout, 
                           description, 
                           versionMajorNumber,
                           versionMinorNumber,
                           versionPatchNumber);
  m_detectorManager->setVersion(version);
} 
 
 
EcalDetectorFactory::~EcalDetectorFactory() 
{ 
  // NB the detector manager (m_detectorManager)is stored in the detector store by the
  // Tool and so we don't delete it.
  delete m_db;
  delete m_materials;
  delete m_geometryManager;
} 

void EcalDetectorFactory::create(GeoPhysVol *world) 
{ 

  msg(MSG::INFO) << "Building Ecal Detector." << endmsg;
  msg(MSG::INFO) << " " << m_detectorManager->getVersion().fullDescription() << endmsg;
  msg(MSG::INFO) << " GDML file: " << m_gdmlFile << endmsg;

  // Change precision.
  int oldPrecision = std::cout.precision(6);

  // The tree tops get added to world. We name it "calo" though.
  GeoPhysVol *calo = world;

  const EcalGeneralParameters * ecalGeneral = m_geometryManager->generalParameters();

  GeoTrf::Transform3D ecalTransform = ecalGeneral->partTransform("Ecal");
  msg(MSG::DEBUG) << "Ecal detector translation: ( " << ecalTransform.translation().x() << " , " << ecalTransform.translation().y() << " , " << ecalTransform.translation().z() << " )" << endmsg;

  std::string rowA_Label = "BottomRow";
  std::string rowB_Label = "TopRow";

  bool rowA_Present = ecalGeneral->partPresent(rowA_Label);
  bool rowB_Present = ecalGeneral->partPresent(rowB_Label);

  //
  //  Module is the same for all rows
  //
  EcalModule module("Module", m_detectorManager, m_geometryManager, m_materials);
  msg(MSG::DEBUG) << "Created Ecal module with dimensions (" << module.width() << "," << module.height() << "," << module.length() << ")" << endmsg;
  //
  // Row A
  //
  if (rowA_Present)
  {
      msg(MSG::DEBUG) << "Building the Ecal Bottom Row." << endmsg;
      m_detectorManager->numerology().addRow(0);

    // Create the row
    EcalRow rowA("EcalBottomRow", &module, m_detectorManager, m_geometryManager, m_materials);
    EcalIdentifier id{m_geometryManager->athenaComps()->getIdHelper()};
    id.setRow(0);
    GeoVPhysVol* rowA_PV = rowA.build(id);
    GeoAlignableTransform* rowA_Transform = new GeoAlignableTransform(ecalTransform * ecalGeneral->partTransform(rowA_Label));
    msg(MSG::DEBUG) << "Ecal " << rowA_Label << " translation: ( " << rowA_Transform->getTransform().translation().x() << " , " << rowA_Transform->getTransform().translation().y() << " , " << rowA_Transform->getTransform().translation().z() << " )" << endmsg;

    GeoNameTag* topLevelNameTag = new GeoNameTag("Ecal");
    calo->add(topLevelNameTag);
    calo->add(new GeoIdentifierTag(0));
    calo->add(rowA_Transform);
    calo->add(rowA_PV);
    m_detectorManager->addTreeTop(rowA_PV);

    // Store alignable transform for row (level = 1)
    m_detectorManager->addAlignableTransform(1, id.getModuleId(), rowA_Transform, rowA_PV);
  }

  //
  // Row B
  //
  if (rowB_Present)
  {
      msg(MSG::DEBUG) << "Building the Ecal Top Row." << endmsg;
      m_detectorManager->numerology().addRow(1);

    // Create the row
    EcalRow rowB("EcalTopRow", &module, m_detectorManager, m_geometryManager, m_materials);
    EcalIdentifier id{m_geometryManager->athenaComps()->getIdHelper()};
    id.setRow(1);
    GeoVPhysVol* rowB_PV = rowB.build(id);
    GeoAlignableTransform* rowB_Transform = new GeoAlignableTransform(ecalTransform * ecalGeneral->partTransform(rowB_Label));
    msg(MSG::DEBUG) << "Ecal " << rowB_Label << " translation: ( " << rowB_Transform->getTransform().translation().x() << " , " << rowB_Transform->getTransform().translation().y() << " , " << rowB_Transform->getTransform().translation().z() << " )" << endmsg;

    GeoNameTag* topLevelNameTag = new GeoNameTag("Ecal");
    calo->add(topLevelNameTag);
    calo->add(new GeoIdentifierTag(1));
    calo->add(rowB_Transform);
    calo->add(rowB_PV);
    m_detectorManager->addTreeTop(rowB_PV);

    // Store alignable transform for row (level = 1)
    m_detectorManager->addAlignableTransform(1, id.getModuleId(), rowB_Transform, rowB_PV);
  }

  // Set the neighbours
  m_detectorManager->initNeighbours();

  // Set number of pmts in numerology.
  const CaloDD::CaloDetectorDesign* design = m_detectorManager->getEcalDesign();
  if (design != nullptr)
  {
    m_detectorManager->numerology().setNumPmtsPerModule(m_detectorManager->getEcalDesign()->cells());
  }
  else
  {
    m_detectorManager->numerology().setNumPmtsPerModule(0);
  }
  

  // Register the keys and the level corresponding to the key
  // and whether it expects a global or local shift.
  // level 0: sensor, level 1: module, level 2, layer/disc, level 3: whole barrel/enccap


  if (!m_useDynamicAlignFolders){

    m_detectorManager->addAlignFolderType(CaloDD::static_run1);
    // m_detectorManager->addFolder("/Calo/Align");
  }
  else {
    m_detectorManager->addAlignFolderType(CaloDD::timedependent_run2);
    // m_detectorManager->addGlobalFolder("/Calo/AlignL1/Calo");
    // m_detectorManager->addGlobalFolder("/Calo/AlignL2/Ecal");
    // m_detectorManager->addChannel("/Calo/AlignL1/Calo",3,CaloDD::global);
    // m_detectorManager->addChannel("/Calo/AlignL2/Ecal",2,CaloDD::global);
    // m_detectorManager->addFolder("/Calo/AlignL3");
  }

  // Return precision to its original value
  std::cout.precision(oldPrecision);

} 
 

const EcalDetectorManager * EcalDetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}
 

