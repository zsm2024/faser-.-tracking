/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EcalGeoModel/EcalDetectorTool.h"

#include "EcalDetectorFactory.h" 
#include "EcalDataBase.h" 
#include "EcalOptions.h" 

// temporary
#include "CaloReadoutGeometry/EcalDetectorManager.h" 
#include "FaserCaloIdentifier/EcalID.h"
#include "DetDescrConditions/AlignableTransformContainer.h"

#include "GeoModelFaserUtilities/GeoModelExperiment.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"
#include "StoreGate/DataHandle.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "AthenaKernel/ClassID_traits.h"
#include "SGTools/DataProxy.h"

using CaloDD::EcalDetectorManager;
using CaloDD::CaloDetectorManager;

//
// Constructor
//
EcalDetectorTool::EcalDetectorTool(const std::string& type,
                                   const std::string& name, 
                                   const IInterface* parent)
  : GeoModelTool(type, name, parent),
  m_cosmic{false},
  m_manager{nullptr},
  m_athenaComps{ },
  m_geoDbTagSvc{"GeoDbTagSvc", name},
  m_rdbAccessSvc{"RDBAccessSvc", name},
  m_geometryDBSvc{"CaloGeometryDBSvc", name}
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
EcalDetectorTool::create()
{ 
  // Get the detector configuration.
  ATH_CHECK(m_geoDbTagSvc.retrieve());
  
  DecodeFaserVersionKey versionKey{&*m_geoDbTagSvc, "Ecal"};
  // Issue error if AUTO.
  if (versionKey.tag() == "AUTO") {
    ATH_MSG_ERROR("AUTO Faser version. Please select a version.");
  }
  ATH_MSG_INFO("Building Ecal with Version Tag: " << versionKey.tag() << " at Node: " << versionKey.node());

  ATH_CHECK(m_rdbAccessSvc.retrieve());
  // Print the Ecal version tag:
  std::string ecalVersionTag{m_rdbAccessSvc->getChildTag("Ecal", versionKey.tag(), versionKey.node(), "FASERDD")};
  ATH_MSG_INFO("Ecal Version: " << ecalVersionTag);
  // Check if version is empty. If so, then the Ecal cannot be built. This may or may not be intentional. We
  // just issue an INFO message. 
  if (ecalVersionTag.empty()) {
    ATH_MSG_INFO("No Ecal Version. Ecal will not be built.");
  } else {
    std::string versionName;
    if (versionKey.custom()) {
      ATH_MSG_WARNING("EcalDetectorTool:  Detector Information coming from a custom configuration!!");
    } else {
      ATH_MSG_DEBUG("EcalDetectorTool:  Detector Information coming from the database and job options IGNORED.");
      ATH_MSG_DEBUG("Keys for Ecal Switches are "  << versionKey.tag()  << "  " << versionKey.node());

      IRDBRecordset_ptr switchSet{m_rdbAccessSvc->getRecordsetPtr("EcalSwitches", versionKey.tag(), versionKey.node(), "FASERDD")};
      if (!switchSet || switchSet->size() == 0)
      {
        ATH_MSG_WARNING("Unable to retrieve switches; Ecal cannot be created");
        return StatusCode::FAILURE;
      }
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
      if (not switches->isFieldNull("GDMLFILE") && m_gdmlFile.empty())
      {
        m_gdmlFile = switches->getString("GDMLFILE");
      } 
    }

    ATH_MSG_DEBUG("Creating the Ecal");
    ATH_MSG_DEBUG("Ecal Geometry Options: ");
    ATH_MSG_DEBUG(" GdmlFile:              " << m_gdmlFile.value());
    ATH_MSG_DEBUG(" Alignable:             " << (m_alignable.value() ? "true" : "false"));
    ATH_MSG_DEBUG(" CosmicLayout:          " << (m_cosmic ? "true" : "false"));
    ATH_MSG_DEBUG(" VersionName:           " << versionName);

    EcalOptions options;
    options.setAlignable(m_alignable.value());
    options.setDynamicAlignFolders(m_useDynamicAlignFolders.value());
    options.setGdmlFile(m_gdmlFile.value());
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
    const EcalID* idHelper{nullptr};
    ATH_CHECK(detStore()->retrieve(idHelper, "EcalID"));
    m_athenaComps.setIdHelper(idHelper);
    idHelper->test_module_packing();
    //
    // This strange way of casting is to avoid an
    // utterly brain damaged compiler warning.
    //
    GeoPhysVol* world{&*theExpt->getPhysVol()};
    if (world != nullptr) ATH_MSG_INFO("Retrieved World PhysVol");

    EcalDetectorFactory theEcal{&m_athenaComps, options};
    ATH_MSG_INFO("Created instance of detector factory");
    theEcal.create(world);
    ATH_MSG_INFO("Called create methon on factory");
    m_manager = theEcal.getDetectorManager();
    ATH_MSG_INFO("Attempted to retrieve detector manager");

    if (m_manager==nullptr) {
      ATH_MSG_FATAL("EcalDetectorManager not created");
      return StatusCode::FAILURE;
    }
      
    // Get the manager from the factory and store it in the detector store.
    //   m_detector is non constant so I can not set it to a const pointer.
    //   m_detector = theSCT.getDetectorManager();
      
    ATH_MSG_DEBUG("Registering EcalDetectorManager. ");
    ATH_CHECK(detStore()->record(m_manager, m_manager->getName()));
    theExpt->addManager(m_manager);
    
    // Create a symLink to the CaloDetectorManager base class
    const CaloDetectorManager* caloDetManager{m_manager};
    ATH_CHECK(detStore()->symLink(m_manager, caloDetManager));
  }

  return StatusCode::SUCCESS;
}

StatusCode 
EcalDetectorTool::clear()
{
  ATH_MSG_WARNING("Called untested EcalDetectorTool::clear()");
  SG::DataProxy* proxy{detStore()->proxy(ClassID_traits<EcalDetectorManager>::ID(), m_manager->getName())};
  if (proxy) {
    proxy->reset();
    m_manager = nullptr;
  }
  return StatusCode::SUCCESS;
}

StatusCode 
EcalDetectorTool::registerCallback ATLAS_NOT_THREAD_SAFE ()
{
  StatusCode sc{StatusCode::FAILURE};
  if (m_alignable.value()) {
    if (m_useDynamicAlignFolders.value()) {
      ATH_MSG_WARNING("Called unsupported EcalDetectorTool::registerCallback() with useDynamicAlignFolders = true");
      return sc;     

      // if (detStore()->contains<CondAttrListCollection>(m_run2L1Folder.value())) {
      //   ATH_MSG_DEBUG("Registering callback on global Container with folder " << m_run2L1Folder.value());
      //   const DataHandle<CondAttrListCollection> calc;
      //   ATH_CHECK(detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), calc, m_run2L1Folder.value()));
      //   sc = StatusCode::SUCCESS;
      // } else {
      //   ATH_MSG_WARNING("Unable to register callback on global Container with folder " << m_run2L1Folder.value());
      //   return StatusCode::FAILURE;
      // }
    
      // if (detStore()->contains<CondAttrListCollection>(m_run2L2Folder.value())) {
      //   ATH_MSG_DEBUG("Registering callback on global Container with folder " << m_run2L2Folder.value());
      //   const DataHandle<CondAttrListCollection> calc;
      //   ATH_CHECK(detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), calc, m_run2L2Folder.value()));
      //   sc = StatusCode::SUCCESS;
      // } else {
      //   ATH_MSG_WARNING("Unable to register callback on global Container with folder " << m_run2L2Folder.value());
      //   return StatusCode::FAILURE;
      // }
    
      // if (detStore()->contains<AlignableTransformContainer>(m_run2L3Folder.value())) {
      //   ATH_MSG_DEBUG("Registering callback on AlignableTransformContainer with folder " << m_run2L3Folder.value());
      //   const DataHandle<AlignableTransformContainer> atc;
      //   ATH_CHECK(detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), atc, m_run2L3Folder.value()));
      //   sc = StatusCode::SUCCESS;
      // } else {
      //   ATH_MSG_WARNING("Unable to register callback on AlignableTransformContainer with folder " << m_run2L3Folder.value());
      //   return StatusCode::FAILURE;
      // }
    } else {
      if (detStore()->contains<AlignableTransformContainer>(m_run1Folder.value())) {
        ATH_MSG_DEBUG("Registering callback on AlignableTransformContainer with folder " << m_run1Folder.value());
        const DataHandle<AlignableTransformContainer> atc;
        ATH_CHECK(detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool*>(this), atc, m_run1Folder.value()));
        sc = StatusCode::SUCCESS;
      } else {
        ATH_MSG_WARNING("Unable to register callback on AlignableTransformContainer with folder "
                        << m_run1Folder.value() << ", Alignment disabled (only if no Run2 scheme is loaded)!");
        return sc;
      }
    }
  } else {
    ATH_MSG_INFO("Alignment disabled. No callback registered");
    // We return failure otherwise it will try and register
    // a GeoModelSvc callback associated with this callback. 
    return sc;
  }
  return sc;
}
  
StatusCode 
EcalDetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I, keys))
{
  ATH_MSG_WARNING("Called untested EcalDetectorTool::align()");
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
