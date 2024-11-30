/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// PreshowerDetectorFactory: This is the top level node 
//


#include "PreshowerDetectorFactory.h" 

#include "PreshowerDataBase.h"
#include "PreshowerIdentifier.h"
#include "PreshowerGeometryManager.h" 
#include "PreshowerMaterialManager.h"
#include "PreshowerGeneralParameters.h"
#include "ScintReadoutGeometry/Version.h" 
#include "ScintReadoutGeometry/ScintCommonItems.h" 
#include "ScintReadoutGeometry/ScintDD_Defs.h"
#include "ScintReadoutGeometry/ScintDetectorDesign.h" 

#include "PreshowerStation.h"
#include "PreshowerPlate.h"
#include "PreshowerWrapping.h"
#include "PreshowerDataBase.h"
#include "PreshowerGeoModel/PreshowerGeoModelAthenaComps.h"

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
 
using ScintDD::PreshowerDetectorManager; 
using ScintDD::ScintCommonItems; 

PreshowerDetectorFactory::PreshowerDetectorFactory(const PreshowerGeoModelAthenaComps * athenaComps,
					                     const PreshowerOptions & options)
  : ScintDD::DetectorFactoryBase(athenaComps),
    m_useDynamicAlignFolders(false)
{ 
  
  // Create the detector manager
  m_detectorManager = new PreshowerDetectorManager(detStore());
  msg(MSG::DEBUG) << "Created PreshowerDetectorManager" << endmsg;

  // Create the database
  m_db = new PreshowerDataBase{athenaComps};
  msg(MSG::DEBUG) << "Created PreshowerDataBase" << endmsg;

  // Create the material manager
  m_materials = new PreshowerMaterialManager{m_db};
  msg(MSG::DEBUG) << "Created PreshowerMaterialManager" << endmsg;

  // Create the geometry manager.
  m_geometryManager = new PreshowerGeometryManager{m_db};
  msg(MSG::DEBUG) << "Created PreshowerGeometryManager" << endmsg;
  m_geometryManager->setOptions(options);
  msg(MSG::DEBUG) << "Set options on PreshowerGeometryManager" << endmsg;

  m_useDynamicAlignFolders = options.dynamicAlignFolders();
 
  // Set Version information
  // Get the geometry tag
  DecodeFaserVersionKey versionKey(geoDbTagSvc(),"Preshower");
  IRDBRecordset_ptr switchSet
    = rdbAccessSvc()->getRecordsetPtr("PreshowerSwitches", versionKey.tag(), versionKey.node(),"FASERDD");
  const IRDBRecord    *switches   = (*switchSet)[0];
  msg(MSG::DEBUG) << "Retrieved PreshowerSwitches" << endmsg;

  std::string layout = "Final";
  std::string description;
  if (!switches->isFieldNull("LAYOUT")) {
    layout = switches->getString("LAYOUT");
  }
  if (!switches->isFieldNull("DESCRIPTION")) {
    description = switches->getString("DESCRIPTION");
  }

  std::string versionTag = rdbAccessSvc()->getChildTag("Preshower", versionKey.tag(), versionKey.node(),"FASERDD");
  std::string versionName = switches->getString("VERSIONNAME");
  int versionMajorNumber = 1;
  int versionMinorNumber = 0;
  int versionPatchNumber = 0;
  ScintDD::Version version(versionTag,
                           versionName, 
                           layout, 
                           description, 
                           versionMajorNumber,
                           versionMinorNumber,
                           versionPatchNumber);
  m_detectorManager->setVersion(version);
} 
 
 
PreshowerDetectorFactory::~PreshowerDetectorFactory() 
{ 
  // NB the detector manager (m_detectorManager)is stored in the detector store by the
  // Tool and so we don't delete it.
  delete m_db;
  delete m_materials;
  delete m_geometryManager;
} 

void PreshowerDetectorFactory::create(GeoPhysVol *world) 
{ 

  msg(MSG::INFO) << "Building Preshower Detector." << endmsg;
  msg(MSG::INFO) << " " << m_detectorManager->getVersion().fullDescription() << endmsg;

  // Change precision.
  int oldPrecision = std::cout.precision(6);

  // The tree tops get added to world. We name it "indet" though.
  GeoPhysVol *scint = world;

  const PreshowerGeneralParameters * preshowerGeneral = m_geometryManager->generalParameters();

  GeoTrf::Transform3D preshowerTransform = preshowerGeneral->partTransform("Preshower");

    std::string stationA_Label = "StationA";
    // std::string stationB_Label = "StationB";
    // std::string absorberA_Label = "AbsorberA";
    // std::string absorberB_Label = "AbsorberB";

    bool stationA_Present = preshowerGeneral->partPresent(stationA_Label);
    // bool stationB_Present = preshowerGeneral->partPresent(stationB_Label);
    // bool absorberA_Present = preshowerGeneral->partPresent(absorberA_Label);
    // bool absorberB_Present = preshowerGeneral->partPresent(absorberB_Label);

    //
    //  Plate is the same for all stations
    //
    PreshowerPlate plate("Plate", m_detectorManager, m_geometryManager, m_materials);
    msg(MSG::DEBUG) << "Created Preshower plate with dimensions (" << plate.thickness() << "," << plate.width() << "," << plate.length() << ")" << endmsg;
    PreshowerWrapping wrapping("Wrapping", m_detectorManager, m_geometryManager, m_materials);
    msg(MSG::DEBUG) << "Created Preshower wrapping with dimensions (" << wrapping.thickness() << "," << wrapping.width() << "," << wrapping.length() << ")" << endmsg;
  //
  // Station A
  //
  if (stationA_Present)
  {
      msg(MSG::DEBUG) << "Building the Preshower Station A." << endmsg;
      m_detectorManager->numerology().addStation(0);

    // Create the station
    PreshowerStation stationA("PreshowerStationA", &plate, &wrapping, m_detectorManager, m_geometryManager, m_materials);
    PreshowerIdentifier id{m_geometryManager->athenaComps()->getIdHelper()};
    id.setStation(0);
    GeoVPhysVol* stationA_PV = stationA.build(id);
    GeoAlignableTransform* stationA_Transform = new GeoAlignableTransform(preshowerTransform * preshowerGeneral->partTransform(stationA_Label));

    GeoNameTag* topLevelNameTag = new GeoNameTag("Preshower");
    scint->add(topLevelNameTag);
    scint->add(new GeoIdentifierTag(0));
    scint->add(stationA_Transform);
    scint->add(stationA_PV);
    m_detectorManager->addTreeTop(stationA_PV);

    // Store alignable transform for station (level = 1)
    m_detectorManager->addAlignableTransform(1, id.getPlateId(), stationA_Transform, stationA_PV);
  }

  //
  // Station B
  //
  // if (stationB_Present)
  // {
  //     msg(MSG::DEBUG) << "Building the Preshower Station B." << endmsg;
  //     m_detectorManager->numerology().addStation(1);

  //   // Create the station
  //   PreshowerStation stationB("PreshowerStationB", &plate, m_detectorManager, m_geometryManager, m_materials);
  //   PreshowerIdentifier id{m_geometryManager->athenaComps()->getIdHelper()};
  //   id.setStation(1);
  //   GeoVPhysVol* stationB_PV = stationB.build(id);
  //   GeoAlignableTransform* stationB_Transform = new GeoAlignableTransform(preshowerTransform * preshowerGeneral->partTransform(stationB_Label));

  //   GeoNameTag* topLevelNameTag = new GeoNameTag("Preshower");
  //   scint->add(topLevelNameTag);
  //   scint->add(new GeoIdentifierTag(0));
  //   scint->add(stationB_Transform);
  //   scint->add(stationB_PV);
  //   m_detectorManager->addTreeTop(stationB_PV);

  //   // Store alignable transform for station (level = 1)
  //   m_detectorManager->addAlignableTransform(1, id.getPlateId(), stationB_Transform, stationB_PV);
  // }

  // Set the neighbours
  m_detectorManager->initNeighbours();

  // Set number of pmts in numerology.
  const ScintDD::ScintDetectorDesign* design = m_detectorManager->getPreshowerDesign();
  if (design != nullptr)
  {
    m_detectorManager->numerology().setNumPmtsPerPlate(m_detectorManager->getPreshowerDesign()->cells());
  }
  else
  {
    m_detectorManager->numerology().setNumPmtsPerPlate(0);
  }
  

  // Register the keys and the level corresponding to the key
  // and whether it expects a global or local shift.
  // level 0: sensor, level 1: module, level 2, layer/disc, level 3: whole barrel/enccap


  if (!m_useDynamicAlignFolders){

    m_detectorManager->addAlignFolderType(ScintDD::static_run1);
    // m_detectorManager->addFolder("/Scint/Align");
  }
  else {
    m_detectorManager->addAlignFolderType(ScintDD::timedependent_run2);
    // m_detectorManager->addGlobalFolder("/Scint/AlignL1/Scint");
    // m_detectorManager->addGlobalFolder("/Scint/AlignL2/Preshower");
    // m_detectorManager->addChannel("/Scint/AlignL1/Scint",3,ScintDD::global);
    // m_detectorManager->addChannel("/Scint/AlignL2/Preshower",2,ScintDD::global);
    // m_detectorManager->addFolder("/Scint/AlignL3");
  }

  // Return precision to its original value
  std::cout.precision(oldPrecision);

} 
 

const PreshowerDetectorManager * PreshowerDetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}
 

