/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeometryManager.h"

#include "TrackerGeoModelUtils/DistortedMaterialManager.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SiCommonItems.h"
#include "SCT_BarrelParameters.h"
#include "SCT_BarrelModuleParameters.h"
#include "SCT_WrappingParameters.h"
#include "SCT_DataBase.h"
#include "SCT_GeneralParameters.h"
#include "FaserSCT_GeoModel/SCT_GeoModelAthenaComps.h"

SCT_GeometryManager::SCT_GeometryManager(SCT_DataBase* rdb)
  : m_athenaComps{rdb->athenaComps()},
    m_rdb{rdb}
{
  // This class uses reference counting. Should not be delete'd in destructor.
  m_commonItems = new TrackerDD::SiCommonItems(m_athenaComps->getIdHelper());

  m_barrelParameters = std::make_unique<SCT_BarrelParameters>(m_rdb);
  m_barrelModuleParameters = std::make_unique<SCT_BarrelModuleParameters>(m_rdb);
  m_generalParameters = std::make_unique<SCT_GeneralParameters>(m_rdb);
  m_wrappingParameters = std::make_unique<SCT_WrappingParameters>(m_rdb);
  m_distortedMatManager = std::make_unique<TrackerDD::DistortedMaterialManager>();
}

SCT_GeometryManager::~SCT_GeometryManager()
{
}

//
// Access to run time options.
//
const SCT_Options & 
SCT_GeometryManager::options() const
{
  return m_options;
}

void
SCT_GeometryManager::setOptions(const SCT_Options & options)
{
  m_options = options;
}

const SCT_GeoModelAthenaComps *
SCT_GeometryManager::athenaComps() const 
{
  return m_athenaComps;
}
  
//
// SiCommonItems which are passed to SiDetectorElements.
//

const TrackerDD::SiCommonItems *
SCT_GeometryManager::commonItems() const
{
  return m_commonItems;
}

const SCT_BarrelParameters * 
SCT_GeometryManager::barrelParameters() const
{    
  return m_barrelParameters.get();
}

const SCT_BarrelModuleParameters * 
SCT_GeometryManager::barrelModuleParameters() const
{    
  return m_barrelModuleParameters.get();
}

const SCT_GeneralParameters * 
SCT_GeometryManager::generalParameters() const
{    
  return m_generalParameters.get();
}

const SCT_WrappingParameters * 
SCT_GeometryManager::wrappingParameters() const
{    
  return m_wrappingParameters.get();
}


const TrackerDD::DistortedMaterialManager * 
SCT_GeometryManager::distortedMatManager() const
{    
  return m_distortedMatManager.get();
}

SCT_GeometryManager&
SCT_GeometryManager::operator=(const SCT_GeometryManager& right) {
  if (this != &right) {
    m_options = right.m_options;
    m_athenaComps = right.m_athenaComps;
    m_commonItems = right.m_commonItems;
    m_rdb = right.m_rdb;
    m_barrelParameters.reset(new SCT_BarrelParameters(m_rdb));
    m_barrelModuleParameters.reset(new SCT_BarrelModuleParameters(m_rdb));
    m_generalParameters.reset(new SCT_GeneralParameters(m_rdb));
    m_wrappingParameters.reset(new SCT_WrappingParameters(m_rdb));
    m_distortedMatManager.reset(new TrackerDD::DistortedMaterialManager());
  }
  return *this;
}

SCT_GeometryManager::SCT_GeometryManager(const SCT_GeometryManager& right) {
  m_options = right.m_options;
  m_athenaComps = right.m_athenaComps;
  m_commonItems = right.m_commonItems;
  m_rdb = right.m_rdb;
  m_barrelParameters.reset(new SCT_BarrelParameters(m_rdb));
  m_barrelModuleParameters.reset(new SCT_BarrelModuleParameters(m_rdb));
  m_generalParameters.reset(new SCT_GeneralParameters(m_rdb));
  m_wrappingParameters.reset(new SCT_WrappingParameters(m_rdb));
  m_distortedMatManager.reset(new TrackerDD::DistortedMaterialManager());
}
