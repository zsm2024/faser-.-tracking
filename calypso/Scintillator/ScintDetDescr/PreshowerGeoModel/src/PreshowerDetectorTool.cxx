/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerGeoModel/PreshowerDetectorTool.h"

#include "PreshowerDetectorFactory.h" 
#include "PreshowerDataBase.h" 
// #include "PreshowerMaterialManager.h" 
#include "PreshowerOptions.h" 

// temporary
#include "ScintReadoutGeometry/PreshowerDetectorManager.h" 
#include "ScintIdentifier/PreshowerID.h"
#include "DetDescrConditions/AlignableTransformContainer.h"

#include "GeoModelFaserUtilities/GeoModelExperiment.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"
#include "StoreGate/DataHandle.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "AthenaKernel/ClassID_traits.h"
#include "SGTools/DataProxy.h"

using ScintDD::PreshowerDetectorManager;
using ScintDD::ScintDetectorManager;

//
// Constructor
//
PreshowerDetectorTool::PreshowerDetectorTool(const std::string& type,
                                   const std::string& name, 
                                   const IInterface* parent)
  : GeoModelTool(type, name, parent),
  m_cosmic{false},
  m_manager{nullptr},
  m_athenaComps{ },
  m_geoDbTagSvc{"GeoDbTagSvc", name},
  m_rdbAccessSvc{"RDBAccessSvc", name},
  m_geometryDBSvc{"ScintGeometryDBSvc", name}
{
  // Get parameter values from jobOptions file
  declareProperty("GeoDbTagSvc", m_geoDbTagSvc);
  declareProperty("RDBAccessSvc", m_rdbAccessSvc);
  declareProperty("GeometryDBSvc", m_geometryDBSvc);
}

//
// Create the Geometry via the factory corresponding to this tool
//

StatusCode
PreshowerDetectorTool::create()
{ 
  // Get the detector configuration.
  ATH_CHECK(m_geoDbTagSvc.retrieve());
  
  DecodeFaserVersionKey versionKey{&*m_geoDbTagSvc, "Preshower"};
  // Issue error if AUTO.
  if (versionKey.tag() == "AUTO") {
    ATH_MSG_ERROR("AUTO Faser version. Please select a version.");
  }
  ATH_MSG_INFO("Building Preshower with Version Tag: " << versionKey.tag() << " at Node: " << versionKey.node());

  ATH_CHECK(m_rdbAccessSvc.retrieve());
  // Print the Preshower version tag:
  std::string preshowerVersionTag{m_rdbAccessSvc->getChildTag("Preshower", versionKey.tag(), versionKey.node(), "FASERDD")};
  ATH_MSG_INFO("Preshower Version: " << preshowerVersionTag);
  // Check if version is empty. If so, then the Preshower cannot be built. This may or may not be intentional. We
  // just issue an INFO message. 
  if (preshowerVersionTag.empty()) {
    ATH_MSG_INFO("No Preshower Version. Preshower will not be built.");
  } else {
    std::string versionName;
    if (versionKey.custom()) {
      ATH_MSG_WARNING("PreshowerDetectorTool:  Detector Information coming from a custom configuration!!");
    } else {
      ATH_MSG_DEBUG("PreshowerDetectorTool:  Detector Information coming from the database and job options IGNORED.");
      ATH_MSG_DEBUG("Keys for Preshower Switches are "  << versionKey.tag()  << "  " << versionKey.node());

      IRDBRecordset_ptr switchSet{m_rdbAccessSvc->getRecordsetPtr("PreshowerSwitches", versionKey.tag(), versionKey.node(), "FASERDD")};
      const IRDBRecord* switches{(*switchSet)[0]};
      m_detectorName.setValue(switches->getString("DETECTORNAME"));

      if (not switches->isFieldNull("COSMICLAYOUT")) 
      {
        m_cosmic = switches->getInt("COSMICLAYOUT");
      }
      if (not switches->isFieldNull("VERSIONNAME")) 
      {
        versionName = switches->getString("VERSIONNAME");
      } 
    }

    ATH_MSG_DEBUG("Creating the Preshower");
    ATH_MSG_DEBUG("Preshower Geometry Options: ");
    ATH_MSG_DEBUG(" Alignable:             " << (m_alignable.value() ? "true" : "false"));
    ATH_MSG_DEBUG(" CosmicLayout:          " << (m_cosmic ? "true" : "false"));
    ATH_MSG_DEBUG(" VersionName:           " << versionName);

    PreshowerOptions options;
    options.setAlignable(m_alignable.value());
    options.setDynamicAlignFolders(m_useDynamicAlignFolders.value());
    m_manager = nullptr;

    // 
    // Locate the top level experiment node 
    // 
    GeoModelExperiment* theExpt{nullptr};
    ATH_CHECK(detStore()->retrieve(theExpt, "FASER"));
      
    // Retrieve the Geometry DB Interface
    ATH_CHECK(m_geometryDBSvc.retrieve());

    // Pass athena services to factory, etc
    m_athenaComps.setDetStore(detStore().operator->());
    m_athenaComps.setGeoDbTagSvc(&*m_geoDbTagSvc);
    m_athenaComps.setGeometryDBSvc(&*m_geometryDBSvc);
    m_athenaComps.setRDBAccessSvc(&*m_rdbAccessSvc);
    const PreshowerID* idHelper{nullptr};
    ATH_CHECK(detStore()->retrieve(idHelper, "PreshowerID"));
    m_athenaComps.setIdHelper(idHelper);
    idHelper->test_plate_packing();
    //
    // This strange way of casting is to avoid an
    // utterly brain damaged compiler warning.
    //
    GeoPhysVol* world{&*theExpt->getPhysVol()};
    if (world != nullptr) ATH_MSG_INFO("Retrieved World PhysVol");

    PreshowerDetectorFactory thePreshower{&m_athenaComps, options};
    ATH_MSG_INFO("Created instance of detector factory");
    thePreshower.create(world);
    ATH_MSG_INFO("Called create methon on factory");
    m_manager = thePreshower.getDetectorManager();
    ATH_MSG_INFO("Attempted to retrieve detector manager");

    if (m_manager==nullptr) {
      ATH_MSG_FATAL("PreshowerDetectorManager not created");
      return StatusCode::FAILURE;
    }
      
    // Get the manager from the factory and store it in the detector store.
    //   m_detector is non constant so I can not set it to a const pointer.
    //   m_detector = theSCT.getDetectorManager();
      
    ATH_MSG_DEBUG("Registering PreshowerDetectorManager. ");
    ATH_CHECK(detStore()->record(m_manager, m_manager->getName()));
    theExpt->addManager(m_manager);
    
    // Create a symLink to the ScintDetectorManager base class
    const ScintDetectorManager* scintDetManager{m_manager};
    ATH_CHECK(detStore()->symLink(m_manager, scintDetManager));
  }

  return StatusCode::SUCCESS;
}

StatusCode 
PreshowerDetectorTool::clear()
{
  ATH_MSG_WARNING("Called untested PreshowerDetectorTool::clear()");
  SG::DataProxy* proxy{detStore()->proxy(ClassID_traits<PreshowerDetectorManager>::ID(), m_manager->getName())};
  if (proxy) {
    proxy->reset();
    m_manager = nullptr;
  }
  return StatusCode::SUCCESS;
}

StatusCode 
PreshowerDetectorTool::registerCallback()
{
  StatusCode sc{StatusCode::FAILURE};
  if (m_alignable.value()) {
      ATH_MSG_WARNING("Called untested PreshowerDetectorTool::registerCallback()");
    if (m_useDynamicAlignFolders.value()) {

      if (detStore()->contains<CondAttrListCollection>(m_run2L1Folder.value())) {
        ATH_MSG_DEBUG("Registering callback on global Container with folder " << m_run2L1Folder.value());
        const DataHandle<CondAttrListCollection> calc;
        ATH_CHECK(detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), calc, m_run2L1Folder.value()));
        sc = StatusCode::SUCCESS;
      } else {
        ATH_MSG_WARNING("Unable to register callback on global Container with folder " << m_run2L1Folder.value());
        return StatusCode::FAILURE;
      }
    
      if (detStore()->contains<CondAttrListCollection>(m_run2L2Folder.value())) {
        ATH_MSG_DEBUG("Registering callback on global Container with folder " << m_run2L2Folder.value());
        const DataHandle<CondAttrListCollection> calc;
        ATH_CHECK(detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), calc, m_run2L2Folder.value()));
        sc = StatusCode::SUCCESS;
      } else {
        ATH_MSG_WARNING("Unable to register callback on global Container with folder " << m_run2L2Folder.value());
        return StatusCode::FAILURE;
      }
    
      if (detStore()->contains<AlignableTransformContainer>(m_run2L3Folder.value())) {
        ATH_MSG_DEBUG("Registering callback on AlignableTransformContainer with folder " << m_run2L3Folder.value());
        const DataHandle<AlignableTransformContainer> atc;
        ATH_CHECK(detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), atc, m_run2L3Folder.value()));
        sc = StatusCode::SUCCESS;
      } else {
        ATH_MSG_WARNING("Unable to register callback on AlignableTransformContainer with folder " << m_run2L3Folder.value());
        return StatusCode::FAILURE;
      }
     
    } else {
    
      if (detStore()->contains<AlignableTransformContainer>(m_run1Folder.value())) {
        ATH_MSG_DEBUG("Registering callback on AlignableTransformContainer with folder " << m_run1Folder.value());
        const DataHandle<AlignableTransformContainer> atc;
        ATH_CHECK(detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), atc, m_run1Folder.value()));
        sc = StatusCode::SUCCESS;
      } else {
        ATH_MSG_WARNING("Unable to register callback on AlignableTransformContainer with folder "
                        << m_run1Folder.value() << ", Alignment disabled (only if no Run2 scheme is loaded)!");
        return StatusCode::FAILURE;
      }
    }
  } else {
    ATH_MSG_INFO("Alignment disabled. No callback registered");
    // We return failure otherwise it will try and register
    // a GeoModelSvc callback associated with this callback. 
    return StatusCode::FAILURE;
  }
  return sc;
}
  
StatusCode 
PreshowerDetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I, keys))
{
  ATH_MSG_WARNING("Called untested PreshowerDetectorTool::align()");
  void* i = &I;
  void* k = &keys;
  if (i == nullptr && k == nullptr) return StatusCode::SUCCESS; // suppress stupid warning
  if (m_manager==nullptr) { 
    ATH_MSG_FATAL("Manager does not exist");
    return StatusCode::FAILURE;
  }    
  if (m_alignable.value()) {
    return m_manager->align(I, keys);
  } else {
    ATH_MSG_DEBUG("Alignment disabled. No alignments applied");
    return StatusCode::SUCCESS;
  }
}
