/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DipoleGeoModel/DipoleTool.h"
#include "DipoleGeoModel/DipoleFactory.h"
#include "DipoleGeoModel/DipoleGeoModelAthenaComps.h"

#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "GeoModelFaserUtilities/GeoModelExperiment.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaKernel/ClassID_traits.h"
#include "SGTools/DataProxy.h"



/**
 ** Constructor(s)
 **/
DipoleTool::DipoleTool( const std::string& type, const std::string& name, const IInterface* parent )
  : GeoModelTool( type, name, parent ),
    m_geoDbTagSvc("GeoDbTagSvc",name),
    m_rdbAccessSvc("RDBAccessSvc",name),
    m_geometryDBSvc("TrackerGeometryDBSvc",name),
    m_manager(0),
    m_athenaComps(0)
{
  declareProperty("OverrideVersionName", m_overrideVersionName);
  declareProperty("RDBAccessSvc", m_rdbAccessSvc);
  declareProperty("GeometryDBSvc", m_geometryDBSvc);
  declareProperty("GeoDbTagSvc", m_geoDbTagSvc);
  // add here the properties
}
/**
 ** Destructor
 **/
DipoleTool::~DipoleTool()
{
  delete m_athenaComps;
}

/**
 ** Create the Detector Node corresponding to this tool
 **/
StatusCode DipoleTool::create()
{ 
  // 
  // Locate the top level experiment node 
  // 
  GeoModelExperiment * theExpt; 
  if (StatusCode::SUCCESS != detStore()->retrieve( theExpt, "FASER" )) { 
    msg(MSG::ERROR) 
        << "Could not find GeoModelExperiment FASER" 
        << endmsg; 
    return (StatusCode::FAILURE); 
  } 
  
  StatusCode result = StatusCode::SUCCESS;


  // Get the detector configuration.
  StatusCode sc = m_geoDbTagSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate GeoDbTagSvc" << endmsg;
    return (StatusCode::FAILURE);
  } 
  
  DecodeFaserVersionKey versionKey(&*m_geoDbTagSvc, "Tracker");
  
  sc = m_rdbAccessSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate RDBAccessSvc" << endmsg;
    return (StatusCode::FAILURE); 
  }  
  
  std::string versionTag = m_rdbAccessSvc->getChildTag("Dipole", versionKey.tag(), versionKey.node(), "FASERDD");
  if(msgLvl(MSG::DEBUG)) msg() << "versionTag=" << versionTag <<" %%%"<< endmsg;

  // If versionTag is NULL then don't build.
  if (versionTag.empty()) { 
    msg(MSG::INFO)  << "No Dipole Version. Dipole will not be built." << endmsg;
    if(msgLvl(MSG::DEBUG)) msg() << "Tracker Version Tag: " << versionKey.tag() << " at Node: " 
	<< versionKey.node() << endmsg;
    return StatusCode::SUCCESS;
  } 

  if(msgLvl(MSG::DEBUG)) msg() << "Keys for Dipole Switches are "  << versionKey.tag()  << "  " << versionKey.node() << endmsg;
  
  std::string versionName;
  std::string descrName="noDescr";
  if (!m_rdbAccessSvc->getChildTag("DipoleSwitches", versionKey.tag(), versionKey.node(), "FASERDD").empty()) {

    IRDBRecordset_ptr switchSet = m_rdbAccessSvc->getRecordsetPtr("DipoleSwitches", versionKey.tag(), versionKey.node(), "FASERDD");

    const IRDBRecord    *switchTable   = (*switchSet)[0];    
    versionName = switchTable->getString("VERSIONNAME"); 
    if (!switchTable->isFieldNull("DESCRIPTION")) descrName = switchTable->getString("DESCRIPTION");
  }

  if (!m_overrideVersionName.empty()) {
    versionName = m_overrideVersionName;
    msg(MSG::INFO) << "Overriding version name: " << versionName << endmsg;
  }
 
  msg(MSG::INFO) << "Building Dipoles. Version: " << versionName << endmsg;

  // Retrieve the Geometry DB Interface
  sc = m_geometryDBSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not locate Geometry DB Interface: " << m_geometryDBSvc.name() << endmsg;
    return (StatusCode::FAILURE); 
  }  

  // Pass athena services to factory, etc
  m_athenaComps = new DipoleGeoModelAthenaComps;
  m_athenaComps->setDetStore(detStore().operator->());
  m_athenaComps->setGeoDbTagSvc(&*m_geoDbTagSvc);
  m_athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
  m_athenaComps->setGeometryDBSvc(&*m_geometryDBSvc);

  GeoPhysVol *world=&*theExpt->getPhysVol();
  DipoleFactory theDipoles(m_athenaComps);
  theDipoles.create(world);
  m_manager=theDipoles.getDetectorManager();
  if (m_manager) {
    theExpt->addManager(m_manager);
    CHECK( detStore()->record (m_manager, m_manager->getName()) );
  } else {
    msg(MSG::FATAL) << "Could not create DipoleManager!" << endmsg;
    return StatusCode::FAILURE;     
  }
  return result;
}

StatusCode DipoleTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<TrackerDD::DipoleManager>::ID(),m_manager->getName());
  if(proxy) {
    proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}
