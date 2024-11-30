/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// SCT_DetectorFactory: This is the top level node 
//


#include "SCT_DetectorFactory.h" 

#include "SCT_DataBase.h"
#include "SCT_Identifier.h"
#include "SCT_GeometryManager.h" 
#include "SCT_MaterialManager.h"
#include "SCT_GeneralParameters.h"
#include "TrackerReadoutGeometry/Version.h" 
#include "TrackerReadoutGeometry/SiCommonItems.h" 
#include "TrackerReadoutGeometry/TrackerDD_Defs.h"
#include "TrackerReadoutGeometry/SCT_ModuleSideDesign.h" 
#include "SCT_Wrapping.h"

// #include "SCT_Barrel.h"
#include "SCT_Station.h"
#include "FaserSCT_GeoModel/SCT_GeoModelAthenaComps.h"

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
 
using TrackerDD::SCT_DetectorManager; 
using TrackerDD::SiCommonItems; 

SCT_DetectorFactory::SCT_DetectorFactory(const SCT_GeoModelAthenaComps * athenaComps,
					 const SCT_Options & options)
  : TrackerDD::DetectorFactoryBase(athenaComps),
    m_useDynamicAlignFolders(false)
{ 
  
  // Create the detector manager
  m_detectorManager = new SCT_DetectorManager(detStore());
  m_detectorManager->msg(MSG::Level::VERBOSE).setLevel(MSG::Level::VERBOSE);

  // Create the database
  m_db = new SCT_DataBase{athenaComps};

  // Create the material manager
  m_materials = new SCT_MaterialManager{m_db};

  // Create the geometry manager.
  m_geometryManager = new SCT_GeometryManager{m_db};
  m_geometryManager->setOptions(options);

  m_useDynamicAlignFolders = options.dynamicAlignFolders();
 
  // Set Version information
  // Get the geometry tag
  DecodeFaserVersionKey versionKey(geoDbTagSvc(),"SCT");
  IRDBRecordset_ptr switchSet
    = rdbAccessSvc()->getRecordsetPtr("SctSwitches", versionKey.tag(), versionKey.node(), "FASERDD");
  const IRDBRecord    *switches   = (*switchSet)[0];
  
  std::string layout = "Final";
  std::string description;
  if (!switches->isFieldNull("LAYOUT")) {
    layout = switches->getString("LAYOUT");
  }
  if (!switches->isFieldNull("DESCRIPTION")) {
    description = switches->getString("DESCRIPTION");
  }

  std::string versionTag = rdbAccessSvc()->getChildTag("SCT", versionKey.tag(), versionKey.node(),"FASERDD");
  std::string versionName = switches->getString("VERSIONNAME");
  int versionMajorNumber = 3;
  int versionMinorNumber = 6;
  int versionPatchNumber = 0;
  TrackerDD::Version version(versionTag,
                           versionName, 
                           layout, 
                           description, 
                           versionMajorNumber,
                           versionMinorNumber,
                           versionPatchNumber);
  m_detectorManager->setVersion(version);

} 
 
 
SCT_DetectorFactory::~SCT_DetectorFactory() 
{ 
  // NB the detector manager (m_detectorManager)is stored in the detector store by the
  // Tool and so we don't delete it.
  delete m_db;
  delete m_materials;
  delete m_geometryManager;
} 

void SCT_DetectorFactory::create(GeoPhysVol *world) 
{ 

  msg(MSG::INFO) << "Building SCT Detector." << endmsg;
  msg(MSG::INFO) << " " << m_detectorManager->getVersion().fullDescription() << endmsg;

  // Change precision.
  int oldPrecision = std::cout.precision(6);

  // The tree tops get added to world. We name it "tracker" though.
  GeoPhysVol *tracker = world;

  const SCT_GeneralParameters * sctGeneral = m_geometryManager->generalParameters();

  GeoTrf::Transform3D sctTransform = sctGeneral->partTransform("SCT");
  // SCT_Barrel station("Station", m_detectorManager, m_geometryManager, m_materials);
  SCT_Wrapping wrapping("Wrapping", m_detectorManager, m_geometryManager, m_materials);
  SCT_Station station("Station", &wrapping, m_detectorManager, m_geometryManager, m_materials);


  std::vector<std::string> partNames {"Interface", "StationA", "StationB", "StationC"};
  for (size_t iStation = 0; iStation < partNames.size(); iStation++)
  {
    if (sctGeneral->partPresent(partNames[iStation]))
    {
      m_detectorManager->numerology().addBarrel(iStation);
      SCT_Identifier id{m_geometryManager->athenaComps()->getIdHelper()};
      id.setStation(iStation);
      tracker->add(new GeoNameTag("SCT"));
      tracker->add(new GeoIdentifierTag(iStation));
      GeoAlignableTransform* stationTransform = new GeoAlignableTransform(sctTransform * sctGeneral->partTransform(partNames[iStation]));
      tracker->add(stationTransform);
      GeoVPhysVol* stationPhys = station.build(id);
      tracker->add(stationPhys);
      m_detectorManager->addTreeTop(stationPhys);
      m_detectorManager->addAlignableTransform(3, id.getWaferId(), stationTransform, stationPhys);
    }
  }

  // Set the neighbours
  m_detectorManager->initNeighbours();

  // Set maximum number of strips in numerology.
  for (int iDesign = 0;  iDesign <  m_detectorManager->numDesigns(); iDesign++) {
    m_detectorManager->numerology().setMaxNumPhiCells(m_detectorManager->getSCT_Design(iDesign)->cells());
  }

  // Register the keys and the level corresponding to the key
  // and whether it expects a global or local shift.
  // level 0: sensor, level 1: module, level 2, layer/disc, level 3: whole barrel/enccap


  if (!m_useDynamicAlignFolders){

    m_detectorManager->addAlignFolderType(TrackerDD::static_run1);
    m_detectorManager->addFolder("/Tracker/Align");
    m_detectorManager->addChannel("/Tracker/Align/Stations",    3, TrackerDD::global);  // Stations in world
    m_detectorManager->addChannel("/Tracker/Align/Planes",      2, TrackerDD::global);  // Planes in world
    m_detectorManager->addChannel("/Tracker/Align/Interface1",   1, TrackerDD::local);  // Modules in planes
    m_detectorManager->addChannel("/Tracker/Align/Interface2",   1, TrackerDD::local);
    m_detectorManager->addChannel("/Tracker/Align/Interface3",   1, TrackerDD::local);
    m_detectorManager->addChannel("/Tracker/Align/Upstream1",   1, TrackerDD::local);  
    m_detectorManager->addChannel("/Tracker/Align/Upstream2",   1, TrackerDD::local);
    m_detectorManager->addChannel("/Tracker/Align/Upstream3",   1, TrackerDD::local);
    m_detectorManager->addChannel("/Tracker/Align/Central1" ,   1, TrackerDD::local);
    m_detectorManager->addChannel("/Tracker/Align/Central2" ,   1, TrackerDD::local);
    m_detectorManager->addChannel("/Tracker/Align/Central3" ,   1, TrackerDD::local);
    m_detectorManager->addChannel("/Tracker/Align/Downstream1", 1, TrackerDD::local);
    m_detectorManager->addChannel("/Tracker/Align/Downstream2", 1, TrackerDD::local);
    m_detectorManager->addChannel("/Tracker/Align/Downstream3", 1, TrackerDD::local);
    // m_detectorManager->addFolder("/Indet/Align");
    // m_detectorManager->addChannel("/Indet/Align/ID",3,InDetDD::global);
    // m_detectorManager->addChannel("/Indet/Align/SCT",2,InDetDD::global);
    
    // if (barrelPresent) {
    //   m_detectorManager->addChannel("/Indet/Align/SCTB1",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTB2",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTB3",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTB4",1,InDetDD::local);
    // }
    // if (forwardPlusPresent) {
    //   m_detectorManager->addChannel("/Indet/Align/SCTEA1",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEA2",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEA3",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEA4",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEA5",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEA6",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEA7",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEA8",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEA9",1,InDetDD::local);
    // }
    // if  (forwardMinusPresent) {
    //   m_detectorManager->addChannel("/Indet/Align/SCTEC1",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEC2",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEC3",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEC4",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEC5",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEC6",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEC7",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEC8",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/Align/SCTEC9",1,InDetDD::local);
    // }
  }
  
  else {
    msg(MSG::FATAL) << "Dynamic alignment not supported." << endmsg;
    // m_detectorManager->addAlignFolderType(TrackerDD::timedependent_run2);
    // m_detectorManager->addGlobalFolder("/Indet/AlignL1/ID");
    // m_detectorManager->addGlobalFolder("/Indet/AlignL2/SCT");
    // m_detectorManager->addChannel("/Indet/AlignL1/ID",3,InDetDD::global);
    // m_detectorManager->addChannel("/Indet/AlignL2/SCT",2,InDetDD::global);
    // m_detectorManager->addFolder("/Indet/AlignL3");

    // if (barrelPresent) {
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTB1",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTB2",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTB3",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTB4",1,InDetDD::local);
    // }
    // if (forwardPlusPresent) {
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEA1",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEA2",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEA3",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEA4",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEA5",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEA6",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEA7",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEA8",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEA9",1,InDetDD::local);
    // }
    // if  (forwardMinusPresent) {
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEC1",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEC2",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEC3",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEC4",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEC5",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEC6",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEC7",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEC8",1,InDetDD::local);
    //   m_detectorManager->addChannel("/Indet/AlignL3/SCTEC9",1,InDetDD::local);
    // }
  }

  // Return precision to its original value
  std::cout.precision(oldPrecision);

} 
 

const SCT_DetectorManager * SCT_DetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}
 

