/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// EmulsionDetectorFactory: This is the top level node 
//


#include "EmulsionDetectorFactory.h" 

#include "EmulsionDataBase.h"
#include "EmulsionIdentifier.h"
#include "EmulsionGeometryManager.h" 
#include "EmulsionMaterialManager.h"
#include "EmulsionGeneralParameters.h"
#include "EmulsionSupportParameters.h"
#include "NeutrinoReadoutGeometry/Version.h" 
#include "NeutrinoReadoutGeometry/NeutrinoCommonItems.h" 
#include "NeutrinoReadoutGeometry/NeutrinoDD_Defs.h"
#include "NeutrinoReadoutGeometry/NeutrinoDetectorDesign.h" 

#include "EmulsionBase.h"
#include "EmulsionPlates.h"
#include "EmulsionDataBase.h"
#include "EmulsionGeoModel/EmulsionGeoModelAthenaComps.h"

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
 
using NeutrinoDD::EmulsionDetectorManager; 
using NeutrinoDD::NeutrinoCommonItems; 

EmulsionDetectorFactory::EmulsionDetectorFactory(const EmulsionGeoModelAthenaComps * athenaComps,
					                     const EmulsionOptions & options)
  : NeutrinoDD::DetectorFactoryBase(athenaComps),
    m_useDynamicAlignFolders(false)
{ 
  
  // Create the detector manager
  m_detectorManager = new EmulsionDetectorManager(detStore());
  msg(MSG::DEBUG) << "Created EmulsionDetectorManager" << endmsg;

  // Create the database
  m_db = new EmulsionDataBase{athenaComps};
  msg(MSG::DEBUG) << "Created EmulsionDataBase" << endmsg;

  // Create the material manager
  m_materials = new EmulsionMaterialManager{m_db};
  msg(MSG::DEBUG) << "Created EmulsionMaterialManager" << endmsg;

  // Create the geometry manager.
  m_geometryManager = new EmulsionGeometryManager{m_db};
  msg(MSG::DEBUG) << "Created EmulsionGeometryManager" << endmsg;
  m_geometryManager->setOptions(options);
  msg(MSG::DEBUG) << "Set options on EmulsionGeometryManager" << endmsg;

  m_useDynamicAlignFolders = options.dynamicAlignFolders();
 
  // Set Version information
  // Get the geometry tag
  DecodeFaserVersionKey versionKey(geoDbTagSvc(),"Emulsion");
  IRDBRecordset_ptr switchSet
    = rdbAccessSvc()->getRecordsetPtr("EmulsionSwitches", versionKey.tag(), versionKey.node(),"FASERDD");
  const IRDBRecord    *switches   = (*switchSet)[0];
  msg(MSG::DEBUG) << "Retrieved EmulsionSwitches" << endmsg;

  std::string layout = "Final";
  std::string description;
  if (!switches->isFieldNull("LAYOUT")) {
    layout = switches->getString("LAYOUT");
  }
  if (!switches->isFieldNull("DESCRIPTION")) {
    description = switches->getString("DESCRIPTION");
  }

  std::string versionTag = rdbAccessSvc()->getChildTag("Emulsion", versionKey.tag(), versionKey.node(),"FASERDD");
  std::string versionName = switches->getString("VERSIONNAME");
  int versionMajorNumber = 1;
  int versionMinorNumber = 0;
  int versionPatchNumber = 0;
  NeutrinoDD::Version version(versionTag,
                           versionName, 
                           layout, 
                           description, 
                           versionMajorNumber,
                           versionMinorNumber,
                           versionPatchNumber);
  m_detectorManager->setVersion(version);
} 
 
 
EmulsionDetectorFactory::~EmulsionDetectorFactory() 
{ 
  // NB the detector manager (m_detectorManager)is stored in the detector store by the
  // Tool and so we don't delete it.
  delete m_db;
  delete m_materials;
  delete m_geometryManager;
} 

void EmulsionDetectorFactory::create(GeoPhysVol *world) 
{ 

  msg(MSG::INFO) << "Building Emulsion Detector." << endmsg;
  msg(MSG::INFO) << " " << m_detectorManager->getVersion().fullDescription() << endmsg;

  // Change precision.
  int oldPrecision = std::cout.precision(6);

  // The tree tops get added to world. We name it "neutrino" though.
  GeoPhysVol *neutrino = world;

  // const EmulsionSupportParameters* emulsionSupport = m_geometryManager->supportParameters();
  // msg(MSG::ALWAYS) << "Found " << emulsionSupport->supportElements().size() << " emulsion support elements" << endmsg;

  const EmulsionGeneralParameters * emulsionGeneral = m_geometryManager->generalParameters();

  GeoTrf::Transform3D emulsionTransform = emulsionGeneral->partTransform("Emulsion");

  std::string stationA_Label = "StationA";

  bool stationA_Present = emulsionGeneral->partPresent(stationA_Label);

  //
  //  Plate is the same for all stations
  //
  // EmulsionBase base("Base", m_detectorManager, m_geometryManager, m_materials);
  // msg(MSG::DEBUG) << "Created Emulsion base with dimensions (" << base.width() << "," << base.height() << "," << base.thickness() << ")" << endmsg;
  //
  // Station A
  //
  if (stationA_Present)
  {
    msg(MSG::DEBUG) << "Building the Emulsion Station A." << endmsg;

    // // Create the station
    EmulsionPlates stationA("EmulsionStationA", m_detectorManager, m_geometryManager, m_materials);
    EmulsionIdentifier id{m_geometryManager->athenaComps()->getIdHelper()};

    GeoVPhysVol* stationA_PV = stationA.build(id);
    GeoAlignableTransform* stationA_Transform = new GeoAlignableTransform(emulsionTransform * emulsionGeneral->partTransform(stationA_Label));

    GeoNameTag* topLevelNameTag = new GeoNameTag("Emulsion");
    neutrino->add(topLevelNameTag);
    neutrino->add(new GeoIdentifierTag(0));
    neutrino->add(stationA_Transform);
    neutrino->add(stationA_PV);
    m_detectorManager->addTreeTop(stationA_PV);

    // // Store alignable transform for station (level = 1)
    m_detectorManager->addAlignableTransform(2, id.getFilmId(), stationA_Transform, stationA_PV);
  }

  // Set the neighbours
  m_detectorManager->initNeighbours();

  // Set number of pmts in numerology.
  // const NeutrinoDD::NeutrinoDetectorDesign* design = m_detectorManager->getEmulsionDesign();
  // if (design != nullptr)
  // {
  //   m_detectorManager->numerology().setNumFilmsPerBase(m_detectorManager->getEmulsionDesign()->cells());
  // }
  // else
  // {
  //   m_detectorManager->numerology().setNumPmtsPerPlate(0);
  // }
  

  // Register the keys and the level corresponding to the key
  // and whether it expects a global or local shift.
  // level 0: sensor, level 1: module, level 2, layer/disc, level 3: whole barrel/enccap


  if (!m_useDynamicAlignFolders){

    m_detectorManager->addAlignFolderType(NeutrinoDD::static_run1);
    // m_detectorManager->addFolder("/Neutrino/Align");
  }
  else {
    m_detectorManager->addAlignFolderType(NeutrinoDD::timedependent_run2);
    // m_detectorManager->addGlobalFolder("/Neutrino/AlignL1/Neutrino");
    // m_detectorManager->addGlobalFolder("/Neutrino/AlignL2/Emulsion");
    // m_detectorManager->addChannel("/Neutrino/AlignL1/Neutrino",3,NeutrinoDD::global);
    // m_detectorManager->addChannel("/Neutrino/AlignL2/Emulsion",2,NeutrinoDD::global);
    // m_detectorManager->addFolder("/Neutrino/AlignL3");
  }

  // Return precision to its original value
  std::cout.precision(oldPrecision);

} 
 

const EmulsionDetectorManager * EmulsionDetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}
 

