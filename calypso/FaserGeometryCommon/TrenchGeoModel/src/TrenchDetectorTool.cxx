/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrenchDetectorTool.h"
#include "TrenchDetectorFactory.h" 
#include "TrenchDetectorManager.h" 

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelFaserUtilities/GeoModelExperiment.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "boost/algorithm/string/predicate.hpp"

TrenchDetectorTool::TrenchDetectorTool( const std::string& type, 
						  const std::string& name,
						  const IInterface* parent )
  : GeoModelTool( type, name, parent ),
    m_manager(0)
{
}

TrenchDetectorTool::~TrenchDetectorTool()
{
}
  

StatusCode TrenchDetectorTool::create()
{ 
  IGeoDbTagSvc *geoDbTag;
  StatusCode sc = service ("GeoDbTagSvc",geoDbTag);
  if(sc.isFailure()) {
    msg(MSG::ERROR) << "Could not locate GeoDbTagSvc" << endmsg;
    return sc;
  }

  IRDBAccessSvc* raccess = 0;
  sc = service("RDBAccessSvc",raccess);
  if(sc.isFailure()) {
    msg(MSG::ERROR) << "Could not locate RDBAccessSvc" << endmsg;
    return sc;
  }

  DecodeFaserVersionKey versionKey(geoDbTag,"Trench");
  // IRDBRecordset_ptr switchSet
  //   = raccess->getRecordsetPtr("TrenchSwitches", versionKey.tag(), versionKey.node(),"FASERDD");
  // const IRDBRecord    *switches   = (*switchSet)[0];
  // msg(MSG::DEBUG) << "Retrieved TrenchSwitches" << endmsg;

  std::string trenchVersion = versionKey.tag();
  msg(MSG::INFO) << "Building Trench geometry version " << trenchVersion << endmsg;
  if(trenchVersion.empty()) {
    msg(MSG::INFO) << "No Trench version for the given configuration. Skip building TrenchGeoModel" << endmsg;
    return StatusCode::SUCCESS;
  }

  std::string versionNode = versionKey.node();

  GeoModelExperiment* theExpt = nullptr;
  if (StatusCode::SUCCESS != detStore()->retrieve(theExpt,"FASER")) { 
    msg(MSG::ERROR) << "Could not find GeoModelExperiment FASER" << endmsg; 
    return StatusCode::FAILURE; 
  } 
 
  GeoPhysVol *world=&*theExpt->getPhysVol();

  if(!m_manager) {
    // If geometry has not been built yet fall back to the default factory
    TrenchDetectorFactory theTrenchFactory(detStore().operator->(),raccess);
    theTrenchFactory.setTagNode(trenchVersion,versionNode);
    theTrenchFactory.create(world);
    m_manager = theTrenchFactory.getDetectorManager();
  }

  if(m_manager) {
    theExpt->addManager(m_manager);
    sc = detStore()->record(m_manager,
			  m_manager->getName());
    if(sc.isFailure()) {
      msg(MSG::ERROR) << "Could not register Trench detector manager" << endmsg;
      return sc;
    }
  }
  else {
    msg(MSG::ERROR) << "ERROR. Failed to build Trench Version " << trenchVersion << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrenchDetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<TrenchDetectorManager>::ID(),m_manager->getName());
  if(proxy) {
    proxy->reset();
    m_manager = 0;
  }
  return StatusCode::SUCCESS;
}

